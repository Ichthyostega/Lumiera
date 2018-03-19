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
 */



#include "lib/test/run.hpp"
#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include "lib/depend2.hpp"
#include "lib/depend-inject.hpp"

#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;
#define SHOW_EXPR(_XX_) \
    cout << "Probe " << STRINGIFY(_XX_) << " ? = " << _XX_ <<endl;


namespace lib {
namespace test{
  
  using ::Test;
//  using util::isSameObject;
  
  namespace {
    
struct Dum
  : boost::noncopyable
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
   ~Dummy() { checksum -= N; cout << "~Dummy<"<<N<<">"<<endl;}
    
    virtual int
    probe()  override
      {
        return N * checksum;
      }
  };


using error::LUMIERA_ERROR_LIFECYCLE;
using error::LUMIERA_ERROR_FATAL;

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
   */
  class DependencyConfiguration_test : public Test
    {
      
      
      virtual void
      run (Arg)
        {
          verify_SubclassCreation();
          verify_customFactory();
          verify_automaticReplacement();
          
          Depend<Dummy<1>> dep11;
          Depend<Dummy<5>> dep5;
          Depend<Dummy<1>> dep12;
          
          cout << "Siz-DUM : " << lib::test::showSizeof(dep11) << " " << lib::test::showSizeof(dep5) << endl;
          cout << "check-vor="<<checksum<<endl;
          
          SHOW_EXPR( dep11().probe() );
          SHOW_EXPR( checksum );
          SHOW_EXPR( dep5().probe() );
          SHOW_EXPR( checksum );
          SHOW_EXPR( dep12().probe() );
          SHOW_EXPR( checksum );
          
          // unable to create singleton instance of abstract baseclass
          VERIFY_ERROR (FATAL, Depend<Dum>{}() );
      
          Depend<Dum> dumm;
          DependInject<Dum>::useSingleton<Dummy<7>>();
          SHOW_EXPR( dumm().probe() );
          SHOW_EXPR( checksum );
          VERIFY_ERROR (LIFECYCLE, DependInject<Dum>::useSingleton<Dummy<9>>() );
          SHOW_EXPR( Depend<Dum>{}().probe() );
          SHOW_EXPR( checksum );
          
          struct SubDummy
            : Dummy<3>
            {
              virtual int
              probe()  override
                {
                  return -checksum + offset;
                }
              
              int offset = 0;
            };
          
          Depend<Dummy<3>> dep3;
          SHOW_EXPR( checksum );
          {
            DependInject<Dummy<3>>::ServiceInstance<SubDummy> service{};
            CHECK (service);
            SHOW_EXPR( checksum );
            SHOW_EXPR( dep3().probe() );
            SHOW_EXPR( checksum );
            service->offset = 5;
            SHOW_EXPR( dep3().probe() );
            SHOW_EXPR( checksum );
          }
          SHOW_EXPR( checksum );
          VERIFY_ERROR (LIFECYCLE, dep3().probe() );
          VERIFY_ERROR (LIFECYCLE, DependInject<Dum>::ServiceInstance<SubDummy>{} );
          SHOW_EXPR( checksum );
          
          {
            DependInject<Dum>::Local<SubDummy>      mockDum;
            DependInject<Dummy<3>>::Local<SubDummy> mockDummy3;
            CHECK (!mockDum);
            CHECK (!mockDummy3);
            SHOW_EXPR( dumm().probe() );
            CHECK ( mockDum);
            CHECK (!mockDummy3);
            SHOW_EXPR( checksum );
            SHOW_EXPR( mockDum->probe() );
            SHOW_EXPR( checksum );
            mockDum->offset = -4;
            SHOW_EXPR( dumm().probe() );
            
            CHECK (!mockDummy3);
            SHOW_EXPR( checksum );
            SHOW_EXPR( dep3().probe() );
            SHOW_EXPR( checksum );
            CHECK ( mockDummy3);
            SHOW_EXPR( mockDummy3->probe() );
            SHOW_EXPR( checksum );
            mockDummy3->offset = 19;
            SHOW_EXPR( dep3().probe() );
            mockDum->offset = -6;
            SHOW_EXPR( dep3().probe() );
            SHOW_EXPR( dumm().probe() );
            SHOW_EXPR( checksum );
          }
          SHOW_EXPR( checksum );
          SHOW_EXPR( dumm().probe() );
          VERIFY_ERROR (LIFECYCLE, dep3().probe() );
          SHOW_EXPR( checksum );
          {
            DependInject<Dummy<3>>::ServiceInstance<SubDummy> service{};
            SHOW_EXPR( checksum );
            SHOW_EXPR( dep3().probe() );
            service->offset = 5;
            SHOW_EXPR( dep3().probe() );
            SHOW_EXPR( checksum );
            {
              DependInject<Dummy<3>>::Local<SubDummy> mockDummy31;
              CHECK (!mockDummy31);
              SHOW_EXPR( checksum );
              SHOW_EXPR( dep3().probe() );
              SHOW_EXPR( checksum );
              mockDummy31->offset = 10;
              SHOW_EXPR( dep3().probe() );
              SHOW_EXPR( mockDummy31->probe() );
              SHOW_EXPR( service->probe() );
              CHECK (mockDummy31->offset != service->offset);
              service->offset = 20;
              SHOW_EXPR( dep3().probe() );
              SHOW_EXPR( mockDummy31->probe() );
              SHOW_EXPR( service->probe() );
              SHOW_EXPR( checksum );
            }
            SHOW_EXPR( checksum );
            SHOW_EXPR( dep3().probe() );
            SHOW_EXPR( checksum );
          }
          SHOW_EXPR( checksum );
          VERIFY_ERROR (LIFECYCLE, dep3().probe() );
          SHOW_EXPR( dumm().probe() );
          SHOW_EXPR( checksum );
      }
      
      
      void
      verify_SubclassCreation()
        {
        }
      
      
      void
      verify_customFactory()
        {
        }
      
      
      
      void
      verify_automaticReplacement()
        {
        }
    };
  
  
  
  LAUNCHER (DependencyConfiguration_test, "unit common");
  
  
}} // namespace lib::test
