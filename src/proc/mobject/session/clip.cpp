/*
  Clip  -  a Media Clip
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "proc/mobject/session/clip.hpp"
#include "proc/assetmanager.hpp"
#include "proc/asset/media.hpp"
#include "proc/asset/clip.hpp"

namespace mobject
  {
  namespace session
    {

    /** new clip-MO linked with the given asset::Clip.
     *  Initially, this clip will cover the whole source media length. 
     */
    Clip::Clip (const asset::Clip& clipDef, const Media& mediaDef)
      : start_(0),
        mediaDef_(mediaDef),
        clipDef_(clipDef)
    {
      setupLength();
    }
    
    
    
    /** implementing the common MObject self test.
     *  Length definition is consitent, underlying 
     *  media def is accessible etc. */
    bool
    Clip::isValid ()  const
    {
      TODO ("check consistency of clip length def, implies accessing the underlying media def");
      return length > 0;
    }
    
    
    void 
    Clip::setupLength()
    {
      TODO ("really calculate the length of a clip and set length field");
      this->length = mediaDef_.getLength();
    }
    
    
    PMedia 
    Clip::getMedia ()  const                        
    { 
      return asset::AssetManager::getPtr (mediaDef_);
    }
    
    
    PClipAsset
    Clip::findClipAsset ()  const
    {
      return asset::AssetManager::getPtr (clipDef_);
    }







  } // namespace mobject::session

} // namespace mobject
