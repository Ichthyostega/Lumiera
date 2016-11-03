/*
  MediaAccessFacade  -  functions for querying media file and channels.

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


/** @file media-access-facade.cpp
 ** TODO media-access-facade.cpp
 */


#include "backend/media-access-facade.hpp"

#include "lib/util.hpp"

using util::isnil;
using lumiera::error::Invalid;

namespace backend {
  
  /** storage for the SingletonFactory 
   *  (actually a lumiera::test::MockInjector) */
  lib::Depend<MediaAccessFacade> MediaAccessFacade::instance;

  
  
  MediaDesc& 
  MediaAccessFacade::queryFile (string const& name)  const
  {
    if (isnil (name))
      throw Invalid ("empty filename passed to MediaAccessFacade.");
    
    UNIMPLEMENTED ("delegate to backend: query accessability of file");
  }
  
  
  ChanDesc 
  MediaAccessFacade::queryChannel (MediaDesc& mHandle, uint chanNo)  const
  {
    UNIMPLEMENTED ("delegate to backend: query channel information");
    ChanDesc nix;
    return nix;
  }



} // namespace backend
