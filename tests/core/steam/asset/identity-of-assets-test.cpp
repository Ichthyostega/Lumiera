/*
  IdentityOfAssets(Test)  -  Asset object identity and versioning

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

/** @file identity-of-assets-test.cpp
 ** unit test \ref IdentityOfAssets_test
 */


#include "include/logging.h"
#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "steam/assetmanager.hpp"
#include "steam/asset/media.hpp"
#include "steam/asset/proc.hpp"

#include "steam/asset/asset-diagnostics.hpp"
#include "vault/media-access-mock.hpp"
#include "lib/depend-inject.hpp"

using util::isnil;
using std::string;


namespace steam {
namespace asset{
namespace test {
  
  using MediaAccessMock = lib::DependInject<backend::MediaAccessFacade>
                                ::Local<backend::test::MediaAccessMock>;
  
  
  
  
  /*******************************************************************//**
   * @test creating several Assets and checking object identity,
   *       detection of duplicates and version handling.
   * @see  proc_interface::AssetManager#reg
   */
  class IdentityOfAssets_test : public Test
    {
      virtual void run(Arg arg)
        {
          createDuplicate();
          
          if (!isnil (arg))
            dumpAssetManager();
          TRACE (asset_mem, "leaving IdentityOfAssets_test::run()");
        } 
      
      
      
      typedef shared_ptr<asset::Media> PM;
      
      /** @test produce an ID clash.
       *        documents the current behaviour of the code as of 9/07
       *  @todo this test is expected to break when the detection
       *        of duplicate registrations is implemented.
       */
      void createDuplicate()
        { 
          MediaAccessMock useMockMedia;
          
          PM mm1 = asset::Media::create ("test-1.mov", VIDEO);
          
          Asset::Ident idi (mm1->ident);         // duplicate Ident record
          PM mm1X = asset::Media::create (idi); //  note: we actually don't call any ctor
          CHECK (mm1 == mm1X);                 //         instead, we got mm1 back.
          
          PM mm2 = asset::Media::create (idi,"test-2.mov");
          
          CHECK (mm1->getID() == mm2->getID()); // different object, same hash
          
          AssetManager& aMang = AssetManager::instance();
          CHECK (aMang.getAsset (mm1->getID()) == mm2); // record of mm1 was replaced by mm2
          CHECK (aMang.getAsset (mm2->getID()) == mm2);
          
          CHECK (aMang.known (mm1->getID()));
          CHECK (aMang.known (mm2->getID()));
          CHECK (mm1->ident.name == "test-1");
          CHECK (mm2->ident.name == "test-1");
          CHECK (mm1->getFilename() == "test-1.mov");
          CHECK (mm2->getFilename() == "test-2.mov");
          
          
          TRACE (asset_mem, "leaving test method scope");
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (IdentityOfAssets_test, "unit asset");
  
  
  
}}} // namespace steam::asset::test
