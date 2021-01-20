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

#include "lib/replaceable-item.hpp"




namespace lib {
namespace wrapper {
namespace test{
  
  using ::Test;
  using lib::test::randStr;
  using lib::test::randTime;
  using util::isSameObject;
  
  using time::Time;
  using time::Duration;
  using std::string;
  using std::rand;
  using std::swap;
  
  
  
  namespace { // Test helper: yet another ctor/dtor counting class
    
    long cntTracker = 0;
    
    struct Tracker
      {
        uint i_;
        
        Tracker()                  : i_(rand() % 500) { ++cntTracker; }
        Tracker(Tracker const& ot) : i_(ot.i_)        { ++cntTracker; }
        Tracker(uint i)            : i_(i)            { ++cntTracker; }
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
   * @test scrutinise an adapter to snapshot non-assignable values.
   *       - create instantiations for various types
   *       - both assignable and non-assignable types
   *       - empty-construct and copy construct the adapter
   *       - perform assignments and even content swapping
   *       - use counting to verify proper instance management
   *       - compare by delegating to element comparison
   * 
   * @see replaceable-item.hpp
   * @see steam::control::MementoTie
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
          
          verifyUsage<ulong> (l1, l2);
          verifyUsage<ulong*> (&l1, &l2);
          
          verifyUsage<string> (s1, s2);
          verifyUsage<string*> (&s1, &s2);
          
          verifyUsage<string> (s2, cp);
          verifyUsage<string> (cp, "Lumiera");
          verifyUsage<const char*> (cp, "Lumiera");
          
          // non-assignable types...
          verifyUsage<Time>  (t1, t2);
          verifyUsage<Time>  (t1, d1);
          verifyUsage<Duration> (d1, t2);
          verifyUsage<Duration> (d1, d2);
          
          verifyNonComparableElements();
          verifyOnlyMoveConstructible();
          verifySaneInstanceHandling();
          verifyWrappedPtr ();
        }
      
      
      template<typename X, typename Y>
      void
      verifyUsage (X she, Y he)
        {
          using It = ReplaceableItem<X>;
          
          It one{she}, two{he};
          REQUIRE (one != two);
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
       *  @note Tracker uses the simple implementation for assignable values,
       *        while NonAssign uses the embedded-buffer implementation
       */
      void
      verifySaneInstanceHandling()
        {
          cntTracker = 0;
          {
            Tracker t1;
            Tracker t2;
            
            verifyUsage<Tracker> (t1, t2);
            verifyUsage<Tracker*> (&t1, &t2);
            verifyUsage<Tracker> (t1, t2.i_);
            verifyUsage<Tracker, Tracker&> (t1, t2);
            
            NonAssign u1;
            NonAssign u2;
            verifyUsage<NonAssign> (u1, u2);
            verifyUsage<NonAssign*> (&u1, &u2);
            verifyUsage<NonAssign> (u1, u2.i_);
            verifyUsage<NonAssign, NonAssign&> (u1, u2);
            verifyUsage<Tracker> (u1, u2);
          }
          CHECK (2 == cntTracker);      // surviving singleton instances
        }                              //  NullValue<Tracker> and NullValue<NonAssign>
      
      
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
      
      
      
      /** @test verify we can handle elements without comparison operator */
      void
      verifyNonComparableElements ()
        {
          struct Wrap
            {
              int i = -10 + rand() % 21;
            };
          
          ReplaceableItem<Wrap> w1 =Wrap{},
                                w2 =Wrap{};
          
          int i = w1.get().i,
              j = w2.get().i;
          
          swap (w1,w2);
          
          CHECK (i == w2.get().i);
          CHECK (j == w1.get().i);
          
          // w1 == w2;     // does not compile since comparison of Wraps is undefined
        }
      
      
      
      /** @test handle elements that allow nothing but move construction
       *  @todo conceptually, the whole point of this container is the ability
       *        to snapshot elements which allow nothing but move construction.
       *        Seemingly, GCC-4.9 does not fully implement perfect forwarding     ///////////////////////TICKET #1059 : re-visit with never compiler
       */
      void
      verifyOnlyMoveConstructible ()
        {
          struct Cagey
            {
              int i = -10 + rand() % 21;
              
              Cagey(Cagey && privy)
                : i(55)
                {
                  swap (i, privy.i);
                }
//            Cagey(Cagey const&)  =delete;                     //////////////////////////////////////////TICKET #1059 : should be deleted for this test to make any sense
              Cagey(Cagey const&)  =default;
              Cagey()              =default;
            };
          
          ReplaceableItem<Cagey> uc1 {std::move (Cagey{})},
                                 uc2 {std::move (Cagey{})};
          
          int i = uc1.get().i,
              j = uc2.get().i;
          
          swap (uc1,uc2);                                       //////////////////////////////////////////TICKET #1059
          
          CHECK (i == uc2.get().i);
          CHECK (j == uc1.get().i);
          
          ReplaceableItem<Cagey> occult {std::move (uc1)};      //////////////////////////////////////////TICKET #1059 : should use the move ctor but uses the copy ctor
          CHECK (j == occult.get().i);
//        CHECK (55 == uc1.get().i);                            //////////////////////////////////////////TICKET #1059
        }
    };
  
  LAUNCHER (ReplaceableItem_test, "unit common");
  
  
}}} // namespace lib::wrapper::test

