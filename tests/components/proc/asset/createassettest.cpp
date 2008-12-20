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


#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "proc/assetmanager.hpp"
#include "proc/asset/media.hpp"
#include "proc/asset/proc.hpp"

#include "proc/asset/assetdiagnostics.hpp"

using util::isnil;
using std::string;


namespace asset
  {
  namespace test
    {
    
    
    
    
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
            TRACE (assetmem, "leaving CreateAsset_test::run()");
          }
        
        
        
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
            ASSERT (registered == mm1);
            registered = aMang.getAsset (mm2->getID());
            ASSERT (registered == mm2);
            registered = aMang.getAsset (mm3->getID());
            ASSERT (registered == mm3);
            
            registered = aMang.getAsset (mm1->getID());
            ASSERT (registered != mm2);
/* 
 * TODO: switch back to original version
 *       once the transition to P<XX> ist done...
 * 
            ASSERT (aMang.getAsset (mm1->getID()) == mm1);
            ASSERT (aMang.getAsset (mm2->getID()) == mm2);
            ASSERT (aMang.getAsset (mm3->getID()) == mm3);
            
            ASSERT (aMang.getAsset (mm1->getID()) != mm2);
*/            
            PAsset aa1 = aMang.getAsset (ID<Asset>(mm1->getID()));   // note we get an Asset ref
            ASSERT (aa1 == mm1);
            PM mX1 = aMang.getAsset (mm1->getID());                // ..and now we get a Media ref
            ASSERT (mX1 == mm1);
            ASSERT (mX1 == aa1);
            
            ASSERT (aMang.known (mm1->getID()));
            ASSERT (aMang.known (mm2->getID()));
            ASSERT (aMang.known (mm3->getID()));
            
            ASSERT ( !aMang.known (mm3->getID(), Category(AUDIO))); // not found within AUDIO-Category
            try 
              { // can't be found if specifying wrong Asset kind.... 
                aMang.getAsset (ID<asset::Proc>(mm1->getID()));
                NOTREACHED;
              }
            catch (lumiera::error::Invalid& xxx) {ASSERT (xxx.getID()==LUMIERA_ERROR_WRONG_ASSET_KIND);}
            try 
              { // try accessing nonexistant ID 
                aMang.getAsset (ID<Asset> (1234567890));
                NOTREACHED;
              }
            catch (lumiera::error::Invalid& xxx) {ASSERT (xxx.getID()==LUMIERA_ERROR_UNKNOWN_ASSET_ID);}
            lumiera_error (); // reset errorflag
            
            
            // checking the Ident-Fields
            ASSERT (mm1->ident.name == "Name-1");
            ASSERT (mm2->ident.name == "testfile1");
            ASSERT (mm3->ident.name == "testfile2");

            ASSERT (cat == Category (VIDEO,"bin1"));
            ASSERT (mm1->ident.category == Category (VIDEO,"bin1"));
            ASSERT (mm2->ident.category == Category (VIDEO,"bin1"));
            ASSERT (mm3->ident.category == Category (VIDEO       ));

            ASSERT (mm1->ident.org == "ichthyo");
            ASSERT (mm2->ident.org == "lumi");
            ASSERT (mm3->ident.org == "lumi");

            ASSERT (mm1->ident.version == 5);
            ASSERT (mm2->ident.version == 1);
            ASSERT (mm3->ident.version == 1);

            ASSERT (mm1->getFilename() == "testfile.mov");
            ASSERT (mm2->getFilename() == "testfile1.mov");
            ASSERT (mm3->getFilename() == "testfile2.mov");
            
            
            TRACE (assetmem, "leaving test method scope");
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
            ASSERT ( checkProperties (candi, key1, ""));

            candi = asset::Media::create(key1, string("testfile.wav"));
            ASSERT ( checkProperties (candi, key1, "testfile.wav"));

            Asset::Ident key2("", Category(AUDIO), "ichthyo", 5);
            candi = asset::Media::create(key2, string("testfile2.wav"));
            ASSERT ( checkProperties (candi, key2, "testfile2.wav"));
            ASSERT (key2.name == "testfile2"); // name filled in automatically 

            candi = asset::Media::create(string("testfile3.wav"), Category(AUDIO));
            ASSERT ( checkProperties (candi, Asset::Ident("testfile3", Category(AUDIO), "lumi", 1)
                                           , "testfile3.wav"));

            candi = asset::Media::create("some/path/testfile4.wav", Category(AUDIO));
            ASSERT ( checkProperties (candi, Asset::Ident("testfile4", Category(AUDIO), "lumi", 1)
                                           , "some/path/testfile4.wav"));

            candi = asset::Media::create("", Category(AUDIO,"sub/bin"));
            ASSERT ( checkProperties (candi, Asset::Ident("nil", Category(AUDIO,"sub/bin"), "lumi", 1)
                                           , ""));

            candi = asset::Media::create("", AUDIO);
            ASSERT ( checkProperties (candi, Asset::Ident("nil", Category(AUDIO), "lumi", 1)
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
    
    
    
  } // namespace test

} // namespace asset
