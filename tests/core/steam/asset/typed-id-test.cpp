/*
  TypedID(Test)  -  verifying registration service for ID to type association

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file typed-id-test.cpp
 ** unit test \ref TypedID_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"

#include "proc/asset/typed-id.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/p.hpp"
#include "proc/assetmanager.hpp"
#include "proc/asset/inventory.hpp"
//#include "proc/mobject/session/clip.hpp"
//#include "proc/mobject/session/fork.hpp"
//#include "lib/meta/trait-special.hpp"
#include "lib/util-foreach.hpp"
#include "lib/format-cout.hpp"
#include "lib/symbol.hpp"

//#include <unordered_map>
//#include <string>

//using lib::test::showSizeof;
//using lib::test::randStr;
//using util::isSameObject;
//using util::and_all;
using util::for_each;
using util::isnil;
//using lib::Literal;
using lib::Symbol;
using lib::P;
//using std::string;



namespace proc {
namespace mobject {
namespace session {
namespace test {
  
  struct DummyEntity { };    ////////////////////////TODO of course the key idea is to mix in TypedID::link, to get an automatic registration and deregistartion
  
  typedef P<DummyEntity> PDum;
  
}}}}

namespace lumiera{  ///////TODO: shouldn't that be namespace lib? or steam?
namespace query  {
  
  /**
   * Adapter for using mobject::session::test::DummyEntity as a
   * \em primary type with the TypedID registration service.
   * Of course this is a test/dummy/demonstration.
   */
  template<>
  struct TypeHandlerXX<proc::mobject::session::test::DummyEntity>
    {
      static Symbol getID() { return "typed-id-test-dummy"; }
      
    };
  
}}



namespace lib {
namespace idi {
namespace test{
  
  using proc::mobject::session::test::DummyEntity;
  using proc::mobject::session::test::PDum;
  
  using DummyID = EntryID<DummyEntity>;
  
  namespace { // Test definitions...
    
  }
  
  
  
  /***********************************************************************//**
   * @test outline of an instance registration and accounting service.
   *       The service is self-contained and supports automatic registration
   *       and deregistration. The instance access is configurable on a 
   *       per-type base.
   *       - create instances of an auto-registered type
   *       - verify the registration entries
   *       - re-access the instances just by symbolic ID
   *       - verify the registration is cleaned up automatically.
   * 
   * @todo draft from 2010 -- partially unimplemented and thus commented out //////////////////////////////////////////STICKET #599
   * 
   * @see typed-id.hpp interface covered here
   * @see typed-lookup.cpp implementation
   */
  class TypedID_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verifyRegistration();
          verifyAssetFrontend();
          verifyInstanceAccess();
          verifyAutomaticCleanup();
        }
      
      
      void
      verifyRegistration()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #599
          uint type_cnt = TypedID::type_count();
          uint elm_cnt = TypedID::element_count();
          
          PDum d1 = TypedID::get<DummyEntity> ("top");
          CHECK (!d1); // doesn't exist
          
          // the above request automatically opened a new category
          CHECK (type_cnt+1 == TypedID::type_count());
          
          PDum up (new DummyEntity ("up"));
          PDum down (new DummyEntity ("down"));
          
          CHECK (elm_cnt+2 == TypedID::element_count());
          
          DummyID idu = TypedID::getID<DummyEntity> ("up");
          CHECK (idu);
          CHECK ("up" == idu.getSym());
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #599
        }
      
      
      void
      verifyAssetFrontend()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #599
          PInv inventory = asset::Meta::create (Category (META,"typed-id-test-dummy"));
          CHECK (inventory);
          CHECK (0 == inventory->size());
          
          // The Inventory is a first-class Asset
          CHECK (AssetManager::instance().known (inventory->getID()));
          
          PDum up (new DummyEntity ("up"));
          PDum down (new DummyEntity ("down"));
          
          // changes reflected immediately
          CHECK (2 == inventory->size());
          
          for_each (inventory->all(), show<BareEntryID> );
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #599
        }
      
      template<typename X>
      static void
      show (X& o)
        {
          cout << "---" << o << endl;
        }
      
      
      void
      verifyInstanceAccess()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #599
          PDum top (new DummyEntity ("top"));
          PDum bot (new DummyEntity ("bottom"));
          
          CHECK (1 == top.use_count());
          CHECK (1 == bot.use_count());              // we hold the only reference
          
          VERIFY_ERROR (MISSING_INSTANCE, TypedID::get<DummyEntity> ("top"));
          
          // to enable fetching instances, an instance link needs to be provided
          top->registerInstance (top);
          TypedID::registerInstance<DummyEntity> (bot);
          
          PDum d1 = TypedID::get<DummyEntity> ("top");
          CHECK (d1);
          CHECK (d1 == top);
          CHECK (d1 != bot);
          CHECK (2 == top.use_count());
          CHECK (1 == bot.use_count());
          
          d1 = TypedID::get<DummyEntity> ("bottom");
          CHECK (d1);
          CHECK (d1 != top);
          CHECK (d1 == bot);
          CHECK (1 == top.use_count());
          CHECK (2 == bot.use_count());
          
          for_each (TypedID::allInstances<DummyEntity>(), show<DummyEntity> );
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #599
        }
      
      
      void
      verifyAutomaticCleanup()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #599
          
          PDum cha (new DummyEntity ("charm"));
          PDum bea (new DummyEntity ("beauty"));
          
          CHECK (2 == TypedID::entry_count<DummyEntity>());
          
          cha.reset();
          CHECK (1 == TypedID::entry_count<DummyEntity>());
          CHECK (bea == *(TypedID::allInstances<DummyEntity>()));
          
          bea.reset();
          CHECK (0 == TypedID::entry_count<DummyEntity>());
          CHECK (! (TypedID::get<DummyEntity>("beauty")));
          CHECK (isnil (TypedID::allInstances<DummyEntity>()));
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #599
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (TypedID_test, "unit asset");
  
  
}}} // namespace lib::idi::test
