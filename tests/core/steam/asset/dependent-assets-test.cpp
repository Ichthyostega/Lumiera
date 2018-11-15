/*
  DependentAssets(Test)  -  check the asset dependency handling

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

/** @file dependent-assets-test.cpp
 ** unit test \ref DependentAssets_test
 */


#include "lib/test/run.hpp"
#include "steam/asset/testasset.hpp"
#include "steam/asset/asset-diagnostics.hpp"
#include "vault/media-access-mock.hpp"
#include "lib/depend-inject.hpp"
#include "steam/asset/media.hpp"
#include "steam/asset/clip.hpp"
#include "lib/util.hpp"

using util::contains;
using util::isnil;


namespace steam {
namespace asset{
namespace test {
  
  using MediaAccessMock = lib::DependInject<vault::MediaAccessFacade>
                                ::Local<vault::test::MediaAccessMock>;
  
  
  
  
  /***************************************************************//**
   * @test the handling of Assets dependent on other Assets and the
   *       enabling/disabling of Assets.
   * @see  asset::Asset
   * @see  asset::Clip
   */
  class DependentAssets_test : public Test
    {
      virtual void run (Arg)
        {
           checkDependencyMechanics ();
           checkUnlinking ();
           checkEnablementPropagation ();
           checkRealAssetDependencyRegistration ();
        }
      
      typedef TestAsset<Asset> TA;
      typedef TA::PA PTestA;
      
      
      /** @test check operation of basic asset dependency support
       */
      void checkDependencyMechanics ()
        {
          PAsset a1 = TA::create();
          CHECK (isnil (a1->getParents()));
          CHECK (isnil (a1->getDependant()));
          
          PTestA a2 = TA::create(a1);
          CHECK (a1 == a2->getParents()[0]);  // TestAsset registered a1 as parent
          CHECK (a2 == a1->getDependant()[0]);
          
          PAsset a3 = TA::create();
          a2->set_depend(a3);
          CHECK (a3 == a2->getParents()[1]);
          CHECK (a2 == a3->getDependant()[0]);
          CHECK (!contains (a1->getDependant(), a3));
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
          CHECK (a1 == a2->getParents()[0]);
          CHECK (a2 == a1->getDependant()[0]);
          CHECK (a2 == a3->getParents()[0]);
          CHECK (a3 == a2->getDependant()[0]);
          
          a2_->call_unlink();
          CHECK (isnil (a2->getDependant()));
          CHECK (!contains (a1->getDependant(), a2));  // has been propagated up
          CHECK (!isnil (a2->getParents()));
          CHECK (contains (a3->getParents(), a2));   // but up-links remain intact
          
          a2_->call_unlink(a1->getID());
          a2_->set_depend(a1);
          PAsset a4 = TA::create(a1);
          CHECK (a1 == a2->getParents()[0]);
          CHECK (a1 == a4->getParents()[0]);
          CHECK (a2 == a1->getDependant()[0]);
          CHECK (a4 == a1->getDependant()[1]);
          
          a1_->call_unlink(a4->getID());
          CHECK (!contains (a1->getDependant(), a4));  // selectively removed
          CHECK ( contains (a1->getDependant(), a2));
          CHECK (a1 == a4->getParents()[0]);           // no propagation
        }
      
      
      /** @test enabling and disabling an asset should
       *        propagate to dependant assets
       */
      void checkEnablementPropagation ()
        {
          PAsset a1 = TA::create();
          PTestA a2_= TA::create(a1);
          PAsset a2 (a2_);
          PAsset a3 = TA::create();   // not dependant
          
          CHECK (a1->isActive());
          CHECK (a2->isActive());
          CHECK (a3->isActive());
          
          a1->enable(false);
          CHECK (!a1->isActive());
          CHECK (!a2->isActive());
          CHECK (a3->isActive());
          
          a2->enable(true);
          CHECK (!a1->isActive());
          CHECK (!a2->isActive());  // ignored because parent is disabled
          
          a1->enable(true);
          CHECK (a1->isActive());
          CHECK (a2->isActive());
          
          a2->enable(false);
          CHECK (a1->isActive());
          CHECK (!a2->isActive());  // disabling not propagated to parent
          a2->enable(true);
          CHECK (a1->isActive());
          CHECK (a2->isActive());
          
          a3->enable(false);
          CHECK (a1->isActive());
          CHECK (a2->isActive());
          CHECK (!a3->isActive());  // no dependency...
          
          a1->enable(false);
          a3->enable();
          CHECK (!a1->isActive());
          CHECK (!a2->isActive());
          CHECK (a3->isActive());
          
          a1->enable();
          a2_->set_depend(a3); // now add a new parent dependency
          a3->enable(false);
          CHECK (a1->isActive());
          CHECK (!a2->isActive()); // has been propagated via the new dependency
          CHECK (!a3->isActive());
          
          a2->enable(true);
          CHECK (a1->isActive());  // no change because one of the parents is disbled
          CHECK (!a2->isActive());
          CHECK (!a3->isActive());
          a1->enable(false);
          CHECK (!a1->isActive());
          a3->enable(true);
          CHECK (!a1->isActive());  // no propagation because the disabled other parent (a1)
          CHECK (!a2->isActive());
          CHECK (a3->isActive());
          a1->enable(true);
          CHECK (a1->isActive());  // but now propagation is possible
          CHECK (a2->isActive());
          CHECK (a3->isActive());
        }
      
      
      /** @test each real world asset subclass has to care
       *        for registering and deregistering any additional
       *        dependencies. Here we collect some more prominent
       *        examples (and hopefully don't fail to cover any
       *        important special cases...)
       */
      void checkRealAssetDependencyRegistration ()
        {
          MediaAccessMock useMockMedia;
          
          // -----Media and Clip--------------------------------
          typedef lib::P<Media> PM;
          typedef lib::P<Clip>  PC;
          PM mm = asset::Media::create("test-1", VIDEO);
          PC cc = mm->createClip()->findClipAsset();
          CHECK (dependencyCheck (cc,mm));
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (DependentAssets_test, "unit function asset");
  
  
  
}}} // namespace steam::asset::test
