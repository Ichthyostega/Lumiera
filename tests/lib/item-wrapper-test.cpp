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
//#include <iostream>
//#include <vector>
#include <cstdlib>
#include <string>



namespace lib {
namespace wrapper {
namespace test{
  
  using ::Test;
//  using boost::lexical_cast;
  using util::isSameObject;
//  using util::for_each;
//  using util::isnil;
//  using std::vector;
  using std::string;
  using lib::test::randStr;
//  using std::cout;
//  using std::endl;
  
  
  
  namespace { // Test data
  
    ///////////TODO
    
  } // (END) Test data
  
  
  
  
  
  
  
  /*******************************************************************************
   *  @test use the ItemWrapper to define inline-storage holding values,
   *        pointers and references. Verify correct behaviour in each case,
   *        including assignment, empty check, invalid dereferentiation.
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
//        Tracker t1;
//        Tracker t2;
          
          verifyWrapper<ulong> (l1, l2);
          verifyWrapper<ulong&> (l1, l2);
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 475  !!!!!!!!!
          verifyWrapper<ulong*> (&l1, &l2);
          verifyWrapper<ulong*> ((0), &l2);
          verifyWrapper<ulong*> (&l1, (0));
          verifyWrapper<ulong const&> (l1, l2);
          
          verifyWrapper<string> (s1, s2);
          verifyWrapper<string&> (s1, s2);
          verifyWrapper<string*> (&s1, &s2);
          
          verifyWrapper<Tracker> (t1, t2);
          verifyWrapper<Tracker&> (t1, t2);
          verifyWrapper<Tracker*> (&t1, &t2);
          
          verifyWrapper<const char*> (cp, "Lumiera");
#endif //////////////////////////////////////////////////////////////////////////////////////TODO using yet undefined facilities.....!!!!!
          
          verifyWrappedRef ();
        }
      
      
      template<typename X>
      void
      verifyWrapper (X val, X otherVal)
        {
          const ItemWrapper<X> wrap(val);
          ASSERT (wrap);
          
          ItemWrapper<X> copy1 (wrap);
          ItemWrapper<X> copy2;
          ItemWrapper<X> empty;
          
          ASSERT (copy1);
          ASSERT (!copy2);
          ASSERT (false == bool(empty));
          
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 475  !!!!!!!!!
          ASSERT (wrap == copy1);
#endif //////////////////////////////////////////////////////////////////////////////////////TODO using yet undefined facilities.....!!!!!
          ASSERT (wrap != copy2);
          ASSERT (wrap != empty);
          
          copy2 = copy1;
          ASSERT (copy2);
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 475  !!!!!!!!!
          ASSERT (wrap == copy2);
#endif //////////////////////////////////////////////////////////////////////////////////////TODO using yet undefined facilities.....!!!!!
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
          
          copy1.reset();
          ASSERT (!copy1);
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 475  !!!!!!!!!
          ASSERT (empty == copy1);
#endif //////////////////////////////////////////////////////////////////////////////////////TODO using yet undefined facilities.....!!!!!
          ASSERT (copy2 != copy1);
          VERIFY_ERROR (BOTTOM_VALUE, *copy1 );
        };
      
      
      void
      verifyWrappedRef ()
        {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 475  !!!!!!!!!
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
          ASSERT (isSameObject (*ptrWrap, x));
          **ptrWrap += 13;
          ASSERT (x == 23);
#endif //////////////////////////////////////////////////////////////////////////////////////TODO using yet undefined facilities.....!!!!!
        }
      
      
      
    };
  
  LAUNCHER (ItemWrapper_test, "unit common");
  
  
}}} // namespace lib::wrapper::test

