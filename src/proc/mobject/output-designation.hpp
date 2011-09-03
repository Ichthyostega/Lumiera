/*
  OUTPUT-DESIGNATION.hpp  -  specifying a desired output destination

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

*/


#ifndef PROC_MOBJECT_OUTPUT_DESIGNATION_H
#define PROC_MOBJECT_OUTPUT_DESIGNATION_H

#include "proc/asset/pipe.hpp"
#include "lib/opaque-holder.hpp"
#include "lib/meta/typelist-util.hpp"

extern "C" {
#include "lib/luid.h"
}

namespace mobject {
  
  namespace mp = lumiera::typelist;
  
  class MObject;
  
  template<class MX>
  class PlacementRef;
  
  typedef PlacementRef<MObject> RefPlacement;
  
  
  
  /**
   * Descriptor to denote the desired target of produced media data.
   * OutputDesignation is always an internal and relative specification
   * and boils down to referring an asset::Pipe by ID. In order to become
   * actually effective, some object within the model additionally
   * needs to \em claim this pipe-ID, meaning that this object
   * states to root and represent this pipe. When the builder
   * encounters a pair of (OutputDesignation, OutputClaim),
   * an actual stream connection will be wired in the
   * processing node network.
   * 
   * @todo couldn't the inline buffer be "downgraded" to InPlaceBuffer or PolymorphicValue??
   *       Seemingly we never-ever need to re-discover the erased type of the embedded spec.
   *       Thus for this to work, we'd just need to add an "empty" spec       ///////////////////TICKET #723
   * 
   * @see OutputMapping_test
   */
  class OutputDesignation
    {
    public:
      typedef asset::ID<asset::Pipe> PID;
      typedef asset::PPipe PPipe;
      
      explicit OutputDesignation (PID explicitTarget);
      explicit OutputDesignation (RefPlacement const& indirectTarget); 
      explicit OutputDesignation (uint relative_busNr =0);
      
      // using default copying
      
      
      /** retrieve the direct destination
       *  this descriptor is actually pointing to.
       *  In case of a target pipe not explicitly specified
       *  this might involve a resolution step and take the 
       *  current context into account. 
       * @param origin starting point for figuring out connections
       * @return a pipe-ID, which should be used as next connection.
       *         This might not be the final designation, but the
       *         directly visible next pipe to connect to
       */
      PID
      resolve (PPipe origin)
        {
          return spec_->resolve (origin);
        }
      
      //TODO: API to retrieve target stream type
      
      
      class TargetSpec
        {
        public:
          virtual ~TargetSpec();
          virtual PID resolve (PPipe origin)  =0;
        };
      
    private:
      enum {
        SPEC_SIZ = mp::maxSize<
                       mp::Types< PID, lumiera_uid, uint>::List>::value 
      };
      typedef lib::OpaqueHolder<TargetSpec, SPEC_SIZ> SpecBuff;
      
      
      /** Storage to hold the Target Spec inline */
      SpecBuff spec_;
      
    };
  
  
  
} // namespace mobject
#endif
