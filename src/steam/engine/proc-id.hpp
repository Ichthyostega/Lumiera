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
//#include "steam/streamtype.hpp"

#include <string>


namespace steam {
namespace engine {
  namespace err = lumiera::error;
  
  using lib::HashVal;
  using std::string;
  using StrView = std::string_view;
  
  class ProcNode;
  
  class ProcID
    {
      StrView nodeSymb_;
      StrView portQual_;
      StrView argLists_;
      
      ProcID (StrView nodeSymb, StrView portQual, StrView argLists);
      
      using ProcNodeRef = std::reference_wrapper<ProcNode>;
      using Leads = lib::Several<ProcNodeRef>;
      
    public:
      /** build and register a processing ID descriptor */
      static ProcID& describe (StrView nodeSymb, StrView portSpec);
      
      /* === symbolic descriptors === */
      
      string genProcName();
      string genProcSpec();        ///< render a descriptor for the operation (without predecessors)
      string genNodeName();
      string genNodeSpec(Leads&);
      string genSrcSpec (Leads&);  ///< transitively enumerate all unique source nodes
      
      friend bool
      operator== (ProcID const& l, ProcID const& r)
      {
        return l.nodeSymb_ == r.nodeSymb_
           and l.portQual_ == r.portQual_
           and l.argLists_ == r.argLists_;
      }
      
      friend bool
      operator!= (ProcID const& l, ProcID const& r)
      { return not (l == r); }
      
      friend HashVal hash_value (ProcID const&);
    };
  
  
  
  
}} // namespace steam::engine
#endif /*ENGINE_PROC_ID_H*/
