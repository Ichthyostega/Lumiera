/*
  INVENTORY.hpp  -  viewing an Typed-ID registration group as meta asset

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


/** @file inventory.hpp
 ** Exposing an ID registration cluster as a Meta Asset
 ** @todo implementation draft from 2010, stalled as of 2016,
 **       but likely to be of relevance eventually...
 */


#ifndef ASSET_INVENTORY_H
#define ASSET_INVENTORY_H

#include "proc/asset/meta.hpp"



namespace proc {
namespace asset {


  /**
   * TypedID registration group exposed as meta asset
   * 
   * @todo WIP-WIP unimplemented
   */
  class Inventory : public Meta
    {};
  
  
  typedef lib::P<Inventory> PInv;
  
  
}} // namespace proc::asset
#endif
