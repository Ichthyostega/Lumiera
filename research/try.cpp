/* try.cpp  -  for trying out some language features....
 *             scons will create the binary bin/try
 *
 */

// 8/07  - how to control NOBUG??
//         execute with   NOBUG_LOG='ttt:TRACE' bin/try
// 1/08  - working out a static initialisation problem for Visitor (Tag creation)
// 1/08  - check 64bit longs
// 4/08  - comparison operators on shared_ptr<Asset>
// 4/08  - conversions on the value_type used for boost::any
// 5/08  - how to guard a downcasting access, so it is compiled in only if the involved types are convertible
// 7/08  - combining partial specialisation and subclasses
// 10/8  - abusing the STL containers to hold noncopyable values
// 6/09  - investigating how to build a mixin template providing an operator bool()
// 12/9  - tracking down a strange "warning: type qualifiers ignored on function return type"
// 1/10  - can we determine at compile time the presence of a certain function (for duck-typing)?
// 4/10  - pretty printing STL containers with python enabled GDB?
// 1/11  - exploring numeric limits
// 1/11  - integer floor and wrap operation(s)
// 1/11  - how to fetch the path of the own executable -- at least under Linux?
// 10/11 - simple demo using a pointer and a struct
// 11/11 - using the boost random number generator(s)
// 12/11 - how to detect if string conversion is possible?
// 1/12  - is partial application of member functions possible?
// 5/14  - c++11 transition: detect empty function object
// 7/14  - c++11 transition: std hash function vs. boost hash
// 9/14  - variadic templates and perfect forwarding
// 11/14 - pointer to member functions and name mangling
// 8/15  - Segfault when loading into GDB (on Debian/Jessie 64bit
// 8/15  - generalising the Variant::Visitor
// 1/16  - generic to-string conversion for ostream
// 1/16  - build tuple from runtime-typed variant container
// 3/17  - generic function signature traits, including support for Lambdas
// 9/17  - manipulate variadic templates to treat varargs in several chunks
// 11/17 - metaprogramming to detect the presence of extension points
// 11/17 - detect generic lambda
// 12/17 - investigate SFINAE failure. Reason was indirect use while in template instantiation
// 03/18 - Dependency Injection / Singleton initialisation / double checked locking


/** @file try.cpp
 ** Rework of the template lib::Depend for singleton and service access.
 */

typedef unsigned int uint;

#include "lib/format-cout.hpp"
#include "lib/depend.hpp"
#include "lib/depend2.hpp"
#include "lib/meta/util.hpp"
//#include "lib/meta/util.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include <boost/noncopyable.hpp>
#include <functional>
#include <type_traits>
#include <memory>


#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;
#define SHOW_EXPR(_XX_) \
    cout << "Probe " << STRINGIFY(_XX_) << " ? = " << _XX_ <<endl;


  namespace error = lumiera::error;
  
  using lib::ClassLock;
  using lib::meta::enable_if;
  
  
  namespace {
    template<typename TAR, typename SEL =void>
    class InstanceHolder
      : boost::noncopyable
      {
        std::unique_ptr<TAR> instance_;
        
        
      public:
        TAR*
        buildInstance()
          {
            if (instance_)
              throw error::Fatal("Attempt to double-create a singleton service. "
                                 "Either the application logic, or the compiler "
                                 "or runtime system is seriously broken"
                                ,error::LUMIERA_ERROR_LIFECYCLE);
            
            // place new instance into embedded buffer
            instance_.reset (new TAR{});
            return instance_.get();
          }
      };
    
    template<typename ABS>
    class InstanceHolder<ABS,  enable_if<std::is_abstract<ABS>>>
      {
      public:
        ABS*
        buildInstance()
          {
            throw error::Fatal("Attempt to create a singleton instance of an abstract class. "
                               "Application architecture or lifecycle is seriously broken.");
          }
      };
  }//(End)Implementation helper
  
  
  
  template<class SRV>
  class DependInject;
  
  template<class SRV>
  class Depend
    {
      using Factory = std::function<SRV*()>;
      
      static SRV* instance;
      static Factory factory;
      
      static InstanceHolder<SRV> singleton;
      
      friend class DependInject<SRV>;
    public:
      
      SRV&
      operator() ()
        {
          if (!instance)
            retrieveInstance();
          ENSURE (instance);
          return *instance;
        }
      
    private:
      void
      retrieveInstance()
        {
          ClassLock<SRV> guard;
          
          if (!instance)
            {
              if (!factory)
                instance = singleton.buildInstance();
              else
                instance = factory();
              factory = disabledFactory;
            }
        }
      
      static SRV*
      disabledFactory()
        {
          throw error::Fatal("Service not available at this point of the Application Lifecycle"
                            ,error::LUMIERA_ERROR_LIFECYCLE);
        }
    };
  
  
  template<class SRV>
  SRV* Depend<SRV>::instance;
  
  template<class SRV>
  typename Depend<SRV>::Factory Depend<SRV>::factory;
  
  template<class SRV>
  InstanceHolder<SRV> Depend<SRV>::singleton;
  
  
  ///////////////////////////////////////////////////////Configuration
  
  using std::move;
  
  
  template<class SRV>
  struct DependInject
    {
      using Factory = typename Depend<SRV>::Factory;
      
      
      /** configure dependency-injection for type SRV to build a subclass singleton
       * @tparam SUB concrete subclass type to build on demand when invoking `Depend<SRV>`
       * @throws error::Logic (LUMIERA_ERROR_LIFECYCLE) when the default factory has already
       *         been invoked at the point when calling this (re)configuration function.
       */
      template<class SUB>
      static void
      useSingleton()
        {
          __assert_compatible<SUB>();
          static InstanceHolder<SUB> singleton;
          installFactory ([&]()
                              {
                                return singleton.buildInstance();
                              });
        }
      
      
      /**
       * Configuration handle to expose a service implementation through the `Depend<SRV>` front-end.
       * This noncopyable (but movable) handle shall be planted within the context operating the service
       * to be exposed. It will immediately create (in RAII style) and manage a heap-allocated instance
       * of the subclass `IMP` and expose a baseclass pointer to this specific instance through `Depend<SRV>`.
       * Moreover, the implementation subclass can be accessed through this handle, which acts as smart-ptr.
       * When the handle goes out of scope, the implementation instance is destroyed and the access through
       * `Depend<SRV>` is closed and inhibited, to prevent on-demand creation of a baseclass `SRV` singleton.
       * @tparam IMP concrete service implementation subclass to build, manage and expose.
       * @throws error::Logic (LUMIERA_ERROR_LIFECYCLE) when the default factory has already
       *         been invoked at the point when calling this (re)configuration function.
       */
      template<class IMP>
      class ServiceInstance
        {
          std::unique_ptr<IMP> instance_;
          
        public:
          ServiceInstance()
            : instance_(new IMP{})
            {
              __assert_compatible<IMP>();
              activateServiceAccess (*instance_);
            }
          
         ~ServiceInstance()
            {
              deactivateServiceAccess();
            }
          
          ServiceInstance (ServiceInstance&&)            = default;
          ServiceInstance (ServiceInstance const&)       = delete;
          ServiceInstance& operator= (ServiceInstance&&) = delete;
          
          explicit
          operator bool()  const
            {
              return bool(instance_);
            }
          
          IMP&
          operator* ()  const
            {
              ENSURE (instance_);
              return *instance_;
            }
          
          IMP*
          operator-> ()  const
            {
              ENSURE (instance_);
              return instance_.get();
            }
        };
      
      
      /**
       * Configuration handle for temporarily shadowing a dependency by a test mock instance.
       * This noncopyable (but movable) handle shall be planted within the immediate test context.
       * It immediately stashes away the existing state and configuration from `Depend<SRV>`, but
       * waits for actual invocation of the `Depend<SRV>`-front-end to create a heap-allocated
       * instance of the `MOC` subclass, which it manages and exposes like a smart-ptr.
       * When the handle goes out of scope, the original state and configuration is restored
       */
      template<class MOC>
      class Local
        {
          std::unique_ptr<MOC> mock_;
          
          SRV* origInstance_;
          Factory origFactory_;
          
        public:
          Local()
            {
              __assert_compatible<MOC>();
              temporarilyInstallAlternateFactory (origInstance_, origFactory_
                                                 ,[this]()
                                                     {
                                                        mock_.reset(new MOC{});
                                                        return mock_.get();
                                                     });
            }
         ~Local()
            {
              restoreOriginalFactory (origInstance_, origFactory_);
            }
          
          Local (Local&&)            = default;
          Local (Local const&)       = delete;
          Local& operator= (Local&&) = delete;
          
          explicit
          operator bool()  const
            {
              return bool(mock_);
            }
          
          MOC&
          operator* ()  const
            {
              ENSURE (mock_);
              return *mock_;
            }
          
          MOC*
          operator-> ()  const
            {
              ENSURE (mock_);
              return mock_.get();
            }
        };
      
      
      
    protected: /* ======= internal access-API for those configurations to manipulate Depend<SRV> ======= */
      template<class IMP>
      friend class ServiceInstance;
      template<class MOC>
      friend class Local;
      
      
      template<class SUB>
      static void
      __assert_compatible()
        {
          static_assert (std::is_base_of<SRV,SUB>::value,
                         "Installed implementation class must be compatible to the interface.");
        }
      
      static void
      installFactory (Factory&& otherFac)
        {
          ClassLock<SRV> guard;
          if (Depend<SRV>::instance)
            throw error::Logic("Attempt to reconfigure dependency injection after the fact. "
                               "The previously installed factory (typically Singleton) was already used."
                              , error::LUMIERA_ERROR_LIFECYCLE);
          Depend<SRV>::factory = move (otherFac);
        }
      
      static void
      temporarilyInstallAlternateFactory (SRV*& stashInstance, Factory& stashFac, Factory&& newFac)
        {
          ClassLock<SRV> guard;
          stashFac = move(Depend<SRV>::factory);
          stashInstance = Depend<SRV>::instance;
          Depend<SRV>::factory = move(newFac);
          Depend<SRV>::instance = nullptr;
        }
      
      static void
      restoreOriginalFactory (SRV*& stashInstance, Factory& stashFac)
        {
          ClassLock<SRV> guard;
          Depend<SRV>::factory = move(stashFac);
          Depend<SRV>::instance = stashInstance;
        }
      
      static void
      activateServiceAccess (SRV& newInstance)
        {
          ClassLock<SRV> guard;
          if (Depend<SRV>::instance)
            throw error::Logic("Attempt to activate an external service implementation, "
                               "but another instance has already been dependency-injected."
                              , error::LUMIERA_ERROR_LIFECYCLE);
          Depend<SRV>::instance = &newInstance;
          Depend<SRV>::factory = Depend<SRV>::disabledFactory;
        }
      
      static void
      deactivateServiceAccess()
        {
          ClassLock<SRV> guard;
          Depend<SRV>::instance = nullptr;
          Depend<SRV>::factory = Depend<SRV>::disabledFactory;
        }
    };
  
  
///////////////////////////////////////////////////////Usage

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

int
main (int, char**)
  {
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
    
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
