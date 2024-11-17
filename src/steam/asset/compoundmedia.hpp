/*
  COMPOUNDMEDIA.hpp  -  multichannel media

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file compoundmedia.hpp
 ** Definition of an Asset to represent a clustering or compound of several media
 ** @warning early draft, never put into use as of 2016 (but there wasn't a reason to
 **          do so either, since we didn't get up to dealing with this topic)
 */


#ifndef ASSET_COMPOUNDMEDIA_H
#define ASSET_COMPOUNDMEDIA_H

#include "steam/asset/media.hpp"

#include <vector>

using std::vector;



namespace steam {
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
    
    
    
}} // namespace steam::asset
#endif
