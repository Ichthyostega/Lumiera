/*
  ItemWrapper(Test)  -  wrapping and holding arbitrary values, pointers and references

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file item-wrapper-test.cpp
 ** unit test \ref ItemWrapper_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-cout.hpp"
#include "lib/random.hpp"
#include "lib/util.hpp"

#include "lib/wrapper.hpp"
#include "lib/wrapper-function-result.hpp"

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>



namespace lib {
namespace wrapper {
namespace test{
  
  using ::Test;
  using lib::test::randStr;
  using lib::test::showSizeof;
  using util::isSameObject;
  
  using std::placeholders::_1;
  using std::ref;
  using std::shared_ptr;
  using std::vector;
  using std::string;
  
  
  
  namespace { // Test helper: yet another ctor/dtor counting class
    
    long cntTracker = 0;
    
    struct Tracker
      {
        uint i_;
        
        Tracker()                  : i_(rani(500)) { ++cntTracker; }
        Tracker(Tracker const& ot) : i_(ot.i_)     { ++cntTracker; }
       ~Tracker()                                  { --cntTracker; }
      };
    
    bool operator== (Tracker const& t1, Tracker const& t2) { return t1.i_ == t2.i_; }
    bool operator!= (Tracker const& t1, Tracker const& t2) { return t1.i_ != t2.i_; }
    
    
    /// to be bound as test function....
    int&
    pickElement (vector<int>& vec, size_t idx)
    {
      return vec[idx];
    }
    
    function<int&(size_t)>
    pickElement_ofVector (vector<int>& vec)
    {
      return std::bind (pickElement, ref(vec), _1 );
    }
  } // (END) Test helpers
  
  
  
  
  
  
  
  /***************************************************************************//**
   * @test use the ItemWrapper to define inline-storage holding values,
   *       pointers and references. Verify correct behaviour in each case,
   *       including (self)assignment, empty check, invalid dereferentiation.
   * 
   * @see  wrapper.hpp
   */
  class ItemWrapper_test : public Test
    {
    
    
      virtual void
      run (Arg)
        {
          seedRand();
          
          ulong l1 (rani (1000));
          ulong l2 (rani (1000));
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
          verifySaneMoveHandling();
          verifyWrappedRef ();
          
          verifyFunctionResult ();
          verifyFunctionRefResult ();
        }
      
      
      template<typename X>
      void
      verifyWrapper (X val, X otherVal)
        {
          const ItemWrapper<X> wrap(val);
          CHECK (wrap);
          
          cout << "ItemWrapper: " << showSizeof(wrap) << endl;
          
          ItemWrapper<X> copy1 (wrap);
          ItemWrapper<X> copy2;
          ItemWrapper<X> empty;
          
          CHECK (copy1);
          CHECK (!copy2);
          CHECK (false == bool(empty));
          
          CHECK (wrap == copy1);
          CHECK (wrap != copy2);
          CHECK (wrap != empty);
          
          copy2 = copy1;
          CHECK (copy2);
          CHECK (wrap == copy2);
          CHECK (wrap != empty);
          
          copy2 = otherVal;
          CHECK (copy2);
          CHECK (wrap != copy2);
          CHECK (wrap != empty);
          
          CHECK (val == *wrap);
          CHECK (val == *copy1);
          CHECK (val != *copy2);
          VERIFY_ERROR (BOTTOM_VALUE, *empty );
          
          CHECK (otherVal == *copy2);
          copy1 = copy2;
          CHECK (otherVal == *copy1);
          CHECK (otherVal == *copy2);
          CHECK (wrap != copy1);
          CHECK (wrap != copy2);
          
          copy1 = empty;                   // assign empty to discard value
          copy1 = copy1;                   // self-assign empty value
          CHECK (!copy1);
          
          copy1 = copy2;
          CHECK (otherVal == *copy1);
          copy1 = copy1;                   // self-assign (will be suppressed)
          CHECK (otherVal == *copy1);
          copy1 = *copy1;                  // self-assign also detected in this case
          CHECK (otherVal == *copy2);
          
          CHECK (copy1);
          copy1.reset();
          CHECK (!copy1);
          CHECK (empty == copy1);
          CHECK (copy2 != copy1);
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
          CHECK (0 == cntTracker);
        }
      
      
      /** @test proper handling of move and rvalue references */
      void
      verifySaneMoveHandling()
        {
          using Data = shared_ptr<int>;
          using Wrap = ItemWrapper<Data>;
          
          Data data{new int(12345)};
          CHECK (1 == data.use_count());
          
          Wrap wrap{data};
          CHECK (2 == data.use_count());
          CHECK (12345 == **wrap);
          CHECK (isSameObject (*data, **wrap));
          CHECK (!isSameObject (data, *wrap));
          
          Wrap wcopy{wrap};
          CHECK (3 == data.use_count());
          
          Wrap wmove{move (wcopy)};
          CHECK (3 == data.use_count());
          CHECK (not wcopy);
          CHECK (wmove);
          
          wcopy = move(wmove);
          CHECK (3 == data.use_count());
          CHECK (not wmove);
          CHECK (wcopy);
          
          Wrap wmove2{move (data)};
          CHECK (0 == data.use_count());
          CHECK (3 == wmove2->use_count());
          CHECK (not data);
          CHECK (wmove2);
          CHECK (wrap);
          
          wmove2 = move (wcopy);
          CHECK (2 == wmove2->use_count());
          CHECK (not wcopy);
          CHECK (wmove2);
          CHECK (wrap);
          
          wmove2 = move (wrap);
          CHECK (1 == wmove2->use_count());
          CHECK (not wrap);
          CHECK (wmove2);
          
          wmove2 = move (wmove);
          CHECK (not wcopy);
          CHECK (not wmove);
          CHECK (not wmove2);
        }
      
      
      /** @test verify especially that we can wrap and handle
       *        a reference "value" in a pointer-like manner
       */
      void
      verifyWrappedRef ()
        {
          int x = 5;
          ItemWrapper<int&> refWrap;
          CHECK (!refWrap);
          
          refWrap = x;
          CHECK (refWrap);
          CHECK (5 == *refWrap);
          CHECK (x == *refWrap);
          
          *refWrap += 5;
          CHECK (x == 10);
          
          ItemWrapper<int*> ptrWrap (& *refWrap);
          CHECK ( isSameObject (**ptrWrap,  x));
          CHECK (!isSameObject ( *ptrWrap, &x));
          **ptrWrap += 13;
          CHECK (x == 23);
        }
      
      
      static auto produceResult() { return rani(); }
      
      /** @test verify an extension built on top of the ItemWrapper:
       *        a function which remembers the last result. As a simple test,
       *        we bind a static helper function to produce a random value
       *        and remember the result returned last.
       */
      void
      verifyFunctionResult()
        {
          FunctionResult<int(void)> randomVal (produceResult);
          
          // function was never invoked, thus the remembered result is NIL
          CHECK (!randomVal);
          VERIFY_ERROR (BOTTOM_VALUE, *randomVal );
          
          int v1 = randomVal();
          CHECK (v1 == *randomVal);
          CHECK (v1 == *randomVal);
          CHECK (v1 == *randomVal);
          CHECK (randomVal);
          
          int v2;
          do v2 = randomVal();
          while (v1 == v2);
          CHECK (v2 == *randomVal);
          CHECK (v2 == *randomVal);
          CHECK (v1 != *randomVal);
        }
      
      
      /** @test verify an extension built on top of the ItemWrapper:
       *        a function which remembers the last result. Here we use
       *        a test function, which picks a member of an vector and
       *        returns a \em reference to it. Thus the cached "result"
       *        can be used to access and change the values within the
       *        original vector. In a real world usage scenario, such a
       *        function could be an (expensive) data structure access.
       */
      void
      verifyFunctionRefResult()
        {
          vector<int> testVec;
          for (uint i=0; i<10; ++i)
            testVec.push_back(i);
          
          FunctionResult<int&(size_t)> funRes (pickElement_ofVector(testVec));
          
          // function was never invoked, thus the remembered result is NIL
          CHECK (!funRes);
          VERIFY_ERROR (BOTTOM_VALUE, *funRes );
          
          int& r5 = funRes (5);
          CHECK (funRes);  // indicates existence of cached result
          
          CHECK (5 == r5);
          CHECK (isSameObject (r5, testVec[5]));
          
          int& r5x = *funRes;
          CHECK (isSameObject (r5, r5x));
          
          CHECK ( isSameObject (r5, *funRes));
          int& r7 = funRes (7);
          CHECK (!isSameObject (r5, *funRes));
          CHECK ( isSameObject (r7, *funRes));
          
          -- r5x;
          ++ *funRes;
          CHECK (5-1 == testVec[5]);
          CHECK (7+1 == testVec[7]);
          CHECK (7+1 == r7);
        }
    };
  
  LAUNCHER (ItemWrapper_test, "unit common");
  
  
}}} // namespace lib::wrapper::test

