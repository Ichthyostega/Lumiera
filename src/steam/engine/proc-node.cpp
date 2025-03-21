/*
  ProcNode  -  Implementation of render node processing

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file proc-node.cpp
 ** Translation unit for implementation details regarding node-IDs and verification.
 ** @remark \ref ProcNode itself is a shell to provide a node-ID and a high-level API
 **         for Render Node invocation. The actual implementation of processing functionality
 **         is located within the [Turnout](\ref turnout.hpp) and the individual »weaving patterns«
 **         embedded therein.
 */


#include "steam/engine/proc-id.hpp"
#include "steam/engine/proc-node.hpp"
#include "lib/several-builder.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/format-string.hpp"
#include "lib/format-util.hpp"
#include "lib/parse.hpp"
#include "lib/util.hpp"

// used for a »backdoor access« in PortDiagnostic::srcPorts
#include "steam/engine/media-weaving-pattern.hpp"
#include "steam/engine/param-weaving-pattern.hpp"

#include <boost/functional/hash.hpp>        /////////////////////////////////////////////////////TICKET #1391 is boost-hash the proper tool for this task?
#include <unordered_set>
#include <set>


namespace steam {
namespace engine {
  
  using lib::explore;
  using util::_Fmt;
  using util::isnil;
  using util::unConst;
  using util::contains;
  using util::isSameObject;
  using boost::hash_combine;
  
  namespace {// Details: parsing, registration and symbol table for node spec data...
    
    std::unordered_set<ProcID> procRegistry;
    std::unordered_set<string> symbRegistry;
    
    /** deduplicate and re-link to the entry in the symbol table */
    void inline
    dedupSymbol (StrView& symbol)
    {
      auto res = symbRegistry.emplace (symbol);
      symbol = *res.first;
    }
    
    /* ===== Parse nested spec ===== */
    
    using util::parse::accept;
    using util::parse::accept_bracket;
    using util::parse::accept_repeated;
    using util::parse::expectResult;
    using lib::meta::NullType;
    using std::regex_match;
    using std::regex;
    
    const regex SPEC_CONTENT{R"_([^,\\\(\)\[\]{}<>"]+)_", regex::optimize};
    const regex NON_QUOTE   {R"_([^"\\]+)_"             , regex::optimize};
    const regex ESCAPE      {R"_(\\.)_"                 , regex::optimize};
    const regex COMMA       {R"_(,)_"                   , regex::optimize};
    const regex D_QUOTE     {R"_(")_"                   , regex::optimize};
    
    auto quoted = accept_repeated(accept(NON_QUOTE).alt(ESCAPE));
    auto quote  = accept_bracket(D_QUOTE,D_QUOTE, quoted);
    
    template<char OPE, char CLO>
    auto&
    syntaxBracketed()
    {
      string esc{"\\"};
      regex OPENING{esc+OPE};
      regex CLOSING{esc+CLO};
      regex NON_PAREN{R"_([^\\)_"+esc+OPE+esc+CLO+"]+"};
      
      static auto paren = expectResult<NullType>();
      auto parenContent = accept_repeated(accept(NON_PAREN)
                                            .alt(ESCAPE)
                                            .alt(quote)
                                            .alt(paren));
      
      paren = accept_bracket(OPENING,CLOSING, parenContent).bind([](auto){ return NullType{}; });
      return paren;
    }
    
    auto specTermSyntax = accept_repeated(accept(SPEC_CONTENT)
                                            .alt(ESCAPE)
                                            .alt(quote)
                                            .alt(syntaxBracketed<'(',')'>())
                                            .alt(syntaxBracketed<'<','>'>())
                                            .alt(syntaxBracketed<'[',']'>())
                                            .alt(syntaxBracketed<'{','}'>())
                                         )
                                         .bindMatch();
    
    
    const regex REPEAT_SPEC {R"_(^(.+)\s*/(\d+)\s*$)_", regex::optimize};
    
    /**
     * Helper to expand an abbreviated repetition of arguments.
     * Implemented as custom-processing layer for IterExplorer,
     * by adapting the »State Core« interface
     * @remark Repetition is indicated by a trailing "/NUM"
     */
    template<class IT>
    struct RepetitionExpander
      : lib::IterStateCore<IT>
      {
        using Core = lib::IterStateCore<IT>;
        
        mutable uint repeat_{0};
        mutable std::smatch mat_;
        
        StrView
        yield()  const
          {
            if (not repeat_)
              {     //  check if the next string ends with repetition marker /NUM
                if (not regex_match (*Core::srcIter(), mat_, REPEAT_SPEC))
                  return *Core::srcIter(); // no repetition -> pass through
                
                // setup repetition by extracting the repetition count
                repeat_ = boost::lexical_cast<uint>(mat_.str(2));
              }
            return StrView(& *mat_[1].first, mat_[1].length());
          }     // several repetitions created from same source
        
        void
        iterNext()
          {  // hold iteration until all repetitions were delivered
            if (repeat_)
              --repeat_;
            if (not repeat_)
              ++ Core::srcIter();
          }
        
        using Core::Core;
      };
    
    /** Argument-Spec processing pipeline;
     *  possibly expands repetition abbreviation,
     *  collects all argument strings into a lib::Several
     */
    lib::Several<const string>
    evaluateArgSeq (std::vector<string>& parsedArgTerms)
    {
      auto several = lib::makeSeveral<const string>();
      lib::explore (parsedArgTerms)
          .processingLayer<RepetitionExpander>()
          .foreach([&](StrView s){ several.emplace<string>(s); });
      return several.build();
    }
    
    auto
    emptyArgSeq()
    {
      return lib::Several<const string>();
    }
    
  } // (END) Details...
  
  
  
  Port::~Port() { }  ///< @remark VTables for the Port-Turnout hierarchy emitted from \ref proc-node.cpp
  
  
  /**
   * @remark this is the only public access point to ProcID entries,
   *   which are automatically deduplicated and managed in a common registry
   *   and retained until end of the Lumiera process (never deleted).
   * @todo isn't returning a non-const reference dangerous? someone might add
   *   mutable state then, thereby undercutting de-duplication into a hashtable.
   */
  ProcID&
  ProcID::describe (StrView nodeSymb, StrView portSpec, ProcAttrib extAttrib)
  {
    REQUIRE (not isnil (nodeSymb));
    REQUIRE (not isnil (portSpec));
    REQUIRE (not contains (nodeSymb, ' '));
    auto p = portSpec.find('(');
    if (p == string::npos)
      throw err::Invalid{_Fmt{"Spec for processing operation must contain at least one argument list. "
                              "Node:%s Spec:%s"}
                             % nodeSymb % portSpec
                        };
    auto res = procRegistry.emplace (ProcID{nodeSymb, portSpec.substr(0,p), portSpec.substr(p), extAttrib});
    ProcID& entry{unConst (*res.first)};
    if (res.second)
      {// new record placed into the registry
        dedupSymbol (entry.nodeName_);
        dedupSymbol (entry.argLists_);
        if (not isnil(entry.portQual_))
          dedupSymbol (entry.portQual_);
      }
    return entry;
  }
  
  /** @internal */
  ProcID::ProcID (StrView nodeSymb, StrView portQual, StrView argLists, ProcAttrib extAttrib)
    : nodeName_{nodeSymb}
    , portQual_{portQual}
    , argLists_{argLists}
    , attrib_{extAttrib}
    { }
  
  /**
   * generate registry hash value based on the distinct data in ProcID.
   * This function is intended to be picked up by ADL, and should be usable
   * both with `std::hash` and `<boost/functional/hash.hpp>`.
   */
  HashVal
  hash_value (ProcID const& procID)
  {
    HashVal hash = boost::hash_value (procID.nodeName_);  ///////////////////////////////////////////////////TICKET #1391 : which technology to use for processing-ID hashes -> cache keys?
    if (not isnil(procID.portQual_))
      hash_combine (hash, procID.portQual_);         ////////////////////////////////////////////////////////TICKET #1391 : should use lib/hash-combine.hpp (stable, but not portable!)
    hash_combine   (hash, procID.argLists_);
    return hash;
  }
  
  string
  ProcID::genProcName()  const
  {
    std::ostringstream buffer;
    buffer << genNodeSymbol()
           << genQualifier();
    return buffer.str();
  }
  
  string
  ProcID::genProcSpec()  const
  {
    std::ostringstream buffer;
    buffer << genNodeSymbol()
           << genQualifier()
           << argLists_;
    return buffer.str();
  }
  
  string
  ProcID::genNodeName()  const
  {
    return string{nodeName_};
  }
  
  string
  ProcID::genNodeSymbol()  const
  {
    auto p = nodeName_.find(':');
    return p == string::npos? string{nodeName_}
                            : string{nodeName_.substr(p+1)};
  }
  
  string
  ProcID::genNodeDomain()  const
  {
    auto p = nodeName_.find(':');
    return p == string::npos? string{}
                            : string{nodeName_.substr(0,p)};
  }
  
  string
  ProcID::genQualifier()  const
  {
    std::ostringstream buffer;
    if (not isnil(portQual_))
      buffer << '.' << portQual_;
    return buffer.str();
  }
  
  
  namespace { // Helper to access ProcID recursively
    ProcID&
    procID (ProcNode& node)
    {
      REQUIRE (not isnil(watch(node).ports()));
      return watch(node).ports().front().procID;
    }
  }
  
  string
  ProcID::genNodeSpec (Leads& leads)  const
  {
    std::ostringstream buffer;
    buffer << nodeName_;
    if (1 != leads.size())
      buffer << genSrcSpec(leads);
    else
      { // single chain....
        ProcNode& p{leads.front().get()};
        buffer << "◁—"
               << procID(p).genNodeName()      // show immediate predecessor
               << procID(p).genSrcSpec(       //  ...followed by it's source(s)
                              watch(p).leads());
      }
    return buffer.str();
  }
  
  string
  ProcID::genSrcSpec (Leads& leads)  const
  {
    return isnil(leads)? string{"-◎"}  // no leads => starting point itself is a source node
                       : "┉┉{"
                         + util::join(
                             explore(leads)
                               .expandAll([](ProcNode& n){ return explore(watch(n).leads()); })  // depth-first expand all predecessors
                               .filter   ([](ProcNode& n){ return watch(n).isSrc(); })           // but retain only leafs (≙ source nodes)
                               .transform([](ProcNode& n){ return procID(n).nodeName_;})         // render the namespace and name of each src
                               .deduplicate())                                                   // sort and deduplicate
                         + "}";
  }
  
  
  /** parse and dissect the argument specification */
  ProcID::ArgModel
  ProcID::genArgModel()  const
  {
    auto argListSyntax = accept_bracket(accept_repeated(0,MAX_NODE_ARG, COMMA, specTermSyntax));
    auto argSpecSyntax = accept(argListSyntax)
                           .opt(argListSyntax)
                                             .bind([](auto model) -> ProcID::ArgModel
                                                    {
                                                      auto [list1,list2] = model;
                                                      if (list2)
                                                        return {evaluateArgSeq(list1), evaluateArgSeq(*list2)};
                                                      else
                                                        return {emptyArgSeq(), evaluateArgSeq(list1)};
                                                    });
    
    argSpecSyntax.parse (argLists_);
    if (not argSpecSyntax.success())
      throw err::Invalid{_Fmt{"Unable to parse argument list. "
                              "Node:%s Spec:%s"}
                             % genProcName() % argLists_
                        };
    return argSpecSyntax.extractResult();
  }
  
  
  
  /**
   * @return symbolic string with format `NodeSymb--<predecessorSpec>`
   * @remark connectivity information is abbreviated and foremost
   *         indicates the data source(s)
   */
  string
  ProcNodeDiagnostic::getNodeSpec()
  {
    REQUIRE (not isnil(ports()));
    return ports().front().procID.genNodeSpec (leads());
  }
  
  string
  ProcNodeDiagnostic::getNodeName()
  {
    REQUIRE (not isnil(ports()));
    return ports().front().procID.genNodeName();
  }
  
  HashVal
  ProcNodeDiagnostic::getNodeHash() ///< @todo not clear yet if this has to include predecessor info
  {
    UNIMPLEMENTED ("calculate an unique hash-key to designate this node");
  }
  
  /**
   * @return symbolic string with format `NodeSymb[.portQualifier](inType[/#][,inType[/#]])(outType[/#][,outType[/#]][ >N])`
   * @remark information presented here is passed-through from builder Level-3, based on semantic markup present there
   */
  string
  ProcNodeDiagnostic::getPortSpec (uint portIdx)
  {
    auto& p{n_.wiring_.ports};
    return portIdx < p.size()? p[portIdx].procID.genProcSpec()
                             : util::FAILURE_INDICATOR;
  }
  
  HashVal
  ProcNodeDiagnostic::getPortHash (uint portIdx)
  {
    UNIMPLEMENTED ("calculate an unique, stable and reproducible hash-key to identify the Turnout");
  }
  
  
  
  namespace {// create a »backdoor access« into actual weaving-pattern instances
    
    using _DummyProc = void(&)(NullType*);
    using _DummyProto = FeedPrototype<_DummyProc>;
    using _DummyMediaWeaving = MediaWeavingPattern<_DummyProto>;
    using _RecastMediaWeaving = _TurnoutDiagnostic<_DummyMediaWeaving>;
    
    using _EmptySpec = decltype(buildParamSpec());
    using _DummyParamWeaving = ParamWeavingPattern<_EmptySpec>;
    using _RecastParamWeaving = _TurnoutDiagnostic<_DummyParamWeaving>;
    
    lib::Several<PortRef> EMPTY_PRECURSORS;
  }
  
  /**
   * Intrude into the Turnout and find out about source connectivity.
   * At interface level, this information about predecessor ports is not retained,
   * but for the most common weaving patterns (Port implementations) there is a way
   * to access implementation internals, bypassing the \ref Port interface; otherwise
   * a reference to an empty port collection is returned.
   * @warning this is a possibly dangerous low-level access, bypassing type safety.
   *   It relies on flags in the ProcID attributes to be set properly by the builder,
   *   and it relies on a common shared prefix in the memory layout of weaving patterns.
   * @remark the \ref Port interface is kept minimal, since a very large number of
   *   implementations and template instantiations can be expected, so that any further
   *   function would cause a lot of additional and mostly redundant code generation. 
   */
  lib::Several<PortRef> const&
  PortDiagnostic::srcPorts()
  {
    if (p_.procID.hasManifoldPatt())
      {
        auto [leads,types] = _RecastMediaWeaving::accessInternal (p_);
        return leads;
      }
    else
    if (p_.procID.hasProxyPatt())
      {
        Port& delegate = std::get<0>(_RecastParamWeaving::accessInternal (p_));
        return watch(delegate).srcPorts();
      }                     // recursive invocation on delegate of proxy
    else
      return EMPTY_PRECURSORS;
  }
  
  /**
   * @return the symbolic string representing this processing port,
   *         as [provided by Node-identification](\ref ProcID::genProcSpec())
   */
  string
  PortDiagnostic::getProcSpec()
  {
    return p_.procID.genProcSpec();
  }
  
  string
  PortDiagnostic::getProcName()
  {
    return p_.procID.genProcName();
  }
  
  HashVal
  PortDiagnostic::getProcHash()  ///< @return as [calculated by Node-identification](\ref ProcID)
  {
    UNIMPLEMENTED ("calculate an unique, stable and reproducible hash-key to identify the Turnout");
  }
  
  
  /* === cross-navigation === */
  
  ProcNodeDiagnostic
  ProcNodeDiagnostic::watchLead (uint leadIdx)
  {
    if (leadIdx >= leads().size())
      throw err::Invalid{_Fmt{"Lead-# %d >= %d (available lead-nodes)."}
                             %     leadIdx  % leads().size()};
    return watch (leads()[leadIdx]);
  }
  
  PortDiagnostic
  ProcNodeDiagnostic::watchPort (uint portIdx)
  {
    if (portIdx >= ports().size())
      throw err::Invalid{_Fmt{"Port-idx %d >= %d (available Ports)."}
                             %     portIdx  % ports().size()};
    return watch (ports()[portIdx]);
  }
  
  PortDiagnostic
  PortDiagnostic::watchLead (uint leadIdx)
  {
    auto& leadPorts = srcPorts();
    if (leadIdx >= leadPorts.size())
      throw err::Invalid{_Fmt{"Lead-Port# %d >= %d (available src-ports)."}
                             %       leadIdx  % leadPorts.size()};
    return watch (leadPorts[leadIdx]);
  }
  
  bool
  PortDiagnostic::verify_connected (uint input, Port& tarPort)
  {
    auto& leadPorts = srcPorts();
    return input < leadPorts.size()
       and leadPorts[input] == tarPort;
  }
  
  bool
  PortDiagnostic::verify_connected (Port& tarPort)
  {
    for (Port& port : srcPorts())
      if (port == tarPort)
        return true;
    return false;
  }
  
  
  /**
   * @remark _ConCheck provides a fluent DSL notation to verify node connectivity.
   *   This is achieved by first collecting some counterparts and index specifications
   *   for the kind of connection to validate. Each qualifier just sets a parameter and
   *   returns the _ConCheck object by move. The final result is retrieved by this bool
   *   conversion — which in fact has to implement a collection of different evaluations.
   *   The proper kind of evaluation will be picked based on the actual arguments given;
   *   the more are present, the more constricted the evaluation becomes. The selection
   *   of the evaluation to take is thus ordered in reverse order, starting with the
   *   most constricted cases. Three different kinds of link validations are provided
   *   - compare two fully qualified Port objects for identity (same object)
   *   - check if a given Port object is present in a collection of Ports
   *   - exhaustive search for a match in the cross product of two Port collections.
   *   The last case is what allows to perform a high-level connectivity test between
   *   two nodes, which are considered as connected if any link is found.
   * @see NodeMeta_test::verify_ID_connectivity()
   */
  _ConCheck::operator bool()
  {
    auto validPort = [this](uint idx) { return idx < anchor.ports().size(); };
    auto validLead = [this](uint idx) { return idx < anchor.leads().size(); };
    auto validSrc  = [this](uint pNo
                           ,uint sNo) { return sNo < anchor.watchPort(pNo).srcPorts().size(); };
    auto validSrcP = [this](ProcNode& lead
                           ,uint idx) { return idx < watch(lead).ports().size(); };
    
    auto find_link = [](auto& seq1, auto& seq2)
                            {
                              return explore(seq1)
                                       .transform([&](auto& elm){ return contains (seq2, *elm); })
                                       .has_any();
                            };
    
     // Determine case to handle,
    //  starting with the most constricted...
    if (portNo and srcNo and srcNode and srcPNo)
      return validPort(*portNo)                                // is_linked(node).port(portNo).asSrc(srcNo).to(srcNode).atPort(srcPNo)
         and validSrc (*portNo, *srcNo)
         and validSrcP(*srcNode,*srcPNo)
         and anchor.watchPort(*portNo).srcPorts()[*srcNo]
             == watch(*srcNode).ports()[*srcPNo];
    else
    if (portNo and srcNo and leadNo and srcPNo)
      return validPort(*portNo)                                // is_linked(node).port(portNo).asSrc(srcNo).toLead(leadNo).atPort(srcPNo)
         and validSrc (*portNo,*srcNo)
         and validLead(*leadNo)
         and validSrcP(anchor.leads()[*leadNo], *srcPNo)
         and anchor.watchPort(*portNo).srcPorts()[*srcNo]
             == anchor.watchLead(*leadNo).ports()[*srcPNo];
    else
    if (portNo and srcNo and srcPort)
      return validPort(*portNo)                                // is_linked(node).port(portNo).asSrc(srcNo).to(srcPort)
         and validSrc (*portNo,*srcNo)
         and anchor.watchPort(*portNo).srcPorts()[*srcNo]
             == *srcPort;
    else
    if (portNo and srcNo and srcNode)
      return validPort(*portNo)                                // is_linked(node).port(portNo).asSrc(srcNo).to(srcNode)
         and validSrc (*portNo,*srcNo)
         and contains (watch(*srcNode).ports()
                      ,anchor.watchPort(*portNo).srcPorts()[*srcNo]);
    else
    if (portNo and srcNo and leadNo)
      return validPort(*portNo)                                // is_linked(node).port(portNo).asSrc(srcNo).toLead(leadNo)
         and validSrc (*portNo,*srcNo)
         and validLead(*leadNo)
         and contains (anchor.watchLead(*leadNo).ports()
                      ,anchor.watchPort(*portNo).srcPorts()[*srcNo]);
    else
    if (portNo and srcNo)
      return validPort(*portNo)                                // is_linked(node).port(portNo).asSrc(srcNo)
         and validSrc (*portNo,*srcNo);
    else
    if (portNo and srcNode and srcPNo)
      return validPort(*portNo)                                // is_linked(node).port(portNo).to(srcNode).atPort(srcPNo)
         and validSrcP(*srcNode,*srcPNo)
         and contains (anchor.watchPort(*portNo).srcPorts()
                      ,watch(*srcNode).ports()[*srcPNo]);
    else
    if (portNo and leadNo and srcPNo)
      return validPort(*portNo)                                // is_linked(node).port(portNo).toLead(leadNo).atPort(srcPNo)
         and validLead(*leadNo)
         and validSrcP(anchor.leads()[*leadNo], *srcPNo)
         and contains (anchor.watchPort(*portNo).srcPorts()
                      ,anchor.watchLead(*leadNo).ports()[*srcPNo]);
    else
    if (portNo and srcPort)
      return validPort(*portNo)                                // is_linked(node).port(portNo).to(srcPort)
         and contains (anchor.watchPort(*portNo).srcPorts()
                      ,*srcPort);
    else
    if (portNo and srcNode)
      return validPort(*portNo)                                // is_linked(node).port(portNo).to(srcNode)
         and find_link(watch(*srcNode).ports()
                      ,anchor.watchPort(*portNo).srcPorts());
    else
    if (portNo and leadNo)
      return validPort(*portNo)                                // is_linked(node).port(portNo).toLead(leadNo)
         and validLead(*leadNo)
         and find_link(anchor.watchLead(*leadNo).ports()
                      ,anchor.watchPort(*portNo).srcPorts());
    else
    if (portNo)
      return validPort(*portNo)                                // is_linked(node).port(portNo)
         and not anchor.watchPort(*portNo).isSrc();
    else
    if (srcNode and leadNo)
      return validLead(*leadNo)                                // is_linked(node).to(srcNode).asLead(leadNo)
         and anchor.leads()[*leadNo]
             == *srcNode;
    else
    if (srcNode)
      return contains (anchor.leads()                          // is_linked(node).to(srcNode)
                      ,*srcNode);
    return false;
  }
  
  
}} // namespace steam::engine
