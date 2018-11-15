/*
  COMPOUNDMEDIA.hpp  -  multichannel media

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


/** @file compoundmedia.hpp
 ** Definition of an Asset to represent a clustering or compound of several media
 ** @warning early draft, never put into use as of 2016 (but there wasn't a reason to
 **          do so either, since we didn't get up to dealing with this topic)
 */


#ifndef ASSET_COMPOUNDMEDIA_H
#define ASSET_COMPOUNDMEDIA_H

#include "proc/asset/media.hpp"

#include <vector>

using std::vector;



namespace proc {
namespace asset {


  /**
   * compound of several elementary media tracks,
   * e.g. the individual media streams found in one media file
   */
  class CompoundMedia : public Media
    {
    protected:
      /** elementary media assets comprising this compound */
      vector<Media *> tracks;  // TODO: shouldn't we use shared_ptr here???

    };
    
    
    
}} // namespace proc::asset
#endif
