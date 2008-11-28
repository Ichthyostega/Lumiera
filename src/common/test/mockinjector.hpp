/*
  MOCKINJECTOR.hpp  -  replacement singleton factory for injecting Test-Mock objects
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 
*/



#ifndef LUMIERA_TEST_MOCKINJECTOR_H
#define LUMIERA_TEST_MOCKINJECTOR_H


#include "common/singletonfactory.hpp"

#include <boost/scoped_ptr.hpp>


namespace lumiera
  {
  namespace test
    {
    using boost::scoped_ptr;
    
    /**
     * Special SingletonFactory allowing to inject some instance of the Singleton
     * class, thus shaddowing "the" (default) Singleton instance temporarily.
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
      
      
      
  } // namespace test

} // namespace lumiera
#endif
