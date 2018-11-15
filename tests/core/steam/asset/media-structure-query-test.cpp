/*
  MediaStructureQuery(Test)  -  check functionality used for creating media assets

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

* *****************************************************/

/** @file media-structure-query-test.cpp
 ** unit test \ref MediaStructureQuery_test
 */


#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "steam/assetmanager.hpp"
#include "steam/asset/media.hpp"
#include "steam/asset/proc.hpp"

#include "steam/asset/asset-diagnostics.hpp"

using util::isnil;
using std::string;


namespace proc {
namespace asset{
namespace test {
  
  
  
  
  /*******************************************************************//**
   * This test documents the Interface used by MediaFactory when loading
   * media files for querying Lumiera's vault layer for information
   * on how the media file is structured.
   */
  class MediaStructureQuery_test : public Test
    {
      virtual void run(Arg) 
        {
          UNIMPLEMENTED ("querying media file structure from vault");
        }
      
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (MediaStructureQuery_test, "unit asset");
  
  
  
}}} // namespace proc::asset::test
