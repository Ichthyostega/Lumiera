/*
  CompoundMedia(Test)  -  handling multichannel Media Assets

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file compoundmediatest.cpp
 ** unit test \ref CompoundMedia_test
 */


#include "include/logging.h"
#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "steam/assetmanager.hpp"
#include "steam/asset/media.hpp"
#include "steam/asset/proc.hpp"

#include "steam/asset/asset-diagnostics.hpp"

using util::isnil;
using std::string;


namespace steam {
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
  
  
  
}}} // namespace steam::asset::test
