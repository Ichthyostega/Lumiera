/*
  Clip(Asset)  -  bookkeeping (asset) view of a media clip.
 
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


#include "proc/asset/clip.hpp"

namespace asset
  {
  
  namespace
    {
      /** @internal derive a sensible asset ident tuple
       *  when creating a asset::Clip based on some asset::Media
       *  @todo getting this one is important for handling creation
       *        of multiple clip instances from one media. Means we
       *        have still to figure out a sensible concept...
       */
      const Asset::Ident
      createClipIdent (const Media& mediaref)
        {
          string name (mediaref.ident.name + "-clip");  // TODO something sensible here; append number, sanitize etc.
          Category category (mediaref.ident.category);
          category.setPath(CLIP_SUBFOLDER);
          return Asset::Ident (name, category, 
                               mediaref.ident.org, 
                               mediaref.ident.version );
          
        }
      
      Media::PClipMO
      createClipMO (const ID<Media>& mediaID)
        {
          return mobject::MObject::create(
                     AssetManager::instance()
                               .getAsset (mediaID));
        }
    }
  
  
  
  Clip::Clip (const Media& mediaref)
    : Media (createClipIdent (mediaref),
             mediaref.getFilename())
    , source_ (mediaref) 
    , clipMO_ (createClipMO (this->getID()))
  {
    
  }
  
  
  /** Directly object creating specialisation of the 
   *  asset::Media interface method: immediately starting
   *  out from this asset::Clip and doing the actual 
   *  MObject-creation in case there isn't already 
   *  a clip-MO linked with this Clip Asset. 
   */
  Media::PClipMO 
  Clip::createClip ()
  {
    if (!clipMO_)
      clipMO_ = mobject::MObject::create(
                  AssetManager::instance()
                     .getAsset (this->getID()));
    
    return clipMO_;
  }
  
  
  /** return this wrapped into a shared ptr,
   *   because it's already the desired asset::Clip
   */
  Media::PClip
  Clip::getClipAsset ()
  {
    return PClip (AssetManager::instance()
                    .getAsset (this->getID()));
  }
  
  
  /** specialisation delegating the decision to
   *  the media asset referred by this clip
   */ 
  Media::PMedia
  Clip::checkCompound ()
  {
    return source_.checkCompound();
  }




} // namespace asset
