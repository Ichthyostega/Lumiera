/*
  PREVIEW.hpp  -  alternative version of the media data, probably with lower resolution

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file preview.hpp
 ** Placeholder "proxy" Asset
 */


#ifndef ASSET_PREVIEW_H
#define ASSET_PREVIEW_H

#include "steam/asset/unknown.hpp"



namespace steam {
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
  
  
  
}} // namespace steam::asset
#endif
