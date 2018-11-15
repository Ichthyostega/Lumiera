/*
  TESTCLIPASSET.hpp  -  test Media-Asset (clip) for checking Assets and MObjects

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
