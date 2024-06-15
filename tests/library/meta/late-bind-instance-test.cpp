/*
  LateBindInstance(Test)  -  verify rewriting of member function invocation 

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

/** @file late-bind-instance-test.cpp
 ** unit test \ref LateBindInstance_test
 */


#include "lib/test/run.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/tracking-dummy.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include <tuple>
#include <string>
#include <cstdlib>


namespace lib  {
namespace meta {
namespace test {
  
  using std::move;
  using std::rand;
  using std::tuple;
  using std::string;
  using lib::meta::dump;
  using lib::test::Tracker;
  
  
  
  
  
  
  /**********************************************************************************//**
   * @test verify a scheme to supply the actual instance for member function invocation.
   * @remark in the end this is just a value replacement in a tuple, where the position
   *         is marked with a placeholder type. This scheme is used for starting threads,
   *         where the actual thread instance is not known yet at the point where the
   *         arguments for the thread-function are constructed
   * @see lib::launchDetached
   */
  class LateBindInstance_test : public Test
    {
      void
      run (Arg)
        {
          demonstrateUsage();
          verify_forwarding();
          verify_cornerCases();
        }
      
      
      
      /** @test demonstrate the usage of lib::meta::lateBindInstance():
       *  - construct an argument tuple
       *  - mark the position of the `this`-ptr with a placeholder
       *  - rewrite the arguments later when the actual instance is known
       *  - member function can now be invoked, since the instance was injected.
       */
      void
      demonstrateUsage()
        {
          uint randomLimit = 2 + rand() % 98;
          
          auto plannedArgs
            = tuple{InstancePlaceholder<LateBindInstance_test>{}
                   ,randomLimit
                   };
          
          // the actual instance may be available only later...
          Test& instanceRef = *this;
          
          // now rewrite the argument tuple to inject the instance-ptr
          auto preparedArgs = lateBindInstance (instanceRef, plannedArgs);
          
          uint res = std::apply (&LateBindInstance_test::theMember, preparedArgs);
          CHECK (res < randomLimit);
        }
      
      uint
      theMember (uint limit)
        {
          return rand() % limit;
        }
      
      
      
      /** @test verify that the rewriting process does not incur unnecessary data copies
       */
      void
      verify_forwarding()
        {
          auto& log = Tracker::log;
          log.clear (this);
          
          InstancePlaceholder<LateBindInstance_test> marker;
          Tracker t1(11);
          
          log.event("construct tuple");
          tuple tup(t1, marker, Tracker{23}, 55);

          Test& instanceRef = *this;
          
          log.event("invoke lateBindInstance");
          auto tup1r = lateBindInstance (instanceRef, tup);
          log.event("got result");
          
          // before invocation the Tracker instances are created (obviously..)
          CHECK (log.verifyCall("ctor").arg(11)
                    .beforeCall("ctor-copy").arg("Track{11}")
                    .beforeEvent("invoke lateBindInstance"));
          CHECK (log.verifyCall("ctor").arg(23)
                    .beforeCall("ctor-move").arg("Track{23}")
                    .beforeCall("dtor").arg(Tracker::DEFUNCT)
                    .beforeEvent("invoke lateBindInstance"));
          
          // but there is no copy operation after the invocation
          CHECK (log.ensureNot("ctor-copy")
                    .afterEvent("invoke lateBindInstance"));
          
          // both Tracker instances are moved two times
          // - once to create the maybeInject-invocation and
          // - once to consolidate the result
          // for each instance one moved-away temporary is destroyed
          CHECK (log.verifyEvent("invoke lateBindInstance")
                    .beforeCall("ctor-move").arg("Track{11}")
                    .beforeCall("ctor-move").arg("Track{11}")
                    .beforeCall("dtor").arg(Tracker::DEFUNCT)
                    .beforeEvent("got result"));
          CHECK (log.verifyEvent("invoke lateBindInstance")
                    .beforeCall("ctor-move").arg("Track{23}")
                    .beforeCall("ctor-move").arg("Track{23}")
                    .beforeCall("dtor").arg(Tracker::DEFUNCT)
                    .beforeEvent("got result"));
          
          cout << "____Tracker-Log_______________\n"
               << util::join(Tracker::log,      "\n")
               << "\n───╼━━━━━━━━━━━╾──────────────"<<endl;
        }
      
      
      /** @test replacement happens uniformly on all kinds of tuples
       */
      void
      verify_cornerCases()
        {
          long dummy{555};
          InstancePlaceholder<long> marker;
         CHECK (dump(lateBindInstance (dummy, tuple<>{}))         == "()"_expect);
         CHECK (dump(lateBindInstance (dummy, tuple{42}))         == "(42)"_expect);
         CHECK (dump(lateBindInstance (dummy, tuple{1,2,3}))      == "(1,2,3)"_expect);
         CHECK (dump(lateBindInstance (dummy, tuple{marker,2,3})) == "(↗555,2,3)"_expect);
         CHECK (dump(lateBindInstance (dummy, tuple{1,marker,3})) == "(1,↗555,3)"_expect);
         CHECK (dump(lateBindInstance (dummy, tuple{1,2,marker})) == "(1,2,↗555)"_expect);
         CHECK (dump(lateBindInstance (dummy, tuple{marker}))     == "(↗555)"_expect);
         CHECK (dump(lateBindInstance (dummy, tuple{string{"1"}
                                                   ,"2"
                                                   ,marker
                                                   ,tuple(2,3)
                                                   , 5.5
                                                   }))            == "(1,2,↗555,«tuple<int, int>»──(2,3),5.5)"_expect);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (LateBindInstance_test, "unit meta");
  
  
  
}}} // namespace lib::meta::test
