/*
  CODEC.hpp  -  description of some media data decoder or encoder facility

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file codec.hpp
 ** Definition of a Codec Asset
 ** @deprecated as of 2016 it seems questionable we'll have to deal with such assets,
 **   since coding/decoding will always be delegated to widely available libraries.
 */


#ifndef ASSET_CODEC_H
#define ASSET_CODEC_H

#include "steam/asset/proc.hpp"



namespace steam {
namespace asset {
  
  /**
   * description of some media data decoder or encoder facility
   */
  class Codec : public Proc
    {
      
    };
  
  
  
}} // namespace steam::asset
#endif
