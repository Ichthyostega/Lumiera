/*
  PREVIEW.hpp  -  alternative version of the media data, probably with lower resolution

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


/** @file preview.hpp
 ** Placeholder "proxy" Asset
 */


#ifndef ASSET_PREVIEW_H
#define ASSET_PREVIEW_H

#include "proc/asset/unknown.hpp"



namespace proc {
namespace asset {
  
  /**
   * special placeholder denoting an alternative version of the media data,
   * typically with lower resolution ("proxy media")
   */
  class Preview : public Unknown
    {
    protected:
      Preview (Media& mediaref);
      friend class MediaFactory;
    };
  
  
  
}} // namespace proc::asset
#endif
