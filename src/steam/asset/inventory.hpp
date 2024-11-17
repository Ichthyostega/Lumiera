/*
  INVENTORY.hpp  -  viewing an Typed-ID registration group as meta asset

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file inventory.hpp
 ** Exposing an ID registration cluster as a Meta Asset
 ** @todo implementation draft from 2010, stalled as of 2016,
 **       but likely to be of relevance eventually...
 */


#ifndef ASSET_INVENTORY_H
#define ASSET_INVENTORY_H

#include "steam/asset/meta.hpp"



namespace steam {
namespace asset {


  /**
   * TypedID registration group exposed as meta asset
   * 
   * @todo WIP-WIP unimplemented
   */
  class Inventory : public Meta
    {};
  
  
  typedef lib::P<Inventory> PInv;
  
  
}} // namespace steam::asset
#endif
