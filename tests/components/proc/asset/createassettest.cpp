/*
  CreateAsset(Test)  -  constructing and registering Assets
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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


#include "common/test/run.hpp"
//#include "common/factory.hpp"
//#include "common/util.hpp"

#include "proc/assetmanager.hpp"
#include "proc/asset/media.hpp"

//#include <boost/format.hpp>
#include <iostream>

//using boost::format;
using std::string;
using std::cout;


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
          } 
        
        void createMedia()
          { 
            Category cat(VIDEO,"bin1");
            Asset::Ident key("Name-1", cat, "ichthyo", 5);
            P<asset::Media> mm1 = asset::Media::create(key,"testfile.mov");
            P<asset::Media> mm2 = asset::Media::create(key);
            P<asset::Media> mm3 = asset::Media::create("testfile2.mov",Category::VIDEO);
            
            // Assets have been registered and can be retrieved by ID
            ASSERT (AssetManager::getAsset (mm1.id) == mm1);
            ASSERT (AssetManager::getAsset (mm2.id) == mm2);
            ASSERT (AssetManager::getAsset (mm3.id) == mm3);
            
            ASSERT (AssetManager::getAsset (mm1.id) != mm2);
            
            PAsset aa1 = AssetManager::getAsset (mm1.id);
            ASSERT (aa1 == mm1);
            P<asset::Media> mX1 = AssetManager::getAsset (mm1.id, Category::VIDEO);
            ASSERT (mX1 == mm1);
            ASSERT (mX1 == aa1);
            try 
              { // can't be found if the Category is wrong.... 
                AssetManager::getAsset (mm1.id, Category::AUDIO);
                NOTREACHED;
              }
            catch (cinelerra::error::Invalid) { }
            
            
            // checking the Ident-Fields
            ASSERT (mm1->ident.name == "Name-1");
            ASSERT (mm2->ident.name == "Name-1");
            ASSERT (mm3->ident.name == "testfile2");

            ASSERT (cat == Category (VIDEO,"bin1"));
            ASSERT (mm1->ident.category == Category (VIDEO,"bin1"));
            ASSERT (mm2->ident.category == Category (VIDEO,"bin1"));
            ASSERT (mm3->ident.category == Category (VIDEO,""));

            ASSERT (mm1->ident.org == "ichthyo");
            ASSERT (mm2->ident.org == "ichthyo");
            ASSERT (mm3->ident.org == "cin3");

            ASSERT (mm1->ident.version == 5);
            ASSERT (mm2->ident.version == 5);
            ASSERT (mm3->ident.version == 1);

            ASSERT (mm1->getFilename() == "testfile.mov");
            ASSERT (isnil (mm2.getFilename());
            ASSERT (mm3->getFilename() == "testfile2.mov");

          }
      };
    
    
    /** Register this test class... */
    LAUNCHER (CreateAsset_test, "unit asset");
    
    
    
  } // namespace test

} // namespace asset
