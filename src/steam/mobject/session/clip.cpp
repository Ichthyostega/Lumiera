/*
  Clip  -  a Media Clip

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file session/clip.cpp
 ** Implementation details regarding a media clip as integrated into the edit / session model.
 ** @todo stalled effort towards a session implementation from 2008
 ** @todo 2016 likely to stay, but expect some extensive rework
 */


#include "steam/mobject/session/clip.hpp"
#include "steam/assetmanager.hpp"
#include "steam/asset/media.hpp"
#include "steam/asset/clip.hpp"
#include "lib/time/mutation.hpp"
#include "lib/util.hpp"

using lib::time::Mutation;
using util::isnil;

namespace steam {
namespace mobject {
namespace session {

  /** new clip-MO linked with the given asset::Clip.
   *  Initially, this clip will cover the whole source media length.
   */
  Clip::Clip (asset::Clip const& clipDef, Media const& mediaDef)
    : mediaDef_(mediaDef)
    , clipDef_(clipDef)
    {
      setupLength();
      throwIfInvalid();
    }
  
  
  
  /** implementing the common MObject self test.
   *  Length definition is consistent, underlying
   *  media def is accessible etc. */
  bool
  Clip::isValid ()  const
  {
    TODO ("check consistency of clip length def, implies accessing the underlying media def");
    return not isnil(length_);
  }
  
  
  void 
  Clip::setupLength()
  {
    TODO ("really calculate the length of a clip and set length field");
    this->length_.accept (Mutation::changeDuration(mediaDef_.getLength()));
  }
  
  
  PMedia 
  Clip::getMedia ()  const                        
  { 
    return asset::AssetManager::wrap (mediaDef_);
  }
  
  
  PClipAsset
  Clip::findClipAsset ()  const
  {
    return asset::AssetManager::wrap (clipDef_);
  }
  
  
  
  
}}} // namespace steam::mobject::session

