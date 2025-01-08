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
 ** Translation unit to hold the actual implementation of node processing operations.
 ** 
 ** @todo WIP-WIP-WIP 6/2024 not clear yet what goes here and what goes into turnout-system.cpp
 */


#include "steam/engine/proc-id.hpp"
#include "steam/engine/proc-node.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/format-string.hpp"
#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include <boost/functional/hash.hpp>
#include <unordered_set>
#include <set>


namespace steam {
namespace engine {
  
  using lib::explore;
  using util::_Fmt;
  using util::isnil;
  using util::unConst;
  using util::contains;
  using boost::hash_combine;
  
  namespace {// Details: registration and symbol table for node spec data...
    
    std::unordered_set<ProcID> procRegistry;
    std::unordered_set<string> symbRegistry;
    
    /** deduplicate and re-link to the entry in the symbol table */
    void inline
    dedupSymbol (StrView& symbol)
    {
      auto res = symbRegistry.emplace (symbol);
      symbol = *res.first;
    }
  } // (END) Details...
  
  
  
  Port::~Port() { }  ///< @remark VTables for the Port-Turnout hierarchy emitted from \ref proc-node.cpp
  
  
  /**
   * @remark this is the only public access point to ProcID entries,
   *   which are automatically deduplicated and managed in a common registry
   *   and retained until end of the Lumiera process (never deleted).
   */
  ProcID&
  ProcID::describe (StrView nodeSymb, StrView portSpec)
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
    auto res = procRegistry.insert (ProcID{nodeSymb, portSpec.substr(0,p), portSpec.substr(p)});
    ProcID& entry{unConst (*res.first)};
    if (res.second)
      {// new record placed into the registry
        dedupSymbol (entry.nodeSymb_);
        dedupSymbol (entry.argLists_);
        if (not isnil(entry.portQual_))
          dedupSymbol (entry.portQual_);
      }
    return entry;
  }
  
  /** @internal */
  ProcID::ProcID (StrView nodeSymb, StrView portQual, StrView argLists)
    : nodeSymb_{nodeSymb}
    , portQual_{portQual}
    , argLists_{argLists}
    { }
  
  /**
   * generate registry hash value based on the distinct data in ProcID.
   * This function is intended to be picked up by ADL, and should be usable
   * both with `std::hash` and `<boost/functional/hash.hpp>`.
   */
  HashVal
  hash_value (ProcID const& procID)
  {
    HashVal hash = boost::hash_value (procID.nodeSymb_);
    if (not isnil(procID.portQual_))
      hash_combine (hash, procID.portQual_);
    hash_combine   (hash, procID.argLists_);
    return hash;
  }
  
  string
  ProcID::genProcName()
  {
    std::ostringstream buffer;
    buffer << nodeSymb_;
    if (not isnil(portQual_))
      buffer << '.' << portQual_;
    return buffer.str();
  }
  
  string
  ProcID::genProcSpec()
  {
    std::ostringstream buffer;
    buffer << nodeSymb_;
    if (not isnil(portQual_))
      buffer << '.' << portQual_;
    buffer << argLists_;
    return buffer.str();
  }
  
  string
  ProcID::genNodeName()
  {
    return string{nodeSymb_};
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
  ProcID::genNodeSpec (Leads& leads)
  {
    std::ostringstream buffer;
    buffer << nodeSymb_;
    if (1 != leads.size())
      buffer << genSrcSpec(leads);
    else
      { // single chain....
        ProcNode& p{leads.front().get()};
        buffer << "◁—"
               << procID(p).genNodeName()      // show immediate predecessor
               << procID(p).genSrcSpec(leads); // and behind that recursively the source(s)
      }
    return buffer.str();
  }
  
  string
  ProcID::genSrcSpec (Leads& leads)
  {
    return isnil(leads)? string{"-◎"}  // no leads => starting point itself is a source node
                       : "┉┉{"
                         + util::join(
                             explore(leads)
                               .expandAll([](ProcNode& n){ return explore(watch(n).leads()); })  // depth-first expand all predecessors
                               .filter   ([](ProcNode& n){ return watch(n).isSrc(); })           // but retain only leafs (≙ source nodes)
                               .transform([](ProcNode& n){ return procID(n).nodeSymb_;})         // render the node-symbol of each src
                               .deduplicate())                                                   // sort and deduplicate
                         + "}";
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
  
  
  lib::Several<PortRef>
  PortDiagnostic::srcPorts()
  {
    UNIMPLEMENTED ("intrude into the Turnout and find out about source connectivity");
  }
  
  /**
   * @return the symbolic string representing this processing port,
   *         as [provided by Node-identification](\ref ProcID::genProcSpec())
   */
  string
  PortDiagnostic::getProcSpec()
  {
    p_.procID.genProcSpec();
  }
  
  HashVal
  PortDiagnostic::getProcHash()  ///< @return as [calculated by Node-identification](\ref ProcID)
  {
    UNIMPLEMENTED ("calculate an unique, stable and reproducible hash-key to identify the Turnout");
  }
  
  
}} // namespace steam::engine
