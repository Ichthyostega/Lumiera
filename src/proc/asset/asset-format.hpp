/*
  ASSET-FORMAT.hpp  -  helpers for display of asset entities

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


#ifndef ASSET_ASSET_FORMAT_H
#define ASSET_ASSET_FORMAT_H

#include "proc/asset.hpp"
#include "proc/asset/category.hpp"

#include <iostream>



namespace asset {
  
  
  inline std::ostream&
  operator<< (std::ostream& os, Category const& cat)
  {
    return os << string(cat); 
  }
  
  inline std::ostream&
  operator<< (std::ostream& os, Asset::Ident const& idi)
  {
    return os << string(idi); 
  }
  
  
  
  
} // namespace asset
#endif
