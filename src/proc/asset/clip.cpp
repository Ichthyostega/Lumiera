/*
  Clip(Asset)  -  bookkeeping (asset) view of a media clip.

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


/** @file clip.cpp
 ** Clip Asset implementation
 */


#include "proc/asset/clip.hpp"
#include "proc/mobject/session/mobjectfactory.hpp"   ////TODO: avoidable?

using std::static_pointer_cast;


namespace proc {
namespace asset {
  
  namespace {
    
    /** @internal derive a sensible asset ident tuple
     *  when creating a asset::Clip based on some asset::Media
     *  @todo getting this one correct is important for handling creation
     *        of multiple clip instances from one media. Means we
     *        have still to figure out a sensible concept...
     */
    const Asset::Ident
    createClipIdent (const Media& mediaref)
    {
      string name (mediaref.ident.name + "-clip");  // TODO something sensible here; append number, sanitise etc.
      Category category (mediaref.ident.category);
      category.setPath(CLIP_SUBFOLDER);
      return Asset::Ident (name, category, 
                           mediaref.ident.org, 
                           mediaref.ident.version );
    }
    
    Media::PClip
    createClipMO (const Clip& thisClipAsset, const Media& mediaChannel)
    {
      return mobject::MObject::create (thisClipAsset,mediaChannel);
    }
  }
  
  
  
  Clip::Clip (Media& mediaref)
    : Media (createClipIdent (mediaref),
             mediaref.getFilename(),
             mediaref.getLength())
    , source_ (mediaref) 
    , clipMO_ (createClipMO (*this, source_))
    {
      this->defineDependency (mediaref);
    }
  
  
  /** Specialisation of the asset::Media interface method,
   *  just returning the already existing Clip-MO. Every
   *  asset::Clip internally holds a Clip-MO, which has 
   *  been created alongside. This Clip-MO may have several
   *  Placements or no placement at all (meaning it need not
   *  be placed within the session) 
   */
  Media::PClip
  Clip::createClip ()  const
  {
    return clipMO_;
  }
  


  /** return this wrapped into a shared ptr,
   *   because it's already the desired asset::Clip
   */
  Media::PClipAsset
  Clip::getClipAsset ()
  {
    return AssetManager::wrap (*this);
  }
  

  
  /** specialisation delegating the decision to
   *  the media asset referred by this clip
   */ 
  Media::PMedia
  Clip::checkCompound()  const
  {
    return source_.checkCompound();       ////////////////////////TODO better interface!!!
  }



}} // namespace proc::asset
