/*
  WRAPPERPTR.hpp  -  variant record able to hold a pointer to some smart-ptr/wrapper types, providing conversions

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file wrapperptr.hpp
 ** Wrapper to treat several flavours of smart-pointers uniformly
 ** @deprecated as of 2016 we should re-think how to organise visitor use in the Builder
 */



#ifndef LUMIERA_WRAPPERPTR_H
#define LUMIERA_WRAPPERPTR_H


#include "lib/variant-o.hpp"
#include "lib/access-casted-o.hpp"

#include "lib/meta/typelist.hpp"
#include "steam/mobject/placement.hpp"
#include "lib/p.hpp"


namespace steam {
  namespace asset   { class Asset; }
  namespace mobject { class MObject; }
  
  
  typedef lib::meta::Types < mobject::Placement<mobject::MObject>*
                           , lib::P<asset::Asset>*
                           > ::List
                           WrapperTypes;
}

namespace lumiera {
  
  
  
  /** 
   * helper to treat various sorts of smart-ptrs uniformly.
   * Implemented as a variant-type value object, it is preconfigured
   * with the possible hierarchy-base classes used within this application.
   * Thus, when passing in an arbitrary smart-ptr, the best fitting smart-ptr
   * type pointing to the corresponding base class is selected for internal storage.
   * Later on, stored values can be retrieved either utilising static or dynamic casts;
   * error reporting is similar to the behaviour of dynamic_cast<T>: when retrieving
   * a pointer, NULL is returned in case of mismatch.
   * 
   * @deprecated not sure if this was a good idea anyway. Better re-think how to handle wrapped objects in visitation
   * @todo really need to switch to the new lib::Variant and util::AccessCasted implementation  ////////////////TICKET #450
   */
  typedef lib::VariantO<steam::WrapperTypes, util::AccessCasted_O> WrapperPtr;
  
  
  
} // namespace lumiera 
#endif
