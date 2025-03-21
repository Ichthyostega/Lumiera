/*
  Meta(Asset)  -  key abstraction: metadata and organisational asset

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file asset/meta.cpp
 ** Implementation details regarding the Meta Asset category
 */


#include "steam/assetmanager.hpp"
#include "steam/asset/meta.hpp"
#include "lib/util.hpp"

using lib::idi::EntryID;

namespace steam {
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
  
  
  
}} // namespace steam::asset




   /**************************************************/
   /* explicit instantiations of the factory methods */
   /**************************************************/

#include "steam/asset/meta/time-grid.hpp"
#include "steam/asset/meta/error-log.hpp"


namespace steam {
namespace asset {
  
  using meta::Descriptor;
  using meta::Builder;
  using meta::TimeGrid;
  
  template Builder<TimeGrid>  MetaFactory::operator() (EntryID<TimeGrid>);
  template Builder<TimeGrid>  MetaFactory::operator() (Descriptor const&, EntryID<TimeGrid>);
  
  using meta::ErrorLog;
  
  template Builder<ErrorLog>  MetaFactory::operator() (EntryID<ErrorLog>);
  template Builder<ErrorLog>  MetaFactory::operator() (Descriptor const&, EntryID<ErrorLog>);
  
}} // namespace steam::asset
