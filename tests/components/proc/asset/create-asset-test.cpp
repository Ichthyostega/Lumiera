/*
  CreateAsset(Test)  -  constructing and registering Assets

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/


#include "include/logging.h"
#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "proc/assetmanager.hpp"
#include "proc/asset/media.hpp"
#include "proc/asset/proc.hpp"

#include "proc/asset/asset-diagnostics.hpp"

using util::isnil;
using std::string;


namespace asset {
namespace test {




  /***********************************************************************
   * @test creating new Assets and registering them with the AssetManager.
   * @see  proc_interface::AssetManager#reg
   */
  class CreateAsset_test : public Test
    {
      virtual void run(Arg arg)
        {
          createMedia();
          factoryVariants();

          if (!isnil (arg))
            dumpAssetManager();
          TRACE (asset_mem, "leaving CreateAsset_test::run()");
        }


                                                          ////////////////////////////////////TICKET #589
      typedef P<Media> PM;                                                         /////TODO: transition to P<>

      /** @test Creating and automatically registering Asset instances.
       *        Re-Retrieving the newly created objects from AssetManager.
       *        Checking AssetManager access functions, esp. getting
       *        different kinds of Assets by ID, querying with the
       *        wrong Category and querying unknown IDs.
       */
      void createMedia()
        {
          Category cat(VIDEO,"bin1");
          Asset::Ident key("Name-1", cat, "ichthyo", 5);
          PM mm1 = asset::Media::create(key,"testfile.mov");
          PM mm2 = asset::Media::create("testfile1.mov", cat);
          PM mm3 = asset::Media::create("testfile2.mov", VIDEO);

          // Assets have been registered and can be retrieved by ID
          AssetManager& aMang = AssetManager::instance();
          PM registered;                                                         /////TODO: transition to P<>
          registered = aMang.getAsset (mm1->getID());
          CHECK (registered == mm1);
          registered = aMang.getAsset (mm2->getID());
          CHECK (registered == mm2);
          registered = aMang.getAsset (mm3->getID());
          CHECK (registered == mm3);

          registered = aMang.getAsset (mm1->getID());
          CHECK (registered != mm2);
/*
* TODO: switch back to original version
*       once the transition to P<XX> is done...
*
          CHECK (aMang.getAsset (mm1->getID()) == mm1);
          CHECK (aMang.getAsset (mm2->getID()) == mm2);
          CHECK (aMang.getAsset (mm3->getID()) == mm3);

          CHECK (aMang.getAsset (mm1->getID()) != mm2);
*/
          PAsset aa1 = aMang.getAsset (ID<Asset>(mm1->getID()));   // note we get an Asset ref
          CHECK (aa1 == mm1);
          PM mX1 = aMang.getAsset (mm1->getID());                // ..and now we get a Media ref
          CHECK (mX1 == mm1);
          CHECK (mX1 == aa1);

          CHECK (aMang.known (mm1->getID()));
          CHECK (aMang.known (mm2->getID()));
          CHECK (aMang.known (mm3->getID()));

          CHECK ( !aMang.known (mm3->getID(), Category(AUDIO))); // not found within AUDIO-Category
          try
            { // can't be found if specifying wrong Asset kind....
              aMang.getAsset (ID<asset::Proc>(mm1->getID()));
              NOTREACHED();
            }
          catch (lumiera::error::Invalid& xxx) {CHECK (xxx.getID()==LUMIERA_ERROR_WRONG_ASSET_KIND);}
          try
            { // try accessing nonexistent ID
              aMang.getAsset (ID<Asset> (1234567890));
              NOTREACHED();
            }
          catch (lumiera::error::Invalid& xxx) {CHECK (xxx.getID()==LUMIERA_ERROR_UNKNOWN_ASSET_ID);}
          lumiera_error (); // reset errorflag


          // checking the Ident-Fields
          CHECK (mm1->ident.name == "Name-1");
          CHECK (mm2->ident.name == "testfile1");
          CHECK (mm3->ident.name == "testfile2");

          CHECK (cat == Category (VIDEO,"bin1"));
          CHECK (mm1->ident.category == Category (VIDEO,"bin1"));
          CHECK (mm2->ident.category == Category (VIDEO,"bin1"));
          CHECK (mm3->ident.category == Category (VIDEO       ));

          CHECK (mm1->ident.org == "ichthyo");
          CHECK (mm2->ident.org == "lumi");
          CHECK (mm3->ident.org == "lumi");

          CHECK (mm1->ident.version == 5);
          CHECK (mm2->ident.version == 1);
          CHECK (mm3->ident.version == 1);

          CHECK (mm1->getFilename() == "testfile.mov");
          CHECK (mm2->getFilename() == "testfile1.mov");
          CHECK (mm3->getFilename() == "testfile2.mov");


          TRACE (asset_mem, "leaving test method scope");
        }


      /** @test different variants of calling the MediaFactory,
       *        with focus on the behaviour of the basic Asset
       *        creation machinery. Covers filling out Asset's
       *        datafields, amending missing pieces of information.
       */
      void factoryVariants()
        {
          PM candi;

          Asset::Ident key1("Au-1", Category(AUDIO), "ichthyo", 5);
          candi = asset::Media::create(key1);
          CHECK ( checkProperties (candi, key1, ""));

          candi = asset::Media::create(key1, string("testfile.wav"));
          CHECK ( checkProperties (candi, key1, "testfile.wav"));

          Asset::Ident key2("", Category(AUDIO), "ichthyo", 5);
          candi = asset::Media::create(key2, string("testfile2.wav"));
          CHECK ( checkProperties (candi, key2, "testfile2.wav"));
          CHECK (key2.name == "testfile2"); // name filled in automatically

          candi = asset::Media::create(string("testfile3.wav"), Category(AUDIO));
          CHECK ( checkProperties (candi, Asset::Ident("testfile3", Category(AUDIO), "lumi", 1)
                                         , "testfile3.wav"));

          candi = asset::Media::create("some/path/testfile4.wav", Category(AUDIO));
          CHECK ( checkProperties (candi, Asset::Ident("testfile4", Category(AUDIO), "lumi", 1)
                                         , "some/path/testfile4.wav"));

          candi = asset::Media::create("", Category(AUDIO,"sub/bin"));
          CHECK ( checkProperties (candi, Asset::Ident("nil", Category(AUDIO,"sub/bin"), "lumi", 1)
                                         , ""));

          candi = asset::Media::create("", AUDIO);
          CHECK ( checkProperties (candi, Asset::Ident("nil", Category(AUDIO), "lumi", 1)
                                         , ""));
        }

      bool checkProperties (PM object, Asset::Ident identity, string filename)
        {
          return identity == object->ident
              && filename == object->getFilename();
        }
    };


  /** Register this test class... */
  LAUNCHER (CreateAsset_test, "unit asset");



}} // namespace asset::test
