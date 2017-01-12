/*
  ReplaceableItem(Test)  -  adapter to take snapshot from non-assignable values

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "lib/util.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"////////////TODO

#include "lib/replaceable-item.hpp"

//#include <functional>
//#include <iostream>
//#include <cstdlib>
//#include <string>
//#include <vector>



namespace lib {
namespace wrapper {
namespace test{
  
  using ::Test;
  using lib::test::randStr;
  using lib::test::randTime;
  using util::isSameObject;
  
  using time::Time;
  using time::Duration;
//  using std::ref;
//  using std::vector;
  using std::string;
  using std::rand;
  
  
  
  namespace { // Test helper: yet another ctor/dtor counting class
    
    long cntTracker = 0;
    
    struct Tracker
      {
        uint i_;
        
        Tracker()                  : i_(rand() % 500) { ++cntTracker; }
        Tracker(Tracker const& ot) : i_(ot.i_)        { ++cntTracker; }
       ~Tracker()                                     { --cntTracker; }
      };
    
    struct NonAssign
      : Tracker
      {
        using Tracker::Tracker;
        NonAssign& operator= (NonAssign const&)  =delete;
      };
    
    
    bool operator== (Tracker const& t1, Tracker const& t2) { return t1.i_ == t2.i_; }
    bool operator!= (Tracker const& t1, Tracker const& t2) { return t1.i_ != t2.i_; }
    
  } // (END) Test helpers
  
  
  
  
  
  
  
  /***************************************************************************//**
   * @test use the ItemWrapper to define inline-storage holding values,
   *       pointers and references. Verify correct behaviour in each case,
   *       including (self)assignment, empty check, invalid dereferentiation.
   * 
   * @see  wrapper.hpp
   */
  class ReplaceableItem_test : public Test
    {
    
    
      virtual void
      run (Arg)
        {
          ulong l1 (rand() % 1000);
          ulong l2 (rand() % 1000);
          string s1 (randStr(50));
          string s2 (randStr(50));
          const char* cp (s1.c_str());
          
          Time     t1{randTime()}, t2{randTime()};
          Duration d1{randTime()}, d2{randTime()};
          
          verifyWrapper<ulong> (l1, l2);
          verifyWrapper<Time>  (t1, d1);
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #888
          verifyWrapper<ulong&> (l1, l2);
          verifyWrapper<ulong*> (&l1, &l2);
          verifyWrapper<ulong*> ((0), &l2);
          verifyWrapper<ulong*> (&l1, (0));
          verifyWrapper<ulong const&> (l1, l2);
          
          verifyWrapper<string> (s1, s2);
          verifyWrapper<string&> (s1, s2);
          verifyWrapper<string*> (&s1, &s2);
          
          verifyWrapper<const char*> (cp, "Lumiera");
          
          
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #888
          verifySaneInstanceHandling();
          verifyWrappedPtr ();
        }
      
      
      template<typename X, typename Y>
      void
      verifyWrapper (X she, Y he)
        {
          using It = ReplaceableItem<X>;
          
          It one{she}, two{he};
          CHECK (two == he);
          CHECK (one == she);
          CHECK (sizeof(one) == sizeof(X));
          CHECK (sizeof(two) == sizeof(X));
          
          It copy1{she};
          It copy2;
          
          CHECK (one == copy1);
          CHECK (one != copy2);
          CHECK (two != copy1);
          CHECK (two != copy2);
          
          CHECK (copy2 == NullValue<X>::get());
          
          copy2 = he;               // assign from value
          CHECK (one == copy1);
          CHECK (one != copy2);
          CHECK (two != copy1);
          CHECK (two == copy2);
          
          std::swap (copy1, copy2); // possibly move construction / move assignment
          CHECK (one != copy1);
          CHECK (one == copy2);
          CHECK (two == copy1);
          CHECK (two != copy2);
          
          copy1 = copy1;            // self assignment (is skipped)
          copy2 = one;              // assignment of an identical value
          
          CHECK (copy1 == he);
          CHECK (copy2 == she);
          CHECK (one   == she);
          CHECK (two   == he);
          
          CHECK (not isSameObject(he, static_cast<X&>(copy1)));
          
          copy1 = It{};             // copy assignment from anonymous holder
          copy1 = copy1;
          CHECK (copy1 == NullValue<X>::get());
          CHECK (copy1 != he);
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
            verifyWrapper<Tracker*> (&t1, &t2);
            verifyWrapper<Tracker, Tracker&> (t1, t2);
            
          }
          CHECK (1 == cntTracker);      // one singleton instance in NullValue<Tracker> survives 
        }
      
      
      /** @test verify especially that we can handle and re-"assign" an embedded pointer */
      void
      verifyWrappedPtr ()
        {
          int x = 5;
          ReplaceableItem<int*> ptrWrap;
          CHECK (ptrWrap.get() == NULL);
          
          ptrWrap = &x;
          CHECK (5 == *ptrWrap.get());
          CHECK (&x == ptrWrap.get());
          
          *ptrWrap.get() += 5;
          CHECK (x == 10);
          
          CHECK ( isSameObject (*ptrWrap.get(), x));
          CHECK (!isSameObject ( ptrWrap.get(), x));
        }
    };
  
  LAUNCHER (ReplaceableItem_test, "unit common");
  
  
}}} // namespace lib::wrapper::test

