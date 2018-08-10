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


/** @file asset/meta.cpp
 ** Implementation details regarding the Meta Asset category
 */


#include "proc/assetmanager.hpp"
#include "proc/asset/meta.hpp"
#include "lib/util.hpp"

using lib::idi::EntryID;

namespace proc {
namespace asset {
  
  using meta::Descriptor;
  
  namespace // Implementation details
  {
    /** helper: .....*/
  }
  
  namespace meta {
    
    Descriptor::~Descriptor() { } // emit VTable here...
    
  }
  
  
  /**storage for the static MetaFactory instance */
  MetaFactory Meta::create;
  
  
  
  /** Generic factory method for Metadata Asset instances.
   *  @param  EntryID specifying the type and a human readable name-ID
   *  @return an meta::Builder struct with the metadata parameters. After configuring
   *          and tweaking those parameters, the builder's \c commit() function
   *          will create a new (immutable) meta asset.
   */
  template<class MA>
  meta::Builder<MA>
  MetaFactory::operator() (EntryID<MA> elementIdentity)
  {
    return meta::Builder<MA> (elementIdentity.getSym());
  }
  
  
  /** Generic factory method for specialising Metadata.
   *  @param  prototype Descriptor of a special kind of metadata,
   *          to be augmented and further specialised. Can indeed
   *          be an existing asset::Meta instance
   *  @param  EntryID specifying the type and a human readable name-ID
   *  @return an meta::Builder struct with the metadata parameters. After configuring
   *          and tweaking those parameters, the builder's \c commit() function
   *          will create a new (immutable) meta asset.
   */
  template<class MA>
  meta::Builder<MA>
  MetaFactory::operator() (Descriptor const& prototype, EntryID<MA> elementIdentity)
  {
    UNIMPLEMENTED ("Meta-Factory: extend or supersede existing meta asset");       ////////////////////TICKET #746
  }
  
  
  
}} // namespace proc::asset




   /**************************************************/
   /* explicit instantiations of the factory methods */
   /**************************************************/

#include "proc/asset/meta/time-grid.hpp"


namespace proc {
namespace asset {
  
  using meta::Descriptor;
  using meta::Builder;
  using meta::TimeGrid;
  
  template Builder<TimeGrid>  MetaFactory::operator() (EntryID<TimeGrid>);
  
  template Builder<TimeGrid>  MetaFactory::operator() (Descriptor const&, EntryID<TimeGrid>);
  
}} // namespace proc::asset
