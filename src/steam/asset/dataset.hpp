/*
  DATASET.hpp  -  meta asset describing a collection of control data

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

*/


/** @file dataset.hpp
 ** Definition of an Asset to represent a set of control data
 ** An example would be captured animation data from some kind of external controller.
 ** @warning as of 2016, this is a design placeholder. The concept seems reasonable though.
 */


#ifndef ASSET_DATASET_H
#define ASSET_DATASET_H

#include "steam/asset/meta.hpp"



namespace proc {
namespace asset {
  
  
  /**
   * meta asset describing a collection of control data
   */
  class Dataset : public Meta
    {};
  
}} // namespace proc::asset
#endif
