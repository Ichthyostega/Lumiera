/*
  TESTCLIPASSET.hpp  -  test Media-Asset (clip) for checking Assets and MObjects

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file testclipasset.hpp
 ** A placeholder Clip asset to support unit testing
 */


#ifndef ASSET_TESTCLIPASSET_H
#define ASSET_TESTCLIPASSET_H


#include "lib/test/run.hpp"
//#include "lib/util.hpp"


using std::string;


namespace steam {
namespace asset{
  
  /**
   * An asset::Media instance Test Clip for checking
   * various Asset operations and for creating 
   * dependent Clip-MObjects.
   * 
   */
  class TestClipAsset
    {
    public:
    };
  
  
  
  
}} // namespace steam::asset
#endif
