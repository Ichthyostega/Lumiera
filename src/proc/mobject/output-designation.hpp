/*
  OUTPUT-DESIGNATION.hpp  -  specifying a desired output destination
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
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
  
  class MObject;
  
  template<class MX>
  class PlacementRef;
  
  typedef PlacementRef<MObject> RefPlacement;

  
  
  /**
   * Descriptor to denote the desired target of produced media data.
   * OutputDesignation is always an internal and relative specification
   * and boils down to referring an asset::Pipe by ID. In order to get
   * actually effective, some object within the model additionally
   * needs to \em claim this pipe-ID, meaning that this object
   * states to root and represent this pipe. When the builder
   * encounters a pair of (OutputDesignation, OutputClaim),
   * an actual stream connection will be wired in the
   * processing node network.
   */
  class OutputDesignation
    {
    public:
      typedef asset::ID<asset::Pipe> PID;
      typedef asset::PPipe PPipe;
      
      PID resolve (PPipe origin);
      
      //TODO: API to retrieve target stream type
      
      
      explicit OutputDesignation (PID explicitTarget);
      explicit OutputDesignation (RefPlacement const& indirectTarget); 
      explicit OutputDesignation (uint relative_busNr =0);
      
      // using default copying
      
      
      class TargetSpec
        {
        public:
          virtual ~TargetSpec();
          PID resolve (PPipe origin);
        };
      
    private:
      enum {
        SPEC_SIZ = lumiera::typelist::maxSize<
                       lumiera::typelist::Types<PID,lumiera_uid,uint>::List>::value 
      };
      typedef lib::OpaqueHolder<TargetSpec, SPEC_SIZ> SpecBuff;
      
      
      /** Storage to hold the Target Spec inline */
      SpecBuff spec_;
      
    };
  
  
  
} // namespace mobject
#endif
