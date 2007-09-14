/*
  Asset}  -  Superinterface: bookeeping view of "things" present in the session
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


#include "proc/asset.hpp"
#include "proc/asset/category.hpp"

namespace proc_interface
  {



  /**
   * List of entities this asset depends on or requires to be functional. May be empty. The head of this list can be considered the primary prerequisite
   */
  vector<PAsset>
  Asset::getParents ()
  {
  }


  /**
   * All the other assets requiring this asset to be functional. For example, all the clips depending on a given media file. May be empty. The dependency relation is transitive.
   */
  vector<PAsset>
  Asset::getDependant ()
  {
  }


  /**
   * weather this asset is swithced on and consequently included in the fixture and participates in rendering
   */
  bool
  Asset::isActive ()
  {
  }


  /**
   * change the enabled status of this asset. Note the corresponding #isActive predicate may depend on the enablement status of parent assets as well
   */
  void
  Asset::enable ()  throw(cinelerra::error::State)
  {
  }



} // namespace proc_interface
