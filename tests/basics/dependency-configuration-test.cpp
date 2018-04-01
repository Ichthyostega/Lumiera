/*
  DependencyConfiguration(Test)  -  verify configuration for injecting dependencies

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file dependency-configuration-test.cpp
 ** unit test \ref DependencyConfiguration_test
 ** @remark this test was written 3/2018 as a byproduct of the third rewrite
 **         of the framework for singletons and dependency-injection. It is
 **         quite redundant with the previously existing DependencyFactory_test
 */



#include "lib/test/run.hpp"
#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include "lib/depend.hpp"
#include "lib/depend-inject.hpp"



namespace lib {
namespace test{
  
  using ::Test;
  using util::isSameObject;
  
  namespace {
    
    struct Dum
      : util::NonCopyable
      {
        virtual ~Dum() { }
        virtual int probe()  =0;
      };
    
    
    int checksum = 0;
    
    template<int N>
    struct Dummy
      : Dum
      {
        Dummy() { checksum += N; }
       ~Dummy() { checksum -= N; }
        
        virtual int
        probe()  override
          {
            return N * checksum;
          }
      };
  }
  
  using error::LERR_(LIFECYCLE);
  using error::LERR_(FATAL);
  
  
  
  
  
  /***************************************************************************//**
   * @test verify the various modes of creating dependencies.
   *       - standard case is singleton creation
   *       - configuration of a specific subclass for the singleton
   *       - expose a service with explicit lifecycle
   *       - use of a custom factory function
   *       - injection of a mock implementation for unit tests
   * 
   * @remark this test basically covers the same ground as DependencyFactory_test;
   *         but while the latter exists since our second rewrite of lib::Depend (2013),
   *         this test here is a byproduct of the third rewrite from 2018 and focuses
   *         more on the configuration and instance identities.
   * @see lib::Dependency
   * @see Singleton_test
   */
  class DependencyConfiguration_test
    : public Test
    {
      
      
      virtual void
      run (Arg)
        {
          checksum = 0;
          
          verify_Singleton();
          verify_SubclassSingleton();
          verify_expose_Service_with_Lifecycle();
          verify_automaticReplacement();
          verify_customFactory();
          
          CHECK (9+7+5+1 == checksum); // singletons stay alive until application shutdown
      }
      
      
      /** @test without special configuration, singletons are injected as dependency */
      void
      verify_Singleton()
        {
          Depend<Dummy<1>> dep11;
          Depend<Dummy<5>> dep5;
          Depend<Dummy<1>> dep12;
          
          CHECK (1 == sizeof(dep11));
          CHECK (1 == sizeof(dep12));
          CHECK (1 == sizeof(dep5));
          
          // no singleton instance created yet
          CHECK ( 0 == checksum );
          
          CHECK ( 1*1 == dep11().probe() );
          CHECK ( 1 == checksum );
          CHECK ((1+5)*5 == dep5().probe() );
          CHECK ((1+5) == checksum );
          CHECK ((1+5)*1 == dep12().probe() );
          CHECK ((1+5) == checksum );
          
          CHECK (not isSameObject (dep11, dep12));
          CHECK (    isSameObject (dep11(), dep12()));
        }
      
      
      /** @test preconfigure a specific subclass to be injected as singleton dependency */
      void
      verify_SubclassSingleton()
        {
          // unable to create singleton instance of abstract baseclass
          VERIFY_ERROR (FATAL, Depend<Dum>{}() );

          CHECK ((1+5) == checksum );
      
          Depend<Dum> dumm;
          DependInject<Dum>::useSingleton<Dummy<7>>();
          CHECK ((1+5) == checksum );
          
          CHECK ((1+5+7)*7 == dumm().probe() );
          CHECK ((1+5+7) == checksum );
          
          VERIFY_ERROR (LIFECYCLE, DependInject<Dum>::useSingleton<Dummy<9>>() );
          CHECK ((1+5+7)*7 == Depend<Dum>{}().probe() );
          CHECK ((1+5+7)*7 == dumm().probe() );
          CHECK ((1+5+7) == checksum );
        }
      
      
      /** @test expose a dedicated service instance, which can be shut down */
      void
      verify_expose_Service_with_Lifecycle()
        {
          CHECK ((1+5+7) == checksum );
          
          struct SubDummy
            : Dummy<3>
            {
              virtual int
              probe()  override
                {
                  return offset - checksum;
                }
              
              int offset = 0;
            };
          
          Depend<Dummy<3>> dep3;
          CHECK ((1+5+7) == checksum );
          {
            DependInject<Dummy<3>>::ServiceInstance<SubDummy> service{};
            CHECK (service);
            CHECK ((1+5+7+3) == checksum );
            CHECK (-(1+5+7+3) == dep3().probe() );
            CHECK ((1+5+7+3) == checksum );
            service->offset = (1+5+7);
            CHECK ( -3 == dep3().probe() );
            CHECK ((1+5+7+3) == checksum );
          }
          CHECK ((1+5+7) == checksum );
          VERIFY_ERROR (LIFECYCLE, dep3().probe() );
          VERIFY_ERROR (LIFECYCLE, DependInject<Dum>::ServiceInstance<SubDummy>{} );
          CHECK ((1+5+7) == checksum );
        }
      
      
      
      /** @test injecting test mocks temporarily */
      void
      verify_automaticReplacement()
        {
          Depend<Dum> dumm;
          Depend<Dummy<3>> depp;
          CHECK ((1+5+7) == checksum );
          CHECK ((1+5+7)*7 == dumm().probe() );
          VERIFY_ERROR (LIFECYCLE, depp().probe() );
          
          struct Mock
            : Dummy<3>
            {
              virtual int
              probe()  override
                {
                  return response;
                }
              
              int response = -1;
            };
          
          {////////////////////////////////////////////////////TEST-Scope
            DependInject<Dum>::Local<Mock>       mockDumm;
            DependInject<Dummy<3>>::Local<Mock>  mockDummy3;
            CHECK ((1+5+7) == checksum );
            
            CHECK (!mockDumm);
            CHECK (!mockDummy3);
            CHECK (-1 == dumm().probe() );       // NOTE: now returning the response from the mock instance
            CHECK ( mockDumm);
            CHECK (!mockDummy3);
            CHECK ((1+5+7+3) == checksum );
            CHECK (-1 == mockDumm->probe() );
            CHECK ((1+5+7+3) == checksum );
            
            mockDumm->response = 11;
            CHECK (11 == dumm().probe() );       // NOTE: now returning the response changed on the mock instance
            
            CHECK (!mockDummy3);                 // the second mock is still in not yet created state...
            CHECK ((1+5+7+3) == checksum );
            CHECK (-1 == depp().probe() );
            CHECK ((1+5+7+3+3) == checksum );    // ...and now we got a second mock instance!
            CHECK ( mockDummy3);
            CHECK (-1 == mockDummy3->probe() );
            CHECK ((1+5+7+3+3) == checksum );
            mockDummy3->response = 22;
            CHECK (22 == depp().probe() );
            mockDumm->response = 12;
            CHECK (22 == depp().probe() );       // these are really two distinct instances
            CHECK (12 == dumm().probe() );
            CHECK ((1+5+7+3+3) == checksum );
          }////////////////////////////////////////////////////(End)TEST-Scope
           
          // Back to normal: the Mocks are gone, original behaviour uncovered
          CHECK ((1+5+7) == checksum );
          CHECK ((1+5+7)*7 == dumm().probe() );
          VERIFY_ERROR (LIFECYCLE, depp().probe() );
          CHECK ((1+5+7) == checksum );
          
          {/////////////////////////////////////////////////////////Service-Scope
            DependInject<Dummy<3>>::ServiceInstance<Mock> service{};
            CHECK ((1+5+7+3) == checksum );      // NOTE: we got a new Dummy<3> service instance
            CHECK (-1 == depp().probe() );       // ..... which returns the pristine mock response
            service->response = 33;
            CHECK (33 == depp().probe() );
            CHECK ((1+5+7+3) == checksum );
            
            {/////////////////////////////////////////////////////////NESTED-TEST-Scope
              DependInject<Dummy<3>>::Local<Mock> mockDummy31;
              CHECK (!mockDummy31);
              CHECK ((1+5+7+3) == checksum );    // ...while SerivceInstance is created eagerly
              CHECK (-1 == depp().probe() );     // the Local mock instance is only created on-demand
              CHECK ((1+5+7+3+3) == checksum );
              mockDummy31->response = 44;
              CHECK (44 == depp().probe() );
              CHECK (44 == mockDummy31->probe() );
              CHECK (33 == service->probe() );
              CHECK (mockDummy31->response != service->response);
              service->response = 34;
              CHECK (44 == depp().probe() );     // NOTE: remains shadowed by the mockDummy
              CHECK (44 == mockDummy31->probe() );
              CHECK (34 == service->probe() );
              CHECK ((1+5+7+3+3) == checksum );
            }/////////////////////////////////////////////////////////(End)NESTED-TEST-Scope
            
            // Now the mock is gone and the service instance becomes uncovered
            CHECK ((1+5+7+3) == checksum );
            CHECK (34 == depp().probe() );       // now reveals the response changed from within the nested test scope
            CHECK ((1+5+7+3) == checksum );
          }/////////////////////////////////////////////////////////(End)Service-Scope
           
          // Back to normal: Mocks is gone, Service is shutdown, original behaviour uncovered
          CHECK ((1+5+7) == checksum );
          VERIFY_ERROR (LIFECYCLE, depp().probe() );
          CHECK ((1+5+7)*7 == dumm().probe() );
          CHECK ((1+5+7) == checksum );
        }
      
      
      
      /** @test instance creation can be preconfigured with a closure
       * Both Singleton and Test-Mock creation can optionally be performed through a
       * user provided Lambda or Functor. To demonstrate this, we use a `Veryspecial` local class,
       * which takes an `int&` as constructor parameter -- and we create the actual instance through
       * a lambda, which happens to capture a local variable by reference.
       * @warning this can be dangerous; in the example demonstrated here, the created singleton instance
       *          continues to live until termination of the test-suite. After leaving this test function,
       *          it thus holds a dangling reference, pointing into stack memory....
       */
      void
      verify_customFactory()
        {
          CHECK ((1+5+7) == checksum );
          
          struct Veryspecial
            : Dummy<9>
            {
              Veryspecial(int& ref)
                : magic_{ref}
              { }
              
              int& magic_;
              
              virtual int
              probe()  override
                {
                  return magic_++;
                }
            };

          // NOTE: the following is rejected due to missing default ctor
          DependInject<Dummy<9>>::useSingleton<Veryspecial>();
          VERIFY_ERROR (FATAL, Depend<Dummy<9>>{}() );
          
          
          int backdoor = 22;
          
          DependInject<Dummy<9>>::useSingleton ([&]{ return new Veryspecial{backdoor}; });
          
          CHECK ((1+5+7) == checksum );
          CHECK ( 22 == backdoor );
          
          Depend<Dummy<9>> tricky;
          CHECK ((1+5+7) == checksum );
          CHECK (22 == backdoor );
          
          CHECK (22 == tricky().probe());
          CHECK (23 == backdoor );
          CHECK ((1+5+7+9) == checksum );        // Veryspecial Dummy<9> subclass was created on the heap
                                                 // and will continue to live there until the testsuite terminates
          backdoor = 41;
          CHECK (41 == tricky().probe());
          CHECK (42 == backdoor );
          
          
          Depend<Dum> dumm;
          CHECK ((1+5+7+9)*7 == dumm().probe() );
          
          {////////////////////////////////////////////////////TEST-Scope
           
////////////// NOTE: the following does not compile
//          //       since Veryspecial has no default ctor...
//          //
//          DependInject<Dum>::Local<Dum> impossible;
            
            DependInject<Dum>::Local<Dum> insidious ([&]{ return new Veryspecial{backdoor}; });
            
            CHECK ((1+5+7+9) == checksum );
            CHECK (not insidious);
            
            CHECK (42 == dumm().probe() );
            CHECK (43 == backdoor );
            CHECK ((1+5+7+9+9) == checksum );
            
            CHECK (isSameObject (dumm(), *insidious));
            
            CHECK (43 == tricky().probe());
            CHECK (44 == backdoor );
            
            backdoor = -1;
            CHECK (-1 == dumm().probe() );
            CHECK ( 0 == backdoor );
            
            CHECK ((1+5+7+9+9) == checksum );
          }////////////////////////////////////////////////////(End)TEST-Scope
          
          CHECK ((1+5+7+9) == checksum );
          CHECK ((1+5+7+9)*7 == dumm().probe() );
          CHECK ( 0 == tricky().probe());
          CHECK (+1 == backdoor );
        }                                        // NOTE: Veryspecial holds a dangling reference into stack memory from now on!
    };
  
  
  
  LAUNCHER (DependencyConfiguration_test, "unit common");
  
  
}} // namespace lib::test
