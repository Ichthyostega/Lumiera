/*
  LazyInit(Test)  -  verify a mechanism to install a self-initialising functor

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file lazy-init-test.cpp
 ** unit test \ref LazyInit_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/lazy-init.hpp"
#include "lib/meta/util.hpp"
#include "lib/util.hpp"

#include <memory>



namespace lib {
namespace test{
  
  using util::isSameObject;
  using lib::meta::isFunMember;
  using lib::meta::disable_if_self;
  using err::LUMIERA_ERROR_LIFECYCLE;
  using std::make_unique;
  
  
  
  
  /***********************************************************************************//**
   * @test Verify a mix-in to allow for lazy initialisation of complex infrastructure
   *       tied to a std::function; the intention is to have a »trap« hidden in the
   *       function itself to trigger on first use and perform the one-time
   *       initialisation, then finally lock the object at a fixed place.
   * @see lazy-init.hpp
   * @see lib::RandomDraw
   */
  class LazyInit_test
    : public Test
    {
      
      void
      run (Arg)
        {
          verify_trojanLambda();
          verify_inlineStorage();
          verify_TargetRelocation();
          verify_triggerMechanism();
          verify_lazyInitialisation();
          verify_complexUsageWithCopy();
        }
      
      
      
      /** @test verify construction of the »trap« front-end eventually to trigger initialisation
       *      - this test does not involve any std::function, rather a heap-allocated copy of a λ
       *      # the _target function_ finally to be invoked performs a verifiable computation
       *      # the _delegate_ receives an memory location and returns a reference to the target
       *      # the generated _»trojan λ«_ captures its own address, invokes the delegate,
       *        retrieves a reference to a target functor, and finally invokes this with actual arguments.
       * @remark the purpose of this convoluted scheme is for the _delegate to perform initialisation,_
       *         taking into account the current memory location „sniffed“ by the trojan.
       */
      void
      verify_trojanLambda()
        {
          size_t beacon;
          auto fun = [&](uint challenge){ return beacon+challenge; };
          
          using Sig = size_t(uint);
          CHECK (isFunMember<Sig> (&fun));
          
          beacon = rand();
          uint c = beacon % 42;
          // verify we can invoke the target function
          CHECK (beacon+c == fun(c));

          // verify we can also invoke the target function through a reference
          using FunType = decltype(fun);
          FunType& funRef = fun;
          CHECK (beacon+c == funRef(c));
          
          // construct delegate function exposing the expected behaviour;
          // additionally this function captures the passed-in address.
          RawAddr location{nullptr};
          auto delegate = [&](RawAddr adr) -> FunType&
                            {
                              location = adr;
                              return fun;
                            };
          using Delegate = decltype(delegate);
          auto delP = make_unique<Delegate> (delegate);
          
          // verify the heap-allocated copy of the delegate behaves as expected
          location = nullptr;
          CHECK (beacon+c == (*delP)(this)(c));
          CHECK (location == this);
          
          // now (finally) build the »trap function«...
          auto trojanLambda = TrojanFun<Sig>::generateTrap (delP.get());
          CHECK (sizeof(trojanLambda) == sizeof(size_t));
          
          // on invocation...
          // - it captures its current location
          // - passes this to the delegate
          // - invokes the target function returned from the delegate
          CHECK (beacon+c == trojanLambda(c));
          CHECK (location == &trojanLambda);
          
          // repeat same with a copy, and changed beacon value
          auto trojanClone = trojanLambda;
          beacon = rand();
          c = beacon % 55;
          CHECK (beacon+c == trojanClone(c));
          CHECK (location == &trojanClone);
          CHECK (beacon+c == trojanLambda(c));
          CHECK (location == &trojanLambda);
        }
      
      
      
      /** @test verify that std::function indeed stores a simple functor inline.
       * @remark The implementation of LazyInit relies crucially on a known optimisation
       *         in the standard library ─ which unfortunately is not guaranteed by the standard:
       *         Typically, std::function will apply _small object optimisation_ to place a very
       *         small functor directly into the wrapper, if the payload has a trivial copy-ctor.
       *         `Libstdc++` is known to be rather restrictive, while other implementations trade
       *         increased storage size of std::function against more optimisation possibilities.
       *         LazyInit exploits this optimisation to „spy“ about the current object location,
       *         allowing to execute the lazy initialisation on first use, without further help
       *         by client code. This trickery seems to be the only way, since λ-capture by reference
       *         is broken after copying or moving the host object (typically required for DSL use).
       *         In case this turns out to be fragile, LazyInit should become a "LateInit" and needs
       *         help by the client or the user to trigger initialisation; alternatively the DSL
       *         could be split off into a separate builder object distinct from RandomDraw.
       */
      void
      verify_inlineStorage()
        {
//        char payload[24];// ◁─────────────────────────────── use this to make the test fail....
          const char* payload = "please look elsewhere";
          auto lambda = [payload]{ return RawAddr(&payload); };
          
          RawAddr location = lambda();
          CHECK (location == &lambda);
          
          std::function funWrap{lambda};
          CHECK (funWrap);
          CHECK (not isSameObject (funWrap, lambda));
          
          location = funWrap();
          CHECK (util::isCloseBy (location, funWrap));
          // if »small object optimisation« was used,
          // the lambda will be copied directly into the std:function;
          // otherwise it will be heap allocated and this test fails.
          
          // for context: these are considered "close by",
          // since both are sitting right here in the same stack frame
          CHECK (util::isCloseBy (funWrap, lambda));
        }
      
      
      
      /** @test verify navigating an object structure
       *        by applying known offsets consecutively
       *        from a starting point within an remote instance
       * @remark in the real usage scenario, we know _only_ the offset
       *        and attempt to find home without knowing the layout.
       */
      void
      verify_TargetRelocation()
        {
          struct Nested
            {
              int unrelated{rand()};
              int anchor{rand()};
            };
          struct Demo
            {
              Nested nested;
              virtual ~Demo(){ };
              virtual RawAddr peek()
                {
                  return &nested.anchor;
                }
            };
          
          // find out generic offset...
          const ptrdiff_t offNested = []{
                                         Nested probe;
                                         return captureRawAddrOffset(&probe, &probe.anchor);
                                        }();
          Demo here;
          // find out actual offset in existing object
          const ptrdiff_t offBase = captureRawAddrOffset(&here, &here.nested);
          
          CHECK (offBase > 0);
          CHECK (offNested > 0);
          
          // create a copy far far away...
          auto farAway = make_unique<Demo> (here);
          
          // reconstruct base address from starting point
          RawAddr startPoint = farAway->peek();
          Nested* farNested = relocate<Nested>(startPoint, -offNested);
          CHECK (here.nested.unrelated == farNested->unrelated);

          Demo* farSelf = relocate<Demo> (farNested, -offBase);
          CHECK (here.nested.anchor == farSelf->nested.anchor);
          CHECK (isSameObject (*farSelf, *farAway));
        }
      
      
      
      /** @test demonstrate the trigger mechanism in isolation
       */
      void
      verify_triggerMechanism()
        {
          using Fun = std::function<float(int)>;
          Fun theFun;
          CHECK (not theFun);
          
          int report{0};
          auto delegate = [&report](RawAddr insideFun) -> Fun&
                              {
                                auto realFun = [&report](int num)
                                                          {
                                                            report += num;
                                                            return num + 23.55f;
                                                          };
                                Fun& target = *relocate<Fun>(insideFun, -FUNCTOR_PAYLOAD_OFFSET);
                                report = -42; // as proof that the init-delegate was invoked
                                target = realFun;
                                return target;
                              };
          CHECK (not theFun);
          // install the init-»trap«
          theFun = TrojanFun<float(int)>::generateTrap (&delegate);
          CHECK (theFun);
          CHECK (0 == report);
          
          // invoke function
          int feed{1+rand()%100};
          float res = theFun (feed);
          
          // delegate *and* realFun were invoked
          CHECK (feed == report + 42);
          CHECK (res = feed -42 +23.55f);
          
          // again...
          report = 0;
          feed = -1-rand()%20;
          res = theFun (feed);
          
          // this time the delegate was *not* invoked,
          // only the installed realFun
          CHECK (feed == report);
          CHECK (res = feed + 23.55f);
        }
      
      
      
      /** @test demonstrate a basic usage scenario
       */
      void
      verify_lazyInitialisation()
        {
          using Fun = std::function<float(int)>;
          using Lazy = LazyInit<Fun>;
          
          bool init{false};
          uint invoked{0};
          Lazy funny{funny, [&](Lazy* self)
                              {
                                Fun& thisFun = static_cast<Fun&> (*self);
                                
                                thisFun = [&invoked](int num)
                                                      {
                                                        ++invoked;
                                                        return num * 0.555f;
                                                      };
                                init = true;
                              }};
          CHECK (not invoked);
          CHECK (not init);
          CHECK (funny);
          
          int feed = 1 + rand()%99;
          CHECK (feed*0.555f == funny(feed));
          CHECK (1 == invoked);
          CHECK (init);
        }
      
      
      
      /** elaborate setup used for integration test */
      struct LazyDemo
        : LazyInit<>
        {
          using Fun = std::function<int(int)>;
          
          int seed{0};
          Fun fun; // ◁────────────────────────────────── this will be initialised lazily....
          
          template<typename FUN>
          auto
          buildInit (FUN&& fun2install)
            {
              return [theFun = forward<FUN> (fun2install)]
                     (LazyDemo* self)
                        { // this runs when init is actually performed....
                          CHECK (self);
                          if (self->fun)
                            // chain-up behind existing function
                            self->fun = [self, prevFun=self->fun, nextFun=theFun]
                                        (int i)
                                          {
                                            return nextFun (prevFun (i));
                                          };
                          else
                            // build new function chain, inject seed from object
                            self->fun = [self, newFun=theFun]
                                        (int i)
                                          {
                                            return newFun (i + self->seed);  // Note: binding to actual instance location
                                          };
                        };
            }
          
          
          LazyDemo()
            : LazyInit{MarkDisabled()}
            , fun{}
            {
              installInitialiser(fun, buildInit([](int){ return 0; }));
            }
                                                 // prevent this ctor from shadowing the copy ctors    //////TICKET #963
          template<typename FUN,      typename =disable_if_self<LazyDemo, FUN>>
          LazyDemo (FUN&& someFun)
            : LazyInit{MarkDisabled()}
            , fun{}
            {
              installInitialiser(fun, buildInit (forward<FUN> (someFun)));
            }
          
          template<typename FUN>
          LazyDemo&&
          attach (FUN&& someFun)
            {
              installInitialiser(fun, buildInit (forward<FUN> (someFun)));
              return move(*this);
            }
        };
      
      /**
       * @test use an elaborately constructed example to cover more corner cases
       *     - the function to manage and initialise lazily is _a member_ of the _derived class_
       *     - the initialisation routine _adapts_ this function and links it with the current
       *       object location; thus, invoking this function on a copy would crash / corrupt memory.
       *     - however, as long as initialisation has not been triggered, LazyDemo instances can be
       *       copied; they may even be assigned to existing instances, overwriting their state.
       *     - a second given function will be chained behind the first one; this happens immediately
       *       if the first function was already invoked (and this initialised)
       *     - but when however both functions are attached immediately, prior to invocation,
       *       then an elaborate chain of initialisers is setup behind the scenes and played back
       *       in definition order once lazy initialisation is triggered
       *     - all the intermediary state is safe to copy and move and fork
       * @remark 11/2023 memory allocations were verified using lib::test::Tracker and the EventLog
       */
      void
      verify_complexUsageWithCopy()
        {
          LazyDemo dd;
          CHECK (not dd.isInit());                    // not initialised, since function was not invoked yet
          CHECK (dd.fun);                             // the functor is not empty anymore, since the »trap« was installed
          
          dd.seed = 2;
          CHECK (0 == dd.fun(22));                    // d1 was default initialised and thus got the "return 0" function
          CHECK (dd.isInit());                        // first invocation also triggered the init-routine
          
          // is »engaged« after init and rejects move / copy
          VERIFY_ERROR (LIFECYCLE, LazyDemo dx{move(dd)} );
          
          
          dd = LazyDemo{[](int i)                     // assign a fresh copy (discarding any state in d1)
                          {
                            return i + 1;             // using a "return i+1" function
                          }};
          CHECK (not dd.isInit());
          CHECK (dd.seed == 0);                       // assignment indeed erased any existing settings (seed≔2)
          CHECK (dd.fun);
          
          CHECK (23 == dd.fun(22));                   // new function was tied in (while also referring to self->seed)
          CHECK (dd.isInit());
          dd.seed = 3;                                // set the seed
          CHECK (26 == dd.fun(22));                   // seed value is picked up dynamically
          
          VERIFY_ERROR (LIFECYCLE, LazyDemo dx{move(dd)} );
          
          // attach a further function, to be chained-up
          dd.attach([](int i)
                          {
                            return i / 2;
                          });
          CHECK (dd.isInit());
          CHECK (dd.seed == 3);
          CHECK (12 == dd.fun(21)); // 21+3+1=25 / 2
          CHECK (13 == dd.fun(22));
          CHECK (13 == dd.fun(23));
          dd.seed++;
          CHECK (14 == dd.fun(23)); // 23+4+1=28 / 2
          CHECK (14 == dd.fun(24));
          CHECK (15 == dd.fun(25));
          
          // ...use exactly the same configuration,
          // but applied in one shot -> chained lazy-Init
          dd = LazyDemo{[](int i){return i+1; }}
                .attach([](int i){return i/2; });
          dd.seed = 3;
          CHECK (not dd.isInit());
          CHECK (dd.seed == 3);
          CHECK (dd.fun);
          
          CHECK (12 == dd.fun(21));
          CHECK (13 == dd.fun(22));
          CHECK (13 == dd.fun(23));
          dd.seed++;
          CHECK (14 == dd.fun(23));
          CHECK (14 == dd.fun(24));
          CHECK (15 == dd.fun(25));
          
          // create a nested graph of chained pending init
          dd = LazyDemo{[](int i){return i+1; }};
          LazyDemo d1{dd};
          LazyDemo d2{move(dd)};
          d2.seed = 3;
          d2.attach ([](int i){return i/2; });
          LazyDemo d3{d2};
          d2.attach ([](int i){return i-1; });
          
          // dd was left in defunct state by the move, and thus is locked
          CHECK (not dd.fun);
          CHECK (dd.isInit());
          VERIFY_ERROR (LIFECYCLE, LazyDemo dx{move(dd)} );
          // this can be amended by assigning another instance not yet engaged
          dd = d2;
          d2.seed = 5;
          std::swap (d2,d3);
          std::swap (d3,d1);
          // confused??    ;-)
          CHECK (not dd.isInit() and dd.seed == 3);  // Seed≡3 {i+1} ⟶ {i/2} ⟶ {i-1}
          CHECK (not d1.isInit() and d1.seed == 5);  // Seed≡5 {i+1} ⟶ {i/2} ⟶ {i-1}
          CHECK (not d2.isInit() and d2.seed == 3);  // Seed≡3 {i+1} ⟶ {i/2}
          CHECK (not d3.isInit() and d3.seed == 0);  // Seed≡0 {i+1}
          
          CHECK (12 == dd.fun(23)); // 23+3 +1 = 27/2 = 13 -1 = 12
          CHECK (13 == d1.fun(23)); // 23+5 +1 = 29/2 = 14 -1 = 13
          CHECK (13 == d2.fun(23)); // 23+3 +1 = 27/2 = 13    = 13
          CHECK (24 == d3.fun(23)); // 23+0 +1                = 24
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (LazyInit_test, "unit common");
  
  
}} // namespace lib::test
