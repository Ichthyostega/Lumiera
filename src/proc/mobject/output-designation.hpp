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


/** @file output-designation.hpp
 ** Core abstraction: symbolic representation of some output target.
 ** An OutputDesignation represents something you might want to connect to
 ** or towards which you'd like to direct output. It might be _absolute,_ in
 ** which case it represents an actual output device attached to the system,
 ** or it might be relative, as a placeholder for further wiring and output routing.
 ** An example for the latter would be the "master bus".
 ** 
 ** The concept of an OutputDesignation plays a crucial role to allow flexible connection
 ** and rearrangement of media content. It allows to build partial structures, which can
 ** be connected automatically to a suitable output when placed into the appropriate context.
 ** In the end, the Builder will resolve all output designations, finally leading to a fixed
 ** set of exit nodes, which can be pulled to deliver content.
 */


#ifndef PROC_MOBJECT_OUTPUT_DESIGNATION_H
#define PROC_MOBJECT_OUTPUT_DESIGNATION_H

#include "proc/asset/pipe.hpp"
#include "lib/hash-value.h"
#include "lib/opaque-holder.hpp"
#include "lib/meta/typelist-manip.hpp"


namespace proc {
namespace mobject {
  
  namespace mp = lib::meta;
  
  class MObject;
  
  template<class MX>
  class PlacementRef;
  
  typedef PlacementRef<MObject> RefPlacement;
  
  
  
  /**
   * Descriptor to denote the desired target of produced media data.
   * OutputDesignation is always an internal and relative specification
   * and boils down to referring an asset::Pipe by ID. In order to become
   * actually effective, some object within the model additionally
   * needs to _claim_ this pipe-ID, meaning that this object
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
      enum
        { VTABLE   = sizeof(size_t)
        , SPEC_SIZ = VTABLE
                   + mp::maxSize<
                       mp::Types< PID, lumiera_uid, uint>::List>::value 
        };
      typedef lib::OpaqueHolder<TargetSpec, SPEC_SIZ> SpecBuff;
      
      
      /** Storage to hold the Target Spec inline */
      SpecBuff spec_;
      
    };
  
  
  
}} // namespace proc::mobject
#endif /*PROC_MOBJECT_OUTPUT_DESIGNATION_H*/
