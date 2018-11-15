/*
  UNKNOWN.hpp  -  placeholder for unknown or unavailable media source

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


/** @file unknown.hpp
 ** Marker Asset to indicate an unknown media source.
 ** Elements marked as Unknown are retained in disabled state within the model.
 ** This allows to open and work on a session file, without having all of the
 ** data, setup or environment available
 */


#ifndef ASSET_UNKNOWN_H
#define ASSET_UNKNOWN_H

#include "steam/asset/media.hpp"
#include "lib/time/timevalue.hpp"



namespace steam {
namespace asset {
  
  using lib::time::FSecs;
  using lib::time::Duration;
  
  const Duration DUMMY_TIME (FSecs(5)); ///< @todo solve config management
  
  /**
   * Placeholder Asset for unknown or unavailable media source.
   * @todo maybe do special handling of the media length, allowing
   *       it to support existing clips even if the media length
   *       is not known?
   */
  class Unknown : public Media
    {
    protected:
      Unknown (const Asset::Ident& idi, string name="", Duration length=DUMMY_TIME);
      friend class MediaFactory;
      
    public:
      virtual Media::PMedia getOrg();
      
    };
  
  LUMIERA_ERROR_DECLARE (ORIG_NOT_FOUND);
  
  
  
  
}} // namespace steam::asset
#endif
