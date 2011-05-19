/*
  MOCKINJECTOR.hpp  -  replacement singleton factory for injecting Test-Mock objects

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



#ifndef LUMIERA_TEST_MOCKINJECTOR_H
#define LUMIERA_TEST_MOCKINJECTOR_H


#include "lib/singleton-factory.hpp"
#include "lib/meta/duck-detector.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/utility/enable_if.hpp>


namespace lib {
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
      scoped_ptr<SI> mock_;
      
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
      
      /** @note MockInjector singleton factory objects can be copied,
       *        but the copy will start out with clean internal state,
       *        i.e. exhibiting normal SingletonFactory behaviour
       *        without mock object.
       */
      MockInjector (const MockInjector& other) 
        : SingletonFactory<SI>(other), mock_(0) { }
      
      MockInjector<SI>& operator= (const MockInjector<SI>& other)
        {
          return SingletonFactory<SI>::operator= (other);
        }
    };
  
  
  
  /** 
   * Details of creating and accessing mock instances,
   * especially for services implemented as Singleton.
   */
  namespace mock {
    
    using boost::enable_if;
    using lumiera::Yes_t;
    using lumiera::No_t;
    
    /** 
     * A Metafunction to find out, if a type in question
     * is used as Singleton and especially has been configured
     * to inject Mock implementations defined as Subclasses. 
     */
    template<class MOCK>
    class is_Singleton_with_MockInjector
      {
        META_DETECT_NESTED  (ServiceInterface);
        META_DETECT_FUNCTION(void, injectSubclass, (TY*) const);
        
        typedef typename MOCK::ServiceInterface ServiceInterface;
        
      public:
        typedef lib::SingletonFactory<ServiceInterface> SingletonFactory;
        
        enum{ value = HasNested_ServiceInterface<MOCK>::value
                   && HasFunSig_injectSubclass<SingletonFactory>::value
            };
      };
    
    
    /**
     * Policy-Trait: How to access and inject
     * a mock service implementation? 
     */
    template<class MOCK, class YES =void>
    struct AccessPoint;
      
    template<class MOCK>
    struct AccessPoint<MOCK, typename enable_if< is_Singleton_with_MockInjector<MOCK> >::type>
      {
        typedef typename is_Singleton_with_MockInjector<MOCK>::SingletonFactory ServiceAccessPoint;
        
        void
        activateMock()
          {
            ServiceAccessPoint().injectSubclass (new MOCK());
          }
        void
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
   */
  template<class SI>
  struct Use4Test
    : boost::noncopyable
    {
      Use4Test()
        {
          mock::AccessPoint<SI>::activateMock();
        }
      
     ~Use4Test()
        {
          mock::AccessPoint<SI>::deactivateMock();
        }
    };
  
  
  
}} // namespace lib::test
#endif
