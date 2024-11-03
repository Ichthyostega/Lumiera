/*
  ProcNode  -  Implementation of render node processing

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/


/** @file proc-node.cpp
 ** Translation unit to hold the actual implementation of node processing operations.
 ** 
 ** @todo WIP-WIP-WIP 6/2024 not clear yet what goes here and what goes into turnout-system.cpp
 */


#include "steam/engine/proc-id.hpp"
#include "steam/engine/proc-node.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"

#include <boost/functional/hash.hpp>
#include <unordered_set>


namespace steam {
namespace engine {
  
  using util::unConst;
  
  namespace { // Details...
    
    std::unordered_set<ProcID> procRegistry;
    
  } // (END) Details...
  
  
//  using mobject::Placement;
  
  Port::~Port() { }  ///< @remark VTables for the Port-Turnout hierarchy emitted from \ref proc-node.cpp
  
  
  /**
   * @remark this is the only public access point to ProcID entries,
   *   which are automatically deduplicated and managed in a common registry
   *   and retained until end of the Lumiera process (never deleted).
   */
  ProcID&
  ProcID::describe (StrView nodeSymb, StrView portSpec)
  {
    auto res = procRegistry.insert (ProcID{"bla","blubb","murks"});
    return unConst (*res.first);
  }
  
  /** @internal */
  ProcID::ProcID (StrView nodeSymb, StrView portQual, StrView argLists)
    : nodeSymb_{nodeSymb}  /////////////////////////////////////////////////////////OOO intern these strings!!
    , portQual_{portQual}
    , argLists_{argLists}
    { }
  
  /** generate registry hash value based on the distinct data in ProcID.
   *  This function is intended to be picked up by ADL, and should be usable
   *  both with `std::hash` and `<boost/functional/hash.hpp>`.
   */
  HashVal
  hash_value (ProcID const& procID)
  {
    return 47; //UNIMPLEMENTED ("ProcID hash");
  }
  
  
  string
  ProcNodeDiagnostic::getNodeSpec()
  {
    UNIMPLEMENTED ("generate a descriptive Spec of this ProcNode for diagnostics");
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
    return p.size() < portIdx? util::FAILURE_INDICATOR
                             : p[portIdx].procID.genProcSpec();
  }
  
  HashVal
  ProcNodeDiagnostic::getPortHash (uint portIdx)
  {
    UNIMPLEMENTED ("calculate an unique, stable and reproducible hash-key to identify the Turnout");
  }
  
  
}} // namespace steam::engine
