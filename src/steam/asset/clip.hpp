/*
  CLIP.hpp  -  bookkeeping (asset) view of a media clip.

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


/** @file asset/clip.hpp
 ** Definition of Asset representation for a media clip
 */


#ifndef ASSET_CLIP_H
#define ASSET_CLIP_H

#include "steam/asset/media.hpp"



/////////////////////////TODO: 1/2010 very likely the handling of the clip-asset needs to be rewritten

namespace proc {
namespace asset {
  
  /**
   * bookkeeping (Asset) view of a media clip.
   */
  class Clip : public Media
    {
      /** media source of this clip  */
      const Media& source_;
      
      /** the corresponding (dependent) clip-MO */
      PClip clipMO_;
      
    public:
      virtual PClip createClip ()  const;
      
    protected:
      Clip (Media& mediaref);
      friend class MediaFactory;
      
      virtual PClipAsset getClipAsset ();
      virtual PMedia checkCompound ()  const;
      
    };
  
  typedef lib::P<const asset::Clip> PClipAsset;
  
  const string CLIP_SUBFOLDER = "clips";      // TODO: handling of hard-wired constants....
  
  
  
}} // namespace proc:asset
#endif
