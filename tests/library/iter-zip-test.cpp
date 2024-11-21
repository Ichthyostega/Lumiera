/*
  IterZip(Test)  -  verify the iterator-combining iterator

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file iter-stack-test.cpp
 ** unit test \ref IterZip_test
 */



#include "lib/test/run.hpp"
#include "lib/iter-zip.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/diagnostic-output.hpp"/////////////TODO
#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include <array>


namespace lib {
namespace test{
  
//  using ::Test;
//  using util::isnil;
//  using LERR_(ITER_EXHAUST);
  using lib::meta::forEach;
  using lib::meta::mapEach;
  using std::make_tuple;
  using std::tuple;
  using std::get;
  
  namespace {// Test Fixture ...
    
    auto num5() { return NumIter{0,5}; }
    
    template<uint N, uint S=0>
    auto numS() { return explore(num5()).transform([](int i){ return i*N + S; }); }
    auto num31(){ return numS<3,1>(); }
    auto num32(){ return numS<3,2>(); }
    auto num33(){ return numS<3,3>(); }
    
    auto hexed = [](int i){ return util::showHash(i,1); };
    
    
    /** Diagnostic helper: join all the elements from the iterator */
    template<class II>
    inline string
    materialise (II&& ii)
    {
      return util::join (std::forward<II> (ii), "-");
    }
  }
  
  
  
/////////////////////////////////////////
/////////////////////////////////////////
  #define TYPE(_EXPR_) showType<decltype(_EXPR_)>()

  
  
  
  
  /*********************************************************************************//**
   * @test demonstrate construction and verify behaviour of a combined-iterator builder.
   *     - construction from arbitrary arguments by tuple-mapping a builder function
   * 
   * @see IterExplorer
   * @see IterExplorer_test
   */
  class IterZip_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          test_Fixture();
          demo_mapToTuple();
          demo_construction();
          UNIMPLEMENTED ("nebbich.");
        }
      
      
      /** @test demonstrate how the test Fixture is used */
      void
      test_Fixture()
        {
          CHECK (materialise (num5()  ) == "0-1-2-3-4"_expect);
          CHECK (materialise (num31() ) == "1-4-7-10-13"_expect);
          CHECK (materialise (num33() ) == "3-6-9-12-15"_expect);
          CHECK (materialise (num32()
                              .transform(hexed)
                             )          == "02-05-08-0B-0E"_expect);
        }
      
      
      /** @test demonstrate to apply a function to tuple contents */
      void
      demo_mapToTuple()
        {
          auto t1 = make_tuple (41u, 0.61803, '6');
          CHECK (t1 == "«tuple<uint, double, char>»──(41,0.61803,6)"_expect );
          
          auto t1f = mapEach (t1, [](auto v){ return v+1; });
          CHECK (t1f == "«tuple<uint, double, int>»──(42,1.61803,55)"_expect );    // ASCII('6') ≙ 54 promoted to int
          
          auto t1ff = mapEach (t1, [](auto& v){ v += 1; return v; });
          CHECK (t1ff == "«tuple<uint, double, char>»──(42,1.61803,7)"_expect );
          CHECK (t1f  == "«tuple<uint, double, int>»──(42,1.61803,55)"_expect );
          CHECK (t1   == "«tuple<uint, double, char>»──(42,1.61803,7)"_expect );   // src-tuple t1 affected by side-effect
          
          // tuple may hold a reference....
          tuple<char, char&> t2{get<2>(t1),get<2>(t1ff)};
          CHECK (t2 == "«tuple<char, char&>»──(7,7)"_expect );
          
          auto t2f = mapEach (t2, [](auto& v){ v -= 1; return v; });
          CHECK (t2f  == "«tuple<char, char>»──(6,6)"_expect );                    // function-result is value, thus res-tuple holds values
          CHECK (t2   == "«tuple<char, char&>»──(6,6)"_expect);                    // ...but src-tuple t2 was affected by side-effect
          CHECK (t1ff == "«tuple<uint, double, char>»──(42,1.61803,6)"_expect );   // ...which in turn holds a ref, so value in t1ff changed
          CHECK (t1   == "«tuple<uint, double, char>»──(42,1.61803,7)"_expect );   // ...while the other one was picked by value => t1 unchanged
          
          // function may return references....
          auto refr = [](auto&& v) -> decltype(auto) { return v; };
          int five = 5;
          CHECK (TYPE (refr(five)) == "int&"_expect);
          CHECK (TYPE (refr(5   )) == "int&"_expect);
          
          auto t2r = mapEach (t2, refr);
          CHECK (t2r == "«tuple<char&, char&>»──(6,6)"_expect );                   // function yields references, which are placed into res-tuple
          
          forEach (t2r, [](auto& v){ v +=23; });
          CHECK (t2r == "«tuple<char&, char&>»──(M,M)"_expect );                   // apply operation with side-effect to the last res-tuple t2r
          CHECK (t2  == "«tuple<char, char&>»──(M,M)"_expect );                    // the referred src-tuple t2 is also affected
          CHECK (t2f == "«tuple<char, char>»──(6,6)"_expect );                     // (while previously constructed t2f holds values unaffected)
          CHECK (t1  == "«tuple<uint, double, char>»──(42,1.61803,7)"_expect );    // the first elm in t2 was bound by value, so no side-effect
          CHECK (t1ff =="«tuple<uint, double, char>»──(42,1.61803,M)"_expect );    // but the second elm in t2 was bound by ref to t1ff
        }
      
      
      template<typename...ITS>
      auto
      buildIterTuple (ITS&& ...iters)
        {
          return make_tuple (lib::explore (std::forward<ITS> (iters)) ...);
        }
      
      
      /** @test demonstrate how a tuple-zipping iterator can be constructed */
      void
      demo_construction()
        {
          // let's start with the basics...
          // We can use lib::explore() to construct a suitable iterator,
          // and thus we can apply it to each var-arg and place the results into a tuple
          auto arry = std::array{3u,2u,1u};
          auto iTup = buildIterTuple (num5(), arry);

          CHECK (TYPE(iTup) == "tuple<IterExplorer<iter_explorer::BaseAdapter<NumIter<int> > >, "
                                     "IterExplorer<iter_explorer::BaseAdapter<iter_explorer::StlRange<array<uint, 3ul>&> > > >"_expect);
          
          // and we can use them as iterators...
          auto iterate_it = [](auto& it){ ++it; };
          auto access_val = [](auto& it){ return *it; };
          
          forEach (iTup, iterate_it);
          auto vTup = mapEach (iTup, access_val);
          CHECK (vTup == "«tuple<int, uint>»──(1,2)"_expect);
          
          using ITup = decltype(iTup);
          
          // Next step: define a »product iterator«
          // by mapping down each of the base operations onto the tuple elements
          struct ProductCore
            {
              ITup iters_;
              
              ProductCore(ITup&& iterTup)
                : iters_{move (iterTup)}
                { }
              
              /* === »state core« protocol API === */
              bool
              checkPoint()  const
                {
                  bool active{true};             // note: optimiser can unroll this
                  forEach (iters_, [&](auto& it){ active = active and bool(it); });
                  return active;
                }
              
              ITup&
              yield()  const
                {
                  return unConst(iters_);        // ◁─────────────── note: we expose the iterator-touple itself as »product«
                }
              
              void
              iterNext()
                {
                  forEach (iters_, [](auto& it){ ++it; });
                }
            };
          
          // ....and now we're essentially set!
          // use library building blocks to construct a tuple-iter-explorer...
          auto ii = explore (ProductCore{buildIterTuple (num5(), arry)})
                      .transform ([&](ITup& iTup){ return mapEach (iTup, access_val); })
                      ;
          
          // hold onto your hat!!!
          CHECK (TYPE(ii) == "IterExplorer<"
                                 "IterableDecorator<"
                                     "tuple<int, uint>, "                   // ◁──────────────────────────────── this is the overall result type
                                     "CheckedCore<"
                                         "iter_explorer::Transformer<"      // ◁──────────────────────────────── the top-layer is a Transformer (to access the value from each src-iter)
                                             "iter_explorer::BaseAdapter<"
                                                 "IterableDecorator<"           // ◁──────────────────────────── the product-iterator we constructed
                                                     "tuple<"                   // ◁──────────────────────────── ....exposing the iterator-tuple as „result“
                                                           "IterExplorer<"      // ◁───────────────────────────────── the first source iterator  (directly wrapping NumIter)
                                                               "iter_explorer::BaseAdapter<NumIter<int> > >, "
                                                           "IterExplorer<"      // ◁───────────────────────────────── the second source iterator (based on a STL collection)
                                                               "iter_explorer::BaseAdapter<"
                                                                   "iter_explorer::StlRange<array<uint, 3ul>&> "
                                                               "> "
                                                            "> "
                                                     ">, "
                                                     "CheckedCore<"             // ◁──────────────────────────── ....and using the given ProductCore as »state core«
                                                         "IterZip_test::demo_construction()::ProductCore> > >, "
                                             "tuple<int, uint> "            // ◁──────────────────────────────── back to top-layer: result-type of the Transformer
                                         "> "
                                     "> "
                                 "> "
                             ">"_expect);
          
          // ....
          // This is indeed a valid iterator,
          // that can be iterated for three steps
          // (limited by the shorter sequence from the array)
          // (first value from num5(), second from the array)
          CHECK (materialise (ii) == "«tuple<int, uint>»──(0,3)-"
                                     "«tuple<int, uint>»──(1,2)-"
                                     "«tuple<int, uint>»──(2,1)"_expect);
        }
/*
SHOW_EXPR          
                (materialise (
                    num32().transform(hexed)
                )
                )
*/
    };
  
  
  LAUNCHER (IterZip_test, "unit common");
  
  
}} // namespace lib::test
