/*
  DependantAssets(Test)  -  unittest for the object creating factory
 
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
#include "proc/asset/testasset.hpp"
#include "common/util.hpp"

//#include <boost/format.hpp>
#include <iostream>

using util::isnil;
using util::contains;
//using boost::format;
using std::string;


namespace asset
  {
  namespace test
    {
    
    
    
    
    /*******************************************************************
     * @test the handling of Assets dependant on other Assets and the
     *       enabling/disabling of Assets.
     * @see  asset::Asset
     * @see  asset::Clip
     */
    class DependantAssets_test : public Test
      {
        virtual void run (Arg arg) 
          {
             checkDependencyMechanics ();
             checkUnlinking ();
             TODO ("activate missing testcases");
/////TODO             checkEnablementPropagation ();
/////TODO             checkRealAssetDependencyRegistration ();
          }
        
        typedef TestAsset<Asset> TA;
        typedef TA::PA PTestA;
        
        /** @test check operation of basic asset dependency support
         */
        void checkDependencyMechanics ()
          {
            PAsset a1 = TA::create();
            ASSERT (isnil (a1->getParents()));
            ASSERT (isnil (a1->getDependant()));
            
            PTestA a2 = TA::create(a1);
            ASSERT (a1 == a2->getParents()[0]);  // TestAsset registered a1 as parent
            ASSERT (a2 == a1->getDependant()[0]);
            
            TRACE(test, "a1.cnt=%i",a1.use_count());
            TRACE(test, "a2.cnt=%i",a2.use_count());
            PAsset a3 = TA::create();
            a2->set_depend(a3);
            ASSERT (a3 == a2->getParents()[1]);
            ASSERT (a2 == a3->getDependant()[0]);
            ASSERT (!contains (a1->getDependant(), a3));
          }
        
        
        /** @test unlink operation removing inter asset links
         */
        void checkUnlinking ()
          {
            PTestA a1_ = TA::create();
            PAsset a1 (a1_);
            PTestA a2_ = TA::create(a1);                   
            PAsset a2 (a2_);                   
            PAsset a3  = TA::create(a2);                   
            ASSERT (a1 == a2->getParents()[0]);
            ASSERT (a2 == a1->getDependant()[0]);
            ASSERT (a2 == a3->getParents()[0]);
            ASSERT (a3 == a2->getDependant()[0]);
            
            a2_->call_unlink();
            ASSERT (isnil (a2->getDependant()));
            ASSERT (!contains (a1->getDependant(), a2));  // has been propagated up
            ASSERT (!isnil (a2->getParents()));
            ASSERT (contains (a3->getParents(), a2));   // but up-links remain intact
            
            a2_->set_depend(a1);
            PAsset a4 = TA::create(a1);
            ASSERT (a1 == a2->getParents()[0]);
            ASSERT (a1 == a4->getParents()[0]);
            ASSERT (a2 == a1->getDependant()[0]);
            ASSERT (a4 == a1->getDependant()[1]);
            
            a1_->call_unlink(a4->getID());
            ASSERT (!contains (a1->getDependant(), a4));  // selectively removed
            ASSERT ( contains (a1->getDependant(), a2));
            ASSERT (a1 == a4->getParents()[0]);           // no propagation
          }
        
        
        /** @test enabling and disabling an asset should 
         *        propagate to dependant assets
         */
        void checkEnablementPropagation ()
          {
            PAsset a1 = TA::create();
            PAsset a2 = TA::create(a1);                   
            PAsset a3 = TA::create();   // not dependant
            
            ASSERT (a1->isActive());
            ASSERT (a2->isActive());
            ASSERT (a3->isActive());
            
            a1->enable(false);
            ASSERT (!a1->isActive());
            ASSERT (!a2->isActive());
            ASSERT (a3->isActive());
            
            a2->enable(true);
            ASSERT (!a1->isActive());
            ASSERT (!a2->isActive());  // ignored because parent is disabled
            
            a1->enable(true);
            ASSERT (a1->isActive());
            ASSERT (a2->isActive());
            
            a2->enable(false);
            ASSERT (a1->isActive());
            ASSERT (!a2->isActive());  // disabling not propagated to parent
            a2->enable(true);
            ASSERT (a1->isActive());
            ASSERT (a2->isActive());

            a3->enable(false);
            ASSERT (a1->isActive());
            ASSERT (a2->isActive());
            ASSERT (!a3->isActive());  // no dependency...

            a1->enable(false);
            a3->enable();
            ASSERT (!a1->isActive());
            ASSERT (!a2->isActive());
            ASSERT (a3->isActive());
          }
        
        
        /** @test each real world asset subclass has to care
         *        for registering and deregistering any additional
         *        dependencies. Here we collect some more prominent
         *        examples (and hopfully don't fail to cover any
         *        important special cases...)
         */
        void checkRealAssetDependencyRegistration ()
          {
            UNIMPLEMENTED ("handling of Asset dependencies");
          }
        
      };
    
    
    /** Register this test class... */
    LAUNCHER (DependantAssets_test, "unit function asset");
    
    
    
  } // namespace test

} // namespace asset
