/*
  Unknown  -  placeholder for unknown or unavailable media source

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file unknown.cpp
 ** Implementation functions regarding a marker asset for unknown media.
 */


#include "steam/asset/unknown.hpp"
#include "lib/format-string.hpp"

using util::_Fmt;

namespace steam {
namespace asset {
  
  LUMIERA_ERROR_DEFINE (ORIG_NOT_FOUND, "Media referred by placeholder not found");
  
  
  
  /** create a placeholder for a media with the given identity.
   *  the denoted original media (identity) can be accessed later
   *  on using the Unknown::getOrg() function.
   *  @note we don't depend on the referred media...
   */
  Unknown::Unknown (const Asset::Ident& idi, string name, Duration length)
    : Media (idi, name, length)
  {
    TODO ("implement some sensible behaviour for the »unknown media« placeholder...");
  }
  
  
  /** using the information stored in this placeholder asset,
   *  try to access the "real" media it stands for.
   */
  Media::PMedia
  Unknown::getOrg()
  {
    UNIMPLEMENTED ("how to get at the original media from a »Unknown« placeholder");
    if (1==0)
      throw lumiera::error::Invalid (_Fmt("Unable to locate original media "
                                          "for ID=%s, filename=\"%s\".")
                                         % this->ident
                                         % this->getFilename()
                                    ,LUMIERA_ERROR_ORIG_NOT_FOUND);
  }
  
  
  
}} // namespace asset
