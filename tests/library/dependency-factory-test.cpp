/*
  DependencyFactory(Test)  -  verify modes of creating singletons and dependencies

  Copyright (C)         Lumiera.org
    2013,               Hermann Vosseler <Ichthyostega@web.de>

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



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include "lib/depend.hpp"
#include "test-target-obj.hpp"

#include <cstdlib>



namespace lib {
namespace test{
  
  using ::Test;
  using util::isSameObject;
  
  namespace {
    
    const uint MAX_ID = 1000;
    
    struct Sub
      : TestTargetObj
      {
        static uint created = 0;
        uint instanceID_;
        
        Sub()
          : TestTargetObj(created++)
          ,instanceID_(rand() % MAX_ID)
          { }
        
        operator string()  const
          {
            return showType(*this)
                 + TestTargetObj::operator string();
          }
      };
    
    struct SubSub
      : Sub
      { };
    
    struct SubSubSub
      : SubSub
      { };
  }
  
  
  
  
  
  /*******************************************************************************
   * @test verify the various modes of creating dependencies.
   *       
   * @see lib::Dependency
   * @see Singleton_test
   */
  class DependencyFactory_test : public Test
    {
      
      
      virtual void
      run (Arg)
        {
          UNIMPLEMENTED ("dependency creation");
        }
      
      
      void
      verify_defaultSingletonCreation()
        {
          Depend<Sub> accessor1;
          Depend<Sub> accessor2;
          
          Sub & o1 = accessor1();
          Sub & o2 = accessor2();
          CHECK (isSameObject (o1, o2));
        }
      
      
      void
      verify_renewal()
        {
          Depend<Sub> accessor1;
          Depend<Sub> accessor2;
          uint id1 = accessor1().instanceID_;
          CHECK (id1 == accessor2().instanceID_);
          
          accessor1.shutdown();
          
          Sub & o2 = accessor2();
          uint id2 = accessor2().instanceID_;
          CHECK (id1 != id2);
          
          Sub & o1 = accessor1();
          CHECK (isSameObject (o1, o2));
          CHECK (id2 == accessor1().instanceID_);
        }
      
      
      void
      verify_SubclassCreation()
        {
          Depend<SubSub> specialAccessor(buildSingleton<SubSubSub>());
          Depend<Sub>    genericAccessor;
          
          SubSub& oSub = specialAccessor();
          Sub&    o    = genericAccessor();
          
          CHECK (!isSameObject (oSub, o));
          CHECK ( INSTANCEOF (SubSubSub, &oSub));
          CHECK (!INSTANCEOF (SubSubSub, &o));
        }
      
      
      void
      verify_FactoryDefinition_is_sticky()
        {
          Depend<SubSub> otherSpecialAccessor;
          
          SubSub& oSub = otherSpecialAccessor();
          CHECK ( INSTANCEOF (SubSubSub, &oSub));
          
          otherSpecialAccessor.shutdown();
          
          Depend<SubSub> yetAnotherSpecialAccessor;
          
          SubSub& yetAnotherInstance = yetAnotherSpecialAccessor();
          CHECK ( INSTANCEOF (SubSubSub, &yetAnotherInstance));
        }
      
      
      void
      verify_customFactory()
        {
          Depend<SubSubSub> customisedAccessor(useFactory (&customFactoryFunction));
          Depend<SubSub>    otherSpecialAccessor;
          
          SubSub&     oSub = otherSpecialAccessor();
          SubSubSub& oSubS = customisedAccessor();
          
          CHECK (!isSameObject (oSub, oSubS));
          CHECK ( INSTANCEOF (SubSubSub, &oSub));
          CHECK ( INSTANCEOF (SubSubSub, &oSubS));
          
          CHECK (oSub.instanceID_ != oSubS.instanceID_);
          CHECK (MAX_ID + 10      == oSubS.instanceID_);
        }
      
      static SubSubSub*
      customFactoryFunction (void)
        {
          SubSubSub* newObject = DependencyFactory::createSingleton<SubSubSub>();
          newObject->instanceID_ = MAX_ID + 10;
          return newObject;
        }
      
      
      
      void
      verify_temporaryReplacement()
        {
          Depend<Sub> genericAccessor;
          Sub& original = genericAccessor();
          uint oID = original.instanceID_;
          
          genericAccessor.injectReplacement (new SubSubSub);
          
          Sub& replacement = genericAccessor();
          uint repID = replacement.instanceID_;
          
          CHECK (!INSTANCEOF (SubSubSub, &original));
          CHECK ( INSTANCEOF (SubSubSub, &replacement));
          CHECK (!isSameObject (original, replacement));
          
          CHECK (oID != repID);
          CHECK (oID == original.instanceID_);
          
          Depend<SubSub>   special;
          Depend<SubSubSub> custom;
          
          CHECK(!isSameObject (replacement, special() ));
          CHECK(!isSameObject (replacement, custom()  ));
          
          genericAccessor.injectReplacement (NULL);
          
          Sub& nextFetch = genericAccessor();
          CHECK (isSameObject (original, nextFetch));
          CHECK (oID == nextFetch.instanceID_);
        }
      
      
      
      void
      verify_automaticReplacement()
        {
          Depend<Sub> genericAccessor;
          Sub& original = genericAccessor();
          uint oID = original.instanceID_;
          
          {
            Use4Test<SubSub> withinThisScope;
            
            Sub& replacement = genericAccessor();
            uint repID = replacement.instanceID_;
            
            CHECK (!INSTANCEOF (SubSub, &original));
            CHECK ( INSTANCEOF (SubSub, &replacement));
            CHECK (!INSTANCEOF (SubSubSub, &replacement));
            CHECK (!isSameObject (original, replacement));
            
            Depend<Sub> anotherAccessor;
            Sub& otherAccess = anotherAccessor();
            CHECK (isSameObject (replacement, otherAccess));
            CHECK (repID == otherAccess.instanceID_);
            CHECK (repID == replacement.instanceID_);
            CHECK (  oID == original.instanceID_);
          }
          
          Sub& nextFetch = genericAccessor();
          CHECK (isSameObject (original, nextFetch));
          CHECK (oID == nextFetch.instanceID_);
        }
    };
  
  
  
  LAUNCHER (DependencyFactory_test, "unit common");
  
  
}} // namespace lib::test
