/*
  Meta(Asset)  -  key abstraction: metadata and organisational asset

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


#include "proc/assetmanager.hpp"
#include "proc/asset/meta.hpp"
#include "lib/util.hpp"
#include "include/logging.h"


namespace asset {
  
  using meta::Descriptor;
  
  namespace // Implementation details
  {
    /** helper: .....*/
  }
  
  namespace meta {
    
    Descriptor::~Descriptor() { } // emit vtable here...
    
  }
  
  
  /**storage for the static MetaFactory instance */
  MetaFactory Meta::create;
  
  
  
  /** Generic factory method for Metadata Asset instances.
   *  @param  EntryID specifying the type and a human readable name-ID
   *  @return an Meta smart ptr linked to the internally registered smart ptr
   *          created as a side effect of calling the concrete Meta subclass ctor.
   */
  template<class MA>
  P<MA>
  MetaFactory::operator() (EntryID<MA> elementIdentity)
  {
    UNIMPLEMENTED ("Meta-Factory");
  }
  
  
  /** Generic factory method for specialising Metadata.
   *  @param  prototype Descriptor of a special kind of metadata,
   *          to be augmented and further specialised. Can indeed
   *          be an existing asset::Meta instance
   *  @param  EntryID specifying the type and a human readable name-ID
   *  @return an Meta smart ptr linked to the internally registered smart ptr
   *          created as a side effect of calling the concrete Meta subclass ctor.
   */
  template<class MA>
  P<MA>
  MetaFactory::operator() (Descriptor const& prototype, EntryID<MA> elementIdentity)
  {
    UNIMPLEMENTED ("Meta-Factory");
  }
  
  
  
} // namespace asset




   /**************************************************/
   /* explicit instantiations of the factory methods */
   /**************************************************/

#include "proc/asset/meta/time-grid.hpp"
//#include "proc/asset/procpatt.hpp"
//#include "proc/asset/timeline.hpp"
//#include "proc/asset/sequence.hpp"


namespace asset {
  
  using meta::Descriptor;
  using meta::TimeGrid;
  
  template P<TimeGrid>  MetaFactory::operator() (EntryID<TimeGrid>);
  
  template P<TimeGrid>  MetaFactory::operator() (Descriptor const&, EntryID<TimeGrid>);
  
} // namespace asset
