/*
  CompoundMedia(Test)  -  handling multichannel Media Assets

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

/** @file compoundmediatest.cpp
 ** unit test \ref CompoundMedia_test
 */


#include "include/logging.h"
#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "proc/assetmanager.hpp"
#include "proc/asset/media.hpp"
#include "proc/asset/proc.hpp"

#include "proc/asset/asset-diagnostics.hpp"

using util::isnil;
using std::string;


namespace proc {
namespace asset{
namespace test {
  
  
  
  
  /*******************************************************************//**
   * Verifying the special operations available for compound media assets
   * comprised of several elementary media assets.
   */
  class CompoundMedia_test : public Test
    {
      virtual void run(Arg arg) 
        {
          buildCompound();
          modifyCompound();
          verifyClipStructure();
          
          if (!isnil (arg))
            dumpAssetManager();
          TRACE (asset_mem, "leaving CreateAsset_test::run()");
        }
      
      
      
      typedef shared_ptr<asset::Media> PM;
      
      /** @test building a compound media asset by using a special
       *        factory, normally intended for loading existing sessions.
       */
      void buildCompound()
        {
          UNIMPLEMENTED ("create new compound media");
        }
      
      /** @test adding and removing elementary media.
       */
      void modifyCompound()
        { 
          UNIMPLEMENTED ("add and remove elementary media to compound media asset");
        }
      
      
      /** @test create a (compound) clip from some compound media asset
       *        and verify the clip mirrors the media asset's structure
       */
      void verifyClipStructure()
        { 
          UNIMPLEMENTED ("create compound clip from compound media and verify structure");
        }
      
      
      bool checkProperties (PM object, Asset::Ident identity, string filename)
        {
          return identity == object->ident   
              && filename == object->getFilename();
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CompoundMedia_test, "unit asset");
  
  
  
}}} // namespace proc::asset::test
