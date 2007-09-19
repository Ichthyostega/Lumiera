/*
  IdentityOfAssets(Test)  -  Asset object identity and versioning
 
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
#include "common/util.hpp"

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
     * @test creating several Assets and checking object identity,
     *       detection of duplicates and version handling.
     * @see  proc_interface::AssetManager#reg
     * @todo to be written; features are missing as of 9/07
     */
    class IdentityOfAssets_test : public Test
      {
        virtual void run(Arg arg) 
          {
            createDuplicate();
            
            if (!isnil (arg))
              dumpAssetManager();
            TRACE (assetmem, "leaving IdentityOfAssets_test::run()");
          } 
        
        
        
        typedef shared_ptr<asset::Media> PM;
        
        /** @test produce an ID clash.
         *        documents the current behaviour of the code as of 9/07
         *  @todo this test is expected to break when the detection
         *        of duplicate registrations is implemented.
         */
        void createDuplicate()
          { 
            PM mm1 = asset::Media::create ("testfile1.mov", VIDEO);
            
            Asset::Ident idi (mm1->ident);         // duplicate Ident record
            PM mm1X = asset::Media::create (idi); //  note: we actually don't call any ctor
            ASSERT (mm1 == mm1X);                //         instead, we got mm1 back.
            
            PM mm2 = asset::Media::create (idi,"testfile2.mov");
            
            ASSERT (mm1->getID() == mm2->getID()); // different object, same hash

            AssetManager& aMang = AssetManager::instance();
            ASSERT (aMang.getAsset (mm1->getID()) == mm2); // record of mm1 was replaced by mm2
            ASSERT (aMang.getAsset (mm2->getID()) == mm2);
            
            ASSERT (aMang.known (mm1->getID()));
            ASSERT (aMang.known (mm2->getID()));
            ASSERT (mm1->ident.name == "testfile1");
            ASSERT (mm2->ident.name == "testfile1");
            ASSERT (mm1->getFilename() == "testfile1.mov");
            ASSERT (mm2->getFilename() == "testfile2.mov");
            
            
            TRACE (assetmem, "leaving test method scope");
          }
        
      };
    
    
    /** Register this test class... */
    LAUNCHER (IdentityOfAssets_test, "unit asset");
    
    
    
  } // namespace test

} // namespace asset
