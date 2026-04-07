/*
  TupleIdxAdaptor(Test)  -  demonstrate building a static dispatcher table

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file fun-trampoline-test.cpp
 ** unit test \ref TupleIdxAdaptor_test
 */



#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "lib/format-string.hpp"
#include "lib/format-util.hpp"

#include "lib/meta/tuple-idx-adaptor.hpp"


using test::showType;
using std::string;
using std::tuple;


namespace lib  {
namespace meta {
namespace test {
  
  
  
  /*************************************************************//**
   * @test show run-time index access and iteration over tuples,
   *       relying on a static function dispatcher table.
   * @see TupleIdxAdaptor_test
   */
  class TupleIdxAdaptor_test : public Test
    {
      virtual void
      run (Arg)
        {
          demonstrate_simpleAccess();
          demonstrate_referenceAccess();
          demonstrate_TupleIteration();
        }
      
      
      
      /** @test build adaptor to get run-time index access to a tuple
       */
      void
      demonstrate_simpleAccess()
        {
          auto tup42 = std::make_tuple (short(4), long(2));
          CHECK (tup42 == "«tuple<short, long>»──(4,2)"_expect );

          TupleIndexAdaptor idxAdapt{tup42};
          CHECK (idxAdapt.size() == 2);
          CHECK (idxAdapt[0] == 4);
          CHECK (idxAdapt[1] == 2);
          
          using ResType = decltype(idxAdapt[0]);
          CHECK (showType<ResType>() == "long"_expect );
        }                             // got access by a common value-type
      
      
      
      /** @test full access by reference to the data in the tuple is possible,
       *        if the data types can be reconciled to a common reference type.
       */
      void
      demonstrate_referenceAccess()
        {
          using Tup = std::tuple<int,int&,int>;
          
          using TupApt = TupleIndexAdaptor<Tup>;
          // indeed provides access by LValue reference
          CHECK (TupApt::isRefAccess);
          CHECK (showType<TupApt::ResType>() == "int&"_expect );
          
          int ii{23};
          TupApt itup{-1,ii,55};
          CHECK (itup == "iTup(-1,23,55)"_expect );
          
          // can access members by subscript...
          CHECK (itup[0] == -1);
          CHECK (itup[1] == 23);
          CHECK (itup[2] == 55);
          
          // yet can also manipulate through this accessor
          CHECK (++itup[0]   ==  0);
          CHECK (  itup[1]++ == 23);
          CHECK (++++itup[2] == 57);

          // data in the tuple fields was indeed manipulated
          CHECK (itup == "iTup(0,24,57)"_expect );
          Tup copy{itup};
          CHECK (getElm<0>(copy) ==  0);
          CHECK (getElm<1>(copy) == 24);
          CHECK (getElm<2>(copy) == 57);
          
          // and since the second field in the tuple is actually a reference,
          // the manipulation did even affect the referred external variable...
          CHECK (ii == "24"_expect );
          ++ii;
          CHECK (25 == itup[1]);
          CHECK (25 == getElm<1>(copy));
        }
      
      
      /** @test Since the TupleIndexAdaptor is subscriptable,
       *        it can be used to build a run-time iterator over the tuple.
       * @remark this demo example is somewhat weird, since the iterator is built
       *   based on the IdxStoreCore, which directly embeds the _source container_.
       *   Thus, when the iterator is copied, the source is copied as well —
       *   which violates several common assumptions regarding iterators.
       *   However, when working carefully around this irregularity,
       *   iteration and manipulation work as expected...
       */
      void
      demonstrate_TupleIteration()
        {
          using Tup = tuple<char,char,char>;
          using TupApt = TupleIndexAdaptor<Tup>;
          using TupIter = IterableDecorator<IdxStoreCore<TupApt>>;
          
          // this iterator actually embeds the source tuple
          TupIter tupIt{TupApt{'i','b','m'}};
          
          // dereferencing this iterator provides access by reference
          CHECK (showType<decltype(*tupIt)>() == "char&"_expect );
          
          // can iterate as expected
          CHECK (bool(tupIt));
          CHECK (*tupIt == "i"_expect );
          CHECK (util::join (tupIt, "·") == "i·b·m"_expect );
          
          do
            --*tupIt;    // manipulate through iterator
          while (++tupIt);
          
          CHECK (not tupIt);
          // yet the manipulated data tuple is still sitting
          // inside of the (now invalidated) iterator...
          CHECK (tupIt == "iTup(h,a,l)"_expect );
        }
    };
  
  LAUNCHER (TupleIdxAdaptor_test, "unit common");
  
  
}}} // namespace lib::meta::test
