/*
  PROC-ID.hpp  -  symbolic and hash identification of processing steps

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file proc-id.hpp
 ** Metadata interface to generate symbolic and hash ID tags for media processing steps.
 ** Functionality is provided to identify a point in the processing chain for sake of
 ** error reporting and unit testing; moreover, identifying information can be chained
 ** and combined into a systematic hash key, to serve as foundation for a stable cache key.
 ** @warning The specification expresses semantic distinctions and it is the responsibility
 **          of the media-processing-library binding plug-in to ensure that classification
 **          matches relevant semantic distinctions. In this context, "different" means
 **          that two functions produce _perceptibly different results_ — which also
 **          implies that for equivalent IDs we can use cached calculation results.
 ** \par Code arrangement
 **    - proc-node.hpp is a shallow interface header (included rather widely)
 **    - proc-id.hpp (this header) details node specifications and is used for the NodeBuidler
 **    - proc-node.cpp acts as »module« and anchor for all implementation services
 ** 
 ** ## Structure and syntax
 ** A complete processing-specification combines a high-level identification of the enclosing
 ** Node with qualifiers to describe a specific functionality variant for a given Port, together
 ** with the structure of the input and output argument lists, and a set of additional, extended
 ** attributes. Any aspects should be recorded here, if they might cause differences in computed
 ** results or are relevant for some further information function (used for diagnostic, engine
 ** instrumentation and job planning)
 ** - the _Node symbol_ is related to the [processing asset](\ref ProcAsset) and is expected
 **   to be structured as `<ontology>:<semanticID>`, e.g. `FFmpeg:gaussianBlur`
 ** - the _Port qualifier_ accounts for specific configuration applied for the given port
 ** - the _Argument lists_ should follow the pattern `[(inType, ...)](outType,...)`, with the
 **   allowed shorthand notation `<type>/N` to designate N identical arguments of type `type`.
 ** For a complete _processing spec,_ the node symbol is possible decorated with a dot and
 ** the port qualifier (if present), then followed by the argument lists.
 ** 
 ** ## Hash computation
 ** Hash-IDs are derived from the full processing spec,_ but also from individual parts alone
 ** for some use cases (e.g. dispatch of information functions). Furthermore, the Hash-IDs of
 ** all Nodes in a processing chain can be combined into a Hash-ID of the chain, which is usable
 ** as cache key: when the hash-ID matches, a cached result can be used instead of re-computation.
 ** @todo Can we assume size_t == 64bit; is this enough to prevent collisions?
 **       Can we afford using a 128bit hash? What about portability of hash values?
 ** @todo WIP-WIP as of 10/2024 this is a draft created as an aside while working towards
 **       the first integration of render engine functionality //////////////////////////////////////////////TICKET #1377 : establish a systematic processing identification
 ** @remark the essential requirement for a systematic and stable cache key is
 **       - to be be re-generated directly from the render node network
 **       - to be differing if and only if the underlying processing structure changes
 ** 
 ** @see turnout.hpp
 ** @see engine::ProcNodeDiagnostic
 ** @see proc-node.cpp for the implementation backend
 */

#ifndef ENGINE_PROC_ID_H
#define ENGINE_PROC_ID_H


#include "lib/error.hpp"
#include "lib/hash-standard.hpp"
#include "lib/several.hpp"
#include "lib/nocopy.hpp"

#include <utility>
#include <string>


namespace steam {
namespace engine {
  namespace err = lumiera::error;
  
  using std::move;
  using lib::HashVal;
  using std::string;
  using StrView = std::string_view;
  
  class ProcNode;
  
  
  /**
   * Extended Attributes for ProcID metadata.
   * @remark used for cache key calculation and
   *         to dispatch information functions.
   * @todo if this grows beyond size_t, it should be
   *       deduplicated and stored in a registry,
   *       similar to the string spec. Storage matters!
   */
  struct ProcAttrib
    {
      bool manifold :1;
      bool isProxy  :1;
      
      ProcAttrib()
        : manifold{true}
        , isProxy{false}
        { }
      
      friend bool
      operator== (ProcAttrib const& l, ProcAttrib const& r)
      {
        return l.manifold == r.manifold
           and l.isProxy  == r.isProxy;
      }
    };
  
  /**
   * Metadata to qualify a Port (and implicitly the enclosing Node).
   * @note must be essentially immutable; should ensure that implementation
   *       never changes anything constituent for the \ref hash_value(),
   *       due to de-duplication into a hashtable (see proc-node.cpp).
   * @warning be sure always to take a reference to the instance emplaced
   *       into the `procRegistry` (see proc-node.cpp); inadvertently taking
   *       a reference to some transient ProcID value leads to insidious errors!
   */
  class ProcID
    : util::MoveOnly   // ◁—— you must not create instances, use ProcID::describe()
    {
      StrView nodeName_;
      StrView portQual_;
      StrView argLists_;
      ProcAttrib attrib_;
      
      ProcID (StrView nodeSymb, StrView portQual, StrView argLists, ProcAttrib);
      
      using ProcNodeRef = std::reference_wrapper<ProcNode>;
      using Leads = lib::Several<ProcNodeRef>;
      
    public:
      /** build and register a processing ID descriptor */
      static ProcID& describe (StrView nodeSymb, StrView portSpec, ProcAttrib extAttrib =ProcAttrib{});
      
      /* === symbolic descriptors === */
      
      string genProcName()       const;
      string genProcSpec()       const;  ///< render a descriptor for the operation (without predecessors)
      string genQualifier()      const;
      string genNodeName()       const;
      string genNodeSymbol()     const;
      string genNodeDomain()     const;
      string genNodeSpec(Leads&) const;
      string genSrcSpec (Leads&) const;  ///< transitively enumerate all unique source nodes
      
      struct ArgModel;
      ArgModel genArgModel()     const;
      
      bool hasManifoldPatt()     const { return attrib_.manifold; }
      bool hasProxyPatt()        const { return attrib_.isProxy; }
      
      friend bool
      operator== (ProcID const& l, ProcID const& r)
      {
        return l.nodeName_ == r.nodeName_
           and l.portQual_ == r.portQual_
           and l.argLists_ == r.argLists_
           and l.attrib_   == r.attrib_;
      }
      
      friend bool
      operator!= (ProcID const& l, ProcID const& r)
      { return not (l == r); }
      
      friend HashVal hash_value (ProcID const&);
    };
  
  
  
  /**
   * Expanded information regarding node input and output.
   * Requires [parsing the spec](\ref ProcID::genArgModel) for construction.
   */
  struct ProcID::ArgModel
    : util::MoveAssign
    {
      using Strings = lib::Several<const string>;
      using iterator = Strings::iterator;
      
      Strings iArg;
      Strings oArg;
      
      bool empty()      const { return not hasArgs(); };
      bool hasArgs()    const { return hasInArgs() or hasOutArgs();}
      bool hasInArgs()  const { return not iArg.empty(); }
      bool hasOutArgs() const { return not oArg.empty(); }
      uint inArity()    const { return iArg.size(); }
      uint outArity()   const { return oArg.size(); }
      
    private:
      ArgModel (Strings&& iarg, Strings&& oarg)
        : iArg{move (iarg)}
        , oArg{move (oarg)}
        { }
      friend ArgModel ProcID::genArgModel()  const;
    };
  
  
}} // namespace steam::engine
#endif /*ENGINE_PROC_ID_H*/
