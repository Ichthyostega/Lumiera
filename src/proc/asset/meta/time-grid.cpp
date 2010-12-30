/*
  TimeGrid  -  reference scale for quantised time

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

* *****************************************************/


#include "proc/asset/meta/time-grid.hpp"
#include "proc/asset/entry-id.hpp"
//#include "lib/util.hpp"
//#include "include/logging.h"


namespace asset {
namespace meta {
  
  namespace {
    
    typedef TimeGrid::Builder Builder;
    
    Asset::Ident
    deriveAssetIdentity (Builder const& setup)
    {
      EntryID<TimeGrid> wtf("wtf**ck");       //////////////////////TODO some real meat here
      return wtf.getIdent();
    }
  }
  
 
  /** */
  TimeGrid::TimeGrid (Builder const& setup)
    : Meta (deriveAssetIdentity (setup))
    { }
  
}} // namespace asset::meta
