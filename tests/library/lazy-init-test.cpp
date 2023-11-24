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
#include "lib/lazy-init.hpp"
//#include "lib/format-string.hpp"
//#include "lib/test/test-helper.hpp"
//#include "lib/test/testdummy.hpp"
#include "lib/test/diagnostic-output.hpp" /////////////////////TODO TODOH
#include "lib/util.hpp"

//#include <array>



namespace lib {
namespace test{
  
//  using util::_Fmt;
  using util::isSameObject;
  using lib::meta::isFunMember;
//  using lib::meta::_FunRet;
//  using err::LUMIERA_ERROR_LIFECYCLE;
  
  
  
  namespace { // policy and configuration for test...

    //
  }//(End) Test config


  
  
  
  /***********************************************************************************//**
   * @test Verify a mix-in to allow for lazy initialisation of complex infrastructure
   *       tied to a std::function; the intention is to have a »trap« hidden in the
   *       function itself to trigger on first use and perform the one-time
   *       initialisation, then finally lock the object in place.
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
        }
      
      
      
      /** @test verify construction of the »trap« front-end eventually to trigger initialisation
       *      - this test does not involve any std::function, rather a heap-allocated copy of a λ
       *      # the _target function_ finally to be invoked performs a verifiable computation
       *      # the _delegate_ receives an memory location and returns a reference to the target
       *      # the generated _»trojan λ«_ captures its own address, invokes the delegate,
       *        retrieves a reference to a target functor, and invokes these with actual arguments.
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
          Delegate *delP = new Delegate(delegate);
          
          // verify the heap-allocated copy of the delegate behaves as expected
          location = nullptr;
          CHECK (beacon+c == (*delP)(this)(c));
          CHECK (location == this);
          
          // now (finally) build the »trap function«,
          // taking ownership of the heap-allocated delegate copy
          auto trojanLambda = TrojanFun<Sig>::generateTrap (delP);
          CHECK (sizeof(trojanLambda) == sizeof(size_t));
          
          // on invocation...
          // - it captures its current location
          // - passes this to the delegate
          // - invokes the target function returned from the delegate
          CHECK (beacon+c == trojanLambda(c));
          CHECK (location == &trojanLambda);
          
          // repeat that with a copy, and changed beacon value
          auto trojanClone = trojanLambda;
          beacon = rand();
          c = beacon % 55;
          CHECK (beacon+c == trojanClone(c));
          CHECK (location == &trojanClone);
          CHECK (beacon+c == trojanLambda(c));
          CHECK (location == &trojanLambda);
        }
      
      
      
      /** @test verify that std::function indeed stores a simple functor inline
       * @remark The implementation of LazyInit relies crucially on a known optimisation
       *         in the standard library ─ which unfortunately is not guaranteed by the standard:
       *         Typically, std::function will apply _small object optimisation_ to place a very
       *         small functor directly into the wrapper, if the payload has a trivial copy-ctor.
       *         Libstdc++ is known to be rather restrictive, other implementations trade increased
       *         storage size of std::function against more optimisation possibilities.
       *         LazyInit exploits this optimisation to „spy“ about the current object location,
       *         to allow executing the lazy initialisation on first use, without further help
       *         by client code. This trickery seems to be the only way, since λ-capture by reference
       *         is broken after copying or moving the host object (which is required for DSL use).
       *         In case this turns out to be fragile, LazyInit should become a "LateInit" and needs
       *         help by the client or the user to trigger initialisation; alternatively the DSL
       *         could be split off into a separate builder object distinct from RandomDraw.
       */
      void
      verify_inlineStorage()
        {
//        char payload[24];// ◁─────────────────────────────── use this to make the test fail....
          const char* payload = "Outer Space";
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
       *        and and attempt to find home without knowing the layout.
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
          auto farAway = std::make_unique<Demo> (here);
          
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
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (LazyInit_test, "unit common");
  
  
}} // namespace lib::test
