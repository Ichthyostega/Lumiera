/*
  MaybeValue(Test)  -  considerations for dealing with optional values

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "lib/maybe.hpp"
#include "lib/util.hpp"

//#include <tr1/functional>
//#include <cstdlib>


namespace lib {
namespace test{
  
  namespace error = lumiera::error;
  
//  using util::isSameObject;
//  using std::rand;
  using util::isnil;
  using error::LUMIERA_ERROR_BOTTOM_VALUE;
  
  
  namespace { // test data and helpers...
    
    
    uint INVOCATION_CNT(0);
    
    /** helper for testing delayed evaluation */
    template<typename VAL>
    class Delayed
      {
        VAL v_;
        
      public:
        Delayed (VAL val) : v_(val) { }
        
        VAL
        operator() () const
          {
            ++INVOCATION_CNT;
            return v_;
          }
      };
    
    template<typename VAL>
    inline Delayed<VAL>
    yield (VAL val)
    {
      
    }
  }
  
  
  
  /***************************************************************************************
   * @test Investigate various situations of using a Maybe value or option monad.
   * @note this is a testbed for experiments for the time being 11/2011
   * 
   * @see lib::Maybe 
   * @see null-value-test.cpp
   * @see util::AccessCasted
   */
  class MaybeValue_test : public Test
    {
      
      void
      run (Arg) 
        {
          show_basicOperations();
          show_delayedAccess();
        }
      
      
      void
      show_basicOperations()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #856
          Maybe<int> one(1);
          Maybe<int> opt(5);
          Maybe<int> nil;
          
          CHECK (opt);    CHECK (!isnil(opt));
          CHECK (!nil);   CHECK ( isnil(nil));
          
          // access the optional value
          CHECK (1 == *one);
          CHECK (5 == *opt);
          
          // can't access an bottom value
          VERIFY_ERROR (BOTTOM_VALUE, *nil);
          
          // flatMap operation (apply a function)
          CHECK (7 == *(opt >>= inc2));
          CHECK (9 == *(opt >>= inc2 >>= inc2));
          
          // alternatives
          CHECK (1 == *(one || opt));
          CHECK (5 == *(nil || opt));
          CHECK (1 == *(nil || one || opt));
          
          CHECK (1 == one.get());
          CHECK (1 == one.getOrElse(9));
          CHECK (9 == nil.getOrElse(9));
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #856
        }
      
      
      void
      show_delayedAccess()
        {
          INVOCATION_CNT = 0;
          
          Maybe<int> nil;
          Maybe<int> two(2);
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #856
          Maybe<int(void)> later(yield(5));
          CHECK (0 == INVOCATION_CNT);
          
          CHECK (2 == *(two || later));
          CHECK (0 == INVOCATION_CNT);
          
          CHECK (5 == *(nil || later));
          CHECK (1 == INVOCATION_CNT);
          
          later.get();
          CHECK (2 == INVOCATION_CNT);
          
          CHECK (2 == two.getOrElse(later));
          CHECK (2 == INVOCATION_CNT);
          
          CHECK (5 == nil.getOrElse(later));
          CHECK (3 == INVOCATION_CNT);
          
          // obviously, this also works just with a function
          CHECK (7 == nil.getOrElse(yield(7)));
          CHECK (4 == INVOCATION_CNT);
          
          // stripping the delayed evaluation
          Maybe<int> some = later;
          CHECK (5 == INVOCATION_CNT);
          CHECK (5 == some);
          
          CHECK (5 == INVOCATION_CNT);
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #856
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (MaybeValue_test, "unit common");
  
  
}} // namespace lib::test
