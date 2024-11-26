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
#include "lib/iter-explorer.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/diagnostic-output.hpp"/////////////TODO
#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include <array>
#include <vector>


namespace lib {
namespace test{
  
  using util::join;
  using util::isnil;
  using util::noneg;
  using LERR_(ITER_EXHAUST);
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
  
  #define TYPE(_EXPR_) showType<decltype(_EXPR_)>()

  
  
  
  /*********************************************************************************//**
   * @test demonstrate construction and verify behaviour of a combined-iterator builder.
   *     - construction from arbitrary arguments by tuple-mapping a builder function
   *     - defining the operation on the product type by lifting individual operations
   *     - use the library building blocks to construct a zip-iter-builder
   *     - iterate a mix of source iterators and containers
   *     - apply additional processing logic by pipelining
   * @see IterExplorer
   * @see IterExplorer_test
   */
  class IterZip_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          test_Fixture();
          demo_mapToTuple();
          demo_construction();
          
          verify_iteration();
          verify_references();
          verify_pipelining();
          verify_exploration();
        }
      
      
      /** @test demonstrate combined iteration */
      void
      simpleUsage()
        {
          auto a = std::array{1u,2u,3u};
          auto v = std::vector{{2l,3l}};
          
          // loop over both in lockstep
          for (auto [u,l] : zip(a,v))
            CHECK (u + 1 == l);
          
          // iterate-with index
          auto it = izip(v);
          CHECK (it);
          CHECK (*it == "«tuple<ulong, long&>»──(0,2)"_expect );
          ++it;
          CHECK (*it == "«tuple<ulong, long&>»──(1,3)"_expect );
          CHECK (it);
          ++it;
          CHECK (not it);
          VERIFY_ERROR (ITER_EXHAUST, *it  );
          VERIFY_ERROR (ITER_EXHAUST, ++it );
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
          tuple<char, char&> t2{get<2>(t1), get<2>(t1ff)};
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
          CHECK (t2r ==  "«tuple<char&, char&>»──(M,M)"_expect );                  // apply operation with side-effect to the last res-tuple t2r
          CHECK (t2  ==  "«tuple<char, char&>»──(M,M)"_expect );                   // the referred src-tuple t2 is also affected
          CHECK (t2f ==  "«tuple<char, char>»──(6,6)"_expect );                    // (while previously constructed t2f holds values unaffected)
          CHECK (t1  ==  "«tuple<uint, double, char>»──(42,1.61803,7)"_expect );   // the first elm in t2 was bound by value, so no side-effect
          CHECK (t1ff == "«tuple<uint, double, char>»──(42,1.61803,M)"_expect );   // but the second elm in t2 was bound by ref to t1ff
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
                  return unConst(iters_);        // ◁─────────────── note: we expose the iterator-tuple itself as »product«
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
          
          // demonstrate the composed pipeline type...
          CHECK (TYPE(ii) == "IterExplorer<"
                                 "IterableDecorator<"
                                     "CheckedCore<"
                                         "iter_explorer::Transformer<"      // ◁──────────────────────────────── the top-layer is a Transformer (to access the value from each src-iter)
                                             "iter_explorer::BaseAdapter<"
                                                 "IterableDecorator<"           // ◁──────────────────────────── the product-iterator we constructed
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
      
      
      
      
      /** @test create various product (tuple) iterators
       *   from mixed source iterators and verify basic iteration.
       */
      void
      verify_iteration()
        {
          CHECK (materialise (
                    zip (num31(), num32(), num33())
                )
                == "«tuple<uint&, uint&, uint&>»──(1,2,3)-"
                   "«tuple<uint&, uint&, uint&>»──(4,5,6)-"
                   "«tuple<uint&, uint&, uint&>»──(7,8,9)-"
                   "«tuple<uint&, uint&, uint&>»──(10,11,12)-"
                   "«tuple<uint&, uint&, uint&>»──(13,14,15)"_expect);
          
          CHECK (materialise(
                    izip (num31(), num32(), num33())
                )
                == "«tuple<ulong, uint&, uint&, uint&>»──(0,1,2,3)-"
                   "«tuple<ulong, uint&, uint&, uint&>»──(1,4,5,6)-"
                   "«tuple<ulong, uint&, uint&, uint&>»──(2,7,8,9)-"
                   "«tuple<ulong, uint&, uint&, uint&>»──(3,10,11,12)-"
                   "«tuple<ulong, uint&, uint&, uint&>»──(4,13,14,15)"_expect);
          
          
          auto s6 = std::array{1,1,2,3,5,8};
          auto s3 = {3,2,1};
          auto s0 = eachNum(5u,5u);
          CHECK (TYPE(s6) == "array<int, 6ul>"_expect );
          CHECK (TYPE(s3) == "initializer_list<int>"_expect );
          CHECK (TYPE(s0) == "NumIter<uint>"_expect );
          
          CHECK (materialise (
                    zip (s6,s6,s6,eachNum('a'))
                )
                == "«tuple<int&, int&, int&, char>»──(1,1,1,a)-"
                   "«tuple<int&, int&, int&, char>»──(1,1,1,b)-"
                   "«tuple<int&, int&, int&, char>»──(2,2,2,c)-"
                   "«tuple<int&, int&, int&, char>»──(3,3,3,d)-"
                   "«tuple<int&, int&, int&, char>»──(5,5,5,e)-"
                   "«tuple<int&, int&, int&, char>»──(8,8,8,f)"_expect);
          
          CHECK (materialise (
                    zip (s6,s3,s6,eachNum('a'))
                )
                == "«tuple<int&, int const&, int&, char>»──(1,3,1,a)-"
                   "«tuple<int&, int const&, int&, char>»──(1,2,1,b)-"
                   "«tuple<int&, int const&, int&, char>»──(2,1,2,c)"_expect);
          
          CHECK (isnil (s0));
          CHECK (materialise (
                    zip (s0,s3,s6,eachNum('a'))
                )
                == ""_expect);
          
          CHECK (materialise (
                    zip (eachNum('a'),eachNum(-1),s0,s0)
                )
                == ""_expect);
          
          CHECK (materialise (
                    zip (eachNum('a'),eachNum(-1),s3,s0)
                )
                == ""_expect);
          
          CHECK (materialise (
                    zip (eachNum('a'),eachNum(-1),s3,s3)
                )
                == "«tuple<char, int, int const&, int const&>»──(a,-1,3,3)-"
                   "«tuple<char, int, int const&, int const&>»──(b,0,2,2)-"
                   "«tuple<char, int, int const&, int const&>»──(c,1,1,1)"_expect);
          
          // a wild mix of data sources,
          // including infinite and virtual ones....
          CHECK (materialise (
                    izip (s6                                                   // a STL container given by reference
                         ,explore(s6).filter([](int i){ return i%2; })         // IterExplorer pipeline with filtering
                         ,numS<17,170>().transform(hexed)                      // IterExplorer pipeline with transformer and object value result
                         ,eachNum((1+sqrt(5))/2)                               // a Lumiera iterator which happens to be almost inexhaustible
                         ,explore(s3).asIterSource()                           // an IterSource, which is a virtual (OO) iterator interface
                         )
                )
                == "«tuple<ulong, int&, int&, string&, double, int const&>»──(0,1,1,AA,1.618034,3)-"
                   "«tuple<ulong, int&, int&, string&, double, int const&>»──(1,1,1,BB,2.618034,2)-"
                   "«tuple<ulong, int&, int&, string&, double, int const&>»──(2,2,3,CC,3.618034,1)"_expect);
        }
      
      
      
      /** @test verify pass-through of references */
      void
      verify_references()
        {
          auto vec = std::vector{1,5};
          auto arr = std::array{2,3};
          
          // Case-1 ------
          auto i1 = izip (vec,arr);
          
          CHECK (*i1 == "«tuple<ulong, int&, int&>»──(0,1,2)"_expect );        // initial state points to the first elements, prefixed with index≡0
          get<1>(*i1) = 5;                                                     // manipulate through the exposed reference
          CHECK (*i1 == "«tuple<ulong, int&, int&>»──(0,5,2)"_expect );        // effect of manipulation is visible

          CHECK (join(vec) == "5, 5"_expect );                                 // manipulation indeed flipped the first element in the vector
          CHECK (join(arr) == "2, 3"_expect );                                 // (while the array remains unaffected)
          
          // Case-2 ------
          auto i2 = izip (explore(vec).transform([](uint v){ return v-1; })    // this time the first iterator is a pipeline with a transformer
                         ,arr);                                                // while the second one is again a direct iteration of the array
          
          CHECK (*i2 == "«tuple<ulong, uint&, int&>»──(0,4,2)"_expect );       // again can see the first elements, and the effect of the transformer
          get<0>(*i2) = 9;                                                     // manipulate complete result tuple
          get<1>(*i2) = 9;
          get<2>(*i2) = 9;
          CHECK (*i2 == "«tuple<ulong, uint&, int&>»──(9,9,9)"_expect );       // effect of the manipulation is visible
          
          ++i2;                                                                // ...but iteration re-uses the internal result-tuple storage
          CHECK (*i2 == "«tuple<ulong, uint&, int&>»──(1,4,3)"_expect );       // and so the effect of the manipulation seems gone
          CHECK (join(vec) == "5, 5"_expect );                                 // ...which is in fact true for the vector, due to the transformer
          CHECK (join(arr) == "9, 3"_expect );                                 // ...while the array could be reached through the reference
        }
      
      
      
      
      /** @test the result is actually an IterExplorer pipeline builder,
       *        which can be used to attach further processing downstream.
       *  @note the design of IterExplorer inherently requires that
       *        generic lambdas accept the _iterator type_ by reference;
       *        structural bindings can only be used in a second step.
       */
      void
      verify_pipelining()
        {
          // for reference: this is the base data.......
          CHECK (materialise (
                    zip (num31(), num32(), num33())
                )
                == "«tuple<uint&, uint&, uint&>»──(1,2,3)-"
                   "«tuple<uint&, uint&, uint&>»──(4,5,6)-"
                   "«tuple<uint&, uint&, uint&>»──(7,8,9)-"
                   "«tuple<uint&, uint&, uint&>»──(10,11,12)-"
                   "«tuple<uint&, uint&, uint&>»──(13,14,15)"_expect);
          
          // transform the tuple into another data value
          CHECK (materialise (
                    zip (num31(), num32(), num33())
                      . transform([](auto& it){ auto [a,b,c] = *it;
                                                return a+b+c;
                                              })
                )
                == "6-15-24-33-42"_expect);
          
          // filter tuples based on inspecting contents
          CHECK (materialise (
                    zip (num31(), num32(), num33())
                      . filter   ([](auto& it){ auto [a,b,c] = *it;
                                                return not ((a+b+c) % 2);
                                              })
                )
                == "«tuple<uint&, uint&, uint&>»──(1,2,3)-"
                   "«tuple<uint&, uint&, uint&>»──(7,8,9)-"
                   "«tuple<uint&, uint&, uint&>»──(13,14,15)"_expect);
          
          // reduce with accessor and std::plus
          CHECK (zip (num31(), num32(), num33())
                      . reduce   ([](auto& it){ auto [a,b,c] = *it;
                                                return a+b+c;
                                              })
                == 6+15+24+33+42);
        }
      
      
      
      /** @test verify the interplay of _child expansion_ and tuple-zipping.
       * @remark the expansion mechanism implies that a _child sequence_ is generated
       *         by an _expand functor,_ based on the current iterator value at that point.
       *         The tricky part here is that this expand functor can sit somewhere in the
       *         source iterators, while the actual signal to expand is sent from »downstream«
       *         and has to be propagated to all children.
       *         Thus two expander-setups are demonstrated first, and then triggered from
       *         a combined iterator, dispatching the trigger over the tuple-zipping step.
       *   - the expansion-sequences unfold the same in each case
       *   - the shortest sequence terminates the overall zip()-evaluation
       *   - when generating the `expandChildrem()` call _after_ the `zip()`,
       *     it is also passed to other iterators that have no expand-functor defined;
       *     for those, it is absorbed without effect. Now, since the expandAll()
       *     actually works by replacing the iterate() by expandChildern(), this means
       *     that the _other sequences_ just do not make any progress.
       */
      void
      verify_exploration()
        {
          CHECK (materialise (
                    num31()
                )
                == "1-4-7-10-13"_expect);

          CHECK (materialise (
                    explore(num31())
                      .expand ([](int i){ return NumIter{noneg(i-1),i}; })
                      .expandAll()
                )
                == "1-0-4-3-2-1-0-7-6-5-4-3-2-1-0-10-9-8-7-6-5-4-3-2-1-0-13-12-11-10-9-8-7-6-5-4-3-2-1-0"_expect);

          CHECK (materialise (
                    explore(num31())
                      .expand ([](int i){ return NumIter{noneg(i-2),i-1}; })
                      .expandAll()
                )
                == "1-4-2-0-7-5-3-1-10-8-6-4-2-0-13-11-9-7-5-3-1"_expect);

          CHECK (materialise (
                  zip
                    ( eachNum(10)
                    , explore(num31())
                        .expand ([](int i){ return NumIter{noneg(i-1),i}; })
                        .expandAll() // ◁────────────────────────────────────────────── expand triggered in source pipeline, before the zip()
                    , explore(num31())
                        .expand ([](int i){ return NumIter{noneg(i-2),i-1}; })
                        .expandAll()
                    )
                )
                == "«tuple<int, uint, uint>»──(10,1,1)-"
                   "«tuple<int, uint, uint>»──(11,0,4)-"
                   "«tuple<int, uint, uint>»──(12,4,2)-"
                   "«tuple<int, uint, uint>»──(13,3,0)-"
                   "«tuple<int, uint, uint>»──(14,2,7)-"
                   "«tuple<int, uint, uint>»──(15,1,5)-"
                   "«tuple<int, uint, uint>»──(16,0,3)-"
                   "«tuple<int, uint, uint>»──(17,7,1)-"
                   "«tuple<int, uint, uint>»──(18,6,10)-"
                   "«tuple<int, uint, uint>»──(19,5,8)-"
                   "«tuple<int, uint, uint>»──(20,4,6)-"
                   "«tuple<int, uint, uint>»──(21,3,4)-"
                   "«tuple<int, uint, uint>»──(22,2,2)-"
                   "«tuple<int, uint, uint>»──(23,1,0)-"
                   "«tuple<int, uint, uint>»──(24,0,13)-"
                   "«tuple<int, uint, uint>»──(25,10,11)-"
                   "«tuple<int, uint, uint>»──(26,9,9)-"
                   "«tuple<int, uint, uint>»──(27,8,7)-"
                   "«tuple<int, uint, uint>»──(28,7,5)-"
                   "«tuple<int, uint, uint>»──(29,6,3)-"
                   "«tuple<int, uint, uint>»──(30,5,1)"_expect);

          CHECK (materialise (
                  zip
                    ( eachNum(10)
                    , explore(num31())
                        .expand ([](int i){ return NumIter{noneg(i-1),i}; })
                    , explore(num31())
                        .expand ([](int i){ return NumIter{noneg(i-2),i-1}; })
                    )
                    .expandAll()  // ◁──────────┲━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ note the difference: expand triggered after the zip()
                )                //             ▽
                == "«tuple<int, uint, uint>»──(10,1,1)-"
                   "«tuple<int, uint, uint>»──(10,0,4)-"
                   "«tuple<int, uint, uint>»──(10,4,2)-"
                   "«tuple<int, uint, uint>»──(10,3,0)-"
                   "«tuple<int, uint, uint>»──(10,2,7)-"
                   "«tuple<int, uint, uint>»──(10,1,5)-"
                   "«tuple<int, uint, uint>»──(10,0,3)-"
                   "«tuple<int, uint, uint>»──(10,7,1)-"
                   "«tuple<int, uint, uint>»──(10,6,10)-"
                   "«tuple<int, uint, uint>»──(10,5,8)-"
                   "«tuple<int, uint, uint>»──(10,4,6)-"
                   "«tuple<int, uint, uint>»──(10,3,4)-"
                   "«tuple<int, uint, uint>»──(10,2,2)-"
                   "«tuple<int, uint, uint>»──(10,1,0)-"
                   "«tuple<int, uint, uint>»──(10,0,13)-"
                   "«tuple<int, uint, uint>»──(10,10,11)-"
                   "«tuple<int, uint, uint>»──(10,9,9)-"
                   "«tuple<int, uint, uint>»──(10,8,7)-"
                   "«tuple<int, uint, uint>»──(10,7,5)-"
                   "«tuple<int, uint, uint>»──(10,6,3)-"
                   "«tuple<int, uint, uint>»──(10,5,1)"_expect);
        }
    };
  
  
  LAUNCHER (IterZip_test, "unit common");
  
  
}} // namespace lib::test
