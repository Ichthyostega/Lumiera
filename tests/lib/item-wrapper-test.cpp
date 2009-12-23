/*
  ItemWrapper(Test)  -  wrapping and holding arbitrary values, pointers and references
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 
* *****************************************************/



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include "lib/wrapper.hpp"

//#include <boost/lexical_cast.hpp>
#include <tr1/functional>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>



namespace lib {
namespace wrapper {
namespace test{
  
  using ::Test;
//  using boost::lexical_cast;
  using util::isSameObject;
//  using util::for_each;
//  using util::isnil;
  using std::tr1::bind;
  using std::vector;
  using std::string;
  using lib::test::randStr;
  using lib::test::showSizeof;
  using std::cout;
  using std::endl;
  
  
  
  namespace { // Test helper: yet another ctor/dtor counting class
    
    long cntTracker = 0;
  
    struct Tracker
      {
        uint i_;
        
        Tracker()                  : i_(rand() % 500) { ++cntTracker; }
        Tracker(Tracker const& ot) : i_(ot.i_)        { ++cntTracker; }
       ~Tracker()                                     { --cntTracker; }
      };
    
    bool operator== (Tracker const& t1, Tracker const& t2) { return t1.i_ == t2.i_; }
    bool operator!= (Tracker const& t1, Tracker const& t2) { return t1.i_ != t2.i_; }
    
  } // (END) Test data
  
  
  
  
  
  
  
  /*******************************************************************************
   *  @test use the ItemWrapper to define inline-storage holding values,
   *        pointers and references. Verify correct behaviour in each case,
   *        including (self)assignment, empty check, invalid dereferentiation.
   *        
   */
  class ItemWrapper_test : public Test
    {
      
      
      virtual void
      run (Arg)
        {
          ulong l1 (rand() % 1000);
          ulong l2 (rand() % 1000);
          string s1 (randStr(50));
          string s2 (randStr(50));
          const char* cp (s1.c_str());
          
          verifyWrapper<ulong> (l1, l2);
          verifyWrapper<ulong&> (l1, l2);
          verifyWrapper<ulong*> (&l1, &l2);
          verifyWrapper<ulong*> ((0), &l2);
          verifyWrapper<ulong*> (&l1, (0));
          verifyWrapper<ulong const&> (l1, l2);
          
          verifyWrapper<string> (s1, s2);
          verifyWrapper<string&> (s1, s2);
          verifyWrapper<string*> (&s1, &s2);
          
          verifyWrapper<const char*> (cp, "Lumiera");
          
          
          verifySaneInstanceHandling();
          verifyWrappedRef ();
          
          verifyFunctionResult ();
        }
      
      
      template<typename X>
      void
      verifyWrapper (X val, X otherVal)
        {
          const ItemWrapper<X> wrap(val);
          ASSERT (wrap);
          
          cout << "ItemWrapper: " << showSizeof(wrap) << endl;
          
          ItemWrapper<X> copy1 (wrap);
          ItemWrapper<X> copy2;
          ItemWrapper<X> empty;
          
          ASSERT (copy1);
          ASSERT (!copy2);
          ASSERT (false == bool(empty));
          
          ASSERT (wrap == copy1);
          ASSERT (wrap != copy2);
          ASSERT (wrap != empty);
          
          copy2 = copy1;
          ASSERT (copy2);
          ASSERT (wrap == copy2);
          ASSERT (wrap != empty);
          
          copy2 = otherVal;
          ASSERT (copy2);
          ASSERT (wrap != copy2);
          ASSERT (wrap != empty);
          
          ASSERT (val == *wrap);
          ASSERT (val == *copy1);
          ASSERT (val != *copy2);
          VERIFY_ERROR (BOTTOM_VALUE, *empty );
          
          ASSERT (otherVal == *copy2);
          copy1 = copy2;
          ASSERT (otherVal == *copy1);
          ASSERT (otherVal == *copy2);
          ASSERT (wrap != copy1);
          ASSERT (wrap != copy2);

          copy1 = empty;                   // assign empty to discard value
          copy1 = copy1;                   // self-assign empty
          ASSERT (!copy1);
          
          copy1 = copy2;
          ASSERT (otherVal == *copy1);
          copy1 = copy1;                   // self-assign (will be suppressed)
          ASSERT (otherVal == *copy1);
          copy1 = *copy1;                  // self-assign also detected in this case 
          ASSERT (otherVal == *copy2);
          
          ASSERT (copy1);
          copy1.reset();
          ASSERT (!copy1);
          ASSERT (empty == copy1);
          ASSERT (copy2 != copy1);
          VERIFY_ERROR (BOTTOM_VALUE, *copy1 );
        };
      
      
      /** @test verify that ctor and dtor calls are balanced,
       *        even when assigning and self-assigning.
       */
      void
      verifySaneInstanceHandling()
        {
          cntTracker = 0;
          {
            Tracker t1;
            Tracker t2;
            
            verifyWrapper<Tracker> (t1, t2);
            verifyWrapper<Tracker&> (t1, t2);
            verifyWrapper<Tracker*> (&t1, &t2);
              
          }
          ASSERT (0 == cntTracker);  
        }
      
      
      /** @test verify especially that we can wrap and handle
       *        a reference "value" in a pointer-like manner
       */
      void
      verifyWrappedRef ()
        {
          int x = 5;
          ItemWrapper<int&> refWrap;
          ASSERT (!refWrap);
          
          refWrap = x;
          ASSERT (refWrap);
          ASSERT (5 == *refWrap);
          ASSERT (x == *refWrap);
          
          *refWrap += 5;
          ASSERT (x == 10);
          
          ItemWrapper<int*> ptrWrap (& *refWrap);
          ASSERT ( isSameObject (**ptrWrap,  x));
          ASSERT (!isSameObject ( *ptrWrap, &x));
          **ptrWrap += 13;
          ASSERT (x == 23);
        }
      
      
      /** @test verify an extension built on top of the ItemWrapper:
       *        a function which remembers the last result */
      void
      verifyFunctionResult()
        {
          vector<int> testVec;
          for (uint i=0; i<10; ++i)
            testVec.push_back(i);
          
          FunctionResult<int&(size_t)> funRes (bind (&vector<int>::at, _1 ));
          
          // function was never invoked, thus the remembered result is NIL
          ASSERT (!funRes);
          VERIFY_ERROR (BOTTOM_VALUE, *funRes );
          
          int& r5 = funRes (5);
          ASSERT (5 == r5);
          ASSERT (isSameObject (r5, testVec[5]));
          
          int r5x = *funRes;
          ASSERT (isSameObject (r5, r5x));
          
          ASSERT ( isSameObject (r5, *funRes));
          int r7 = funRes (7);
          ASSERT (!isSameObject (r5, *funRes));
          ASSERT (!isSameObject (r7, *funRes));
          
          -- r5x;
          ++ *funRes;
          ASSERT (5+1 == testVec[5]);
          ASSERT (7+1 == testVec[7]);
          ASSERT (7+1 == r7);
        }
    };
  
  LAUNCHER (ItemWrapper_test, "unit common");
  
  
}}} // namespace lib::wrapper::test

