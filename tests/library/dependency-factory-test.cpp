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

/** @file dependency-factory-test.cpp
 ** unit test \ref DependencyFactory_test
 ** @remark this is an old test from 2013 and thus verifies that the functionality
 **         for dependency-injection was not broken by the rewrite in 2018
 ** @see dependency-configuration-test.cpp
 */



#include "lib/test/run.hpp"
#include "lib/format-obj.hpp"
#include "lib/util.hpp"

#include "lib/depend.hpp"
#include "lib/test/depend-4test.hpp"
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
        static uint created;
        uint instanceID_;
        
        Sub()
          : TestTargetObj(created++)
          ,instanceID_(rand() % MAX_ID)
          { }
        
        virtual operator string()  const
          {
            return util::typeStr(this)
                 + TestTargetObj::operator string();
          }
      };
    uint Sub::created = 0;
    
    
    struct SubSub
      : Sub
      {
        /** marker typedef for Depend4Test,
         *  allowing to pick the correct Depend<ServiceInterface>
         *  to apply the instrumentation with the test mock. */
        typedef Sub ServiceInterface;
      };
    
    struct SubSubSub
      : SubSub
      { };
  }
  
  
  
  
  
  /***************************************************************************//**
   * @test verify the various modes of creating dependencies.
   *       - standard case is singleton creation
   *       - configuration of a specific subclass for the singleton
   *       - use of a custom factory function
   *       - injection of a mock implementation for unit tests
   * 
   * @see lib::Dependency
   * @see Singleton_test
   * @see DependencyConfiguration_test newer test from 2018 to cover the same ground
   */
  class DependencyFactory_test : public Test
    {
      
      
      virtual void
      run (Arg)
        {
          verify_defaultSingletonCreation();
          verify_SubclassCreation();
          verify_FactoryDefinition_is_sticky();
          verify_customFactory();
          verify_temporaryReplacement();
          verify_automaticReplacement();
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
          
          Depend<SubSub> yetAnotherSpecialAccessor;
          
          SubSub& yetAnotherInstance = yetAnotherSpecialAccessor();
          CHECK ( INSTANCEOF (SubSubSub, &yetAnotherInstance));
        }
      
      
      void
      verify_customFactory()
        {
          Depend<SubSubSub> customisedAccessor(&customFactoryFunction);
          Depend<SubSub>    otherSpecialAccessor;
          
          SubSub&     oSub = otherSpecialAccessor();
          SubSubSub& oSubS = customisedAccessor();
          
          CHECK (!isSameObject (oSub, oSubS));
          CHECK ( INSTANCEOF (SubSubSub, &oSub));
          CHECK ( INSTANCEOF (SubSubSub, &oSubS));
          
          CHECK (oSub.instanceID_ != oSubS.instanceID_);
          CHECK (MAX_ID + 10      == oSubS.instanceID_);
        }
      
      static void*
      customFactoryFunction (void)
        {
          static SubSubSub specialInstance;
              // NOTE: the factory function is responsible
              //       for managing the instance's lifecycle
          
          specialInstance.instanceID_ = MAX_ID + 10;
          return &specialInstance;
        }
      
      
      
      void
      verify_temporaryReplacement()
        {
          typedef Depend<Sub> GenericAccessor;
          
          GenericAccessor genericAccessor;
          Sub& original = genericAccessor();
          uint oID = original.instanceID_;
          
          SubSubSub mockObject;
          Sub* shaddowedOriginal = GenericAccessor::injectReplacement (&mockObject);
          
          Sub& replacement = genericAccessor();
          CHECK ( isSameObject (replacement, mockObject));
          CHECK (!isSameObject (original, replacement));
          CHECK ( isSameObject (original, *shaddowedOriginal));
          
          Depend<SubSub>   special;
          Depend<SubSubSub> custom;
          
          CHECK(!isSameObject (replacement, special() ));
          CHECK(!isSameObject (replacement, custom()  ));
          
          GenericAccessor::injectReplacement (shaddowedOriginal);
          
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
            Depend4Test<SubSub> withinThisScope;
            
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
            
             // verify the instrumentation indeed targeted the generic accessor,
            //  and *not* an accessor of the sub type, i.e Depend<SubSub>
            Depend<SubSub> genericSubTypeAccessor;
            SubSub& subTypeAccess = genericSubTypeAccessor();
            CHECK ( INSTANCEOF (SubSub, &subTypeAccess));
            CHECK (!isSameObject (replacement, subTypeAccess));
            CHECK (!isSameObject (original,    subTypeAccess));
            CHECK (repID != subTypeAccess.instanceID_);
            CHECK (  oID != subTypeAccess.instanceID_);
          }
          
          Sub& nextFetch = genericAccessor();
          CHECK (isSameObject (original, nextFetch));
          CHECK (oID == nextFetch.instanceID_);
        }
    };
  
  
  
  LAUNCHER (DependencyFactory_test, "unit common");
  
  
}} // namespace lib::test
