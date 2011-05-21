/*
  MOCK-INJECTOR.hpp  -  replacement singleton factory for injecting Test-Mock objects

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

*/

/** @file mock-injector.hpp
 ** Support for unit-testing with a mock implementation of some singleton service.
 ** Using a Singleton to access a global service (instead of using a dependency injection
 ** framework) has the downside of making unit-tests hard to write. When the code to be
 ** tested accesses the global service, usually, several further services must be available.
 ** Usually this (and the lack of flexibility) counts as the main arguments against using
 ** Singletons. Yet still our current analysis (as of 2011) for Lumiera is that this
 ** drawback doesn't justify all the overhead of writing our own dependency-injection
 ** container (given the lack of pre-existing suitable C++ implementations).
 ** 
 ** lib::test::MockInjector provides a mechanism to overcome this problem. Given
 ** a Singleton service, it allows to install a subclass of the service interface
 ** temporarily as a mock implementation. As an additional convenience shortcut,
 ** the Use4Test template can be used as a scoped variable to automate this process
 ** of installing and removing the mock service within the call scope of a single test
 ** method or test object.
 ** 
 ** \par prerequisites
 ** The Singleton needs to be managed and accessed through Lumiera's lib::Singleton
 ** factory. Moreover, an explicit template specialisation to use MockInjector instead
 ** of the plain lib::SingletonFactory needs to be given globally, for the service interface
 ** to use. For this template specialisation, a forward declaration of the service is
 ** sufficient. See singleton-preconfigure.hpp for examples.
 ** 
 ** The Mock implementation of the service needs to be default constructible and
 ** assignment compatible to the service interface. Moreover, the mock implementation class
 ** must expose a typedef \c ServiceInterface pointing to this interface. The instance of
 ** the mock service implementation will be created in heap memory and managed by a static
 ** smart-ptr variable; it will be destroyed when leaving the scope marked by Use4Test.
 ** 
 ** @see SingletonFactory
 ** @see SingletonTestMock_test
 ** @see MediaAccessMock_test usage example
 */


#ifndef LUMIERA_TEST_MOCK_INJECTOR_H
#define LUMIERA_TEST_MOCK_INJECTOR_H


#include "lib/singleton-factory.hpp"
#include "lib/meta/duck-detector.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>
#include <boost/utility/enable_if.hpp>


namespace lib {
  template <class SI>
  class Singleton;        // forward decl; see singleton-preconfigure
  
  
namespace test{
  
  using boost::scoped_ptr;
  
  /**
   * Special SingletonFactory allowing to inject some instance of the Singleton
   * class, thus shadowing "the" (default) Singleton instance temporarily.
   * This allows installing a Mock Subclass of the Singleton for running tests,
   * while the Singleton can be used as usual in production code.
   * @note we use the default policies or SingletonFactory 
   */
  template<class SI>
  class MockInjector : public SingletonFactory<SI>
    {
      static scoped_ptr<SI> mock_;
      
    public:
      /** Overwriting the normal Singleton creation Interface
       *  to return some mock if defined, falling back to the
       *  default Singleton creation behaviour else.
       */
      SI& operator() ()
        {
          if (mock_)
            return *mock_;
          else
            return SingletonFactory<SI>::operator() ();
        }
      
      void injectSubclass (SI* mockobj)
        {
          TRACE_IF (mockobj,  test, "Singleton: installing Mock object");
          TRACE_IF (!mockobj, test, "Singleton: removing Mock object");
          mock_.reset (mockobj);
        }
      
      
      MockInjector () {};
      
      // MockInjector objects may be copied freely (no object fields) 
    };
  
  /** embedded static var to hold onto a configured mock implementation */
  template<class SI>
  scoped_ptr<SI> MockInjector<SI>::mock_;
  
  
  
  
  
  
  /** Details of creating and accessing mock instances, *
   *  especially for services implemented as Singleton. */
  namespace mock {
    
    using boost::enable_if;
    using lumiera::Yes_t;
    using lumiera::No_t;
    
    /**
     * Metafunction: does the Type in question
     * give us a clue about what service interface to use?
     */
    template<class MOCK>
    class defines_ServiceInterface
      {
        META_DETECT_NESTED  (ServiceInterface);
        
      public:
        enum{ value = HasNested_ServiceInterface<MOCK>::value
            };
      };
    
    /**
     * Trait template: getting the types of the
     * Service interface and SingletonFactory
     */
    template<class IMP, class YES =void>
    struct SingletonAccess
      {
        typedef No_t ServiceInterface;
        typedef No_t Factory;
      };
    
    template<class IMP>
    struct SingletonAccess<IMP, typename enable_if< defines_ServiceInterface<IMP> >::type>
      {
        typedef typename IMP::ServiceInterface   ServiceInterface;
        typedef lib::Singleton<ServiceInterface> Factory;
      };
    
    
    /** 
     * A Metafunction to find out, if a type in question
     * is used as Singleton and especially has been configured
     * to inject Mock implementations defined as Subclasses. 
     */
    template<class MOCK>
    class is_Singleton_with_MockInjector
      {
        typedef typename SingletonAccess<MOCK>::ServiceInterface Interface;
        typedef typename SingletonAccess<MOCK>::Factory SingletonFactory;
        
        META_DETECT_MEMBER (injectSubclass);
      public:
        
        enum{ value = defines_ServiceInterface<MOCK>::value
                   && HasMember_injectSubclass<SingletonFactory>::value
            };
      };
    
    
    /**
     * Policy-Trait: How to access and inject
     * a mock service implementation? 
     */
    template<class MOCK, class YES =void>
    struct AccessPoint
      {
        BOOST_STATIC_ASSERT (is_Singleton_with_MockInjector<MOCK>::value );
      };                    // Hint: (1) the mock needs a typedef pointing to the ServiceInterface
                            //       (2) prepare the Singleton-Factory for injecting a Mock implementation...
                            //           look into singleton-preconfigure.hpp for examples!
    
    template<class MOCK>
    struct AccessPoint<MOCK, typename enable_if< is_Singleton_with_MockInjector<MOCK> >::type>
      {
        typedef typename SingletonAccess<MOCK>::Factory ServiceAccessPoint;
        
        static void
        activateMock()
          {
            ServiceAccessPoint().injectSubclass (new MOCK());
          }
        static void
        deactivateMock()
          {
            ServiceAccessPoint().injectSubclass (0);
          }
      };
  }//(End) details of injecting the mock implementation
  
  
  
  
  /**
   * Scoped object for installing/deinstalling a mocked service automatically.
   * Placing a suitably specialised instance of this template into a local scope
   * will inject the corresponding mock installation and remove it when the
   * control flow leaves this scope.
   * @param MOCK the concrete mock implementation to inject. It needs to 
   *        subclass the service interface and expose a typedef \c ServiceInterace 
   */
  template<class MOCK>
  struct Use4Test
    : boost::noncopyable
    {
      Use4Test()
        {
          mock::AccessPoint<MOCK>::activateMock();
        }
      
     ~Use4Test()
        {
          mock::AccessPoint<MOCK>::deactivateMock();
        }
    };
  
  
  
}} // namespace lib::test
#endif
