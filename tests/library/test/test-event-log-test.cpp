/*
  TestEventLog(Test)  -  helper for event registration and verification

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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
//#include "lib/test/test-helper.hpp"
#include "lib/format-util.hpp"
#include "lib/test/event-log.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"

#include <iostream>
#include <string>

using util::join;
using util::isnil;
using lumiera::error::LUMIERA_ERROR_ASSERTION;

using std::string;
using std::cout;
using std::endl;


namespace lib {
namespace test{
namespace test{
  
  template<class T>
  class Wrmrmpft 
    {
      T tt_;
    };
  
  struct Murpf { };
  
  
  
  
  /***********************************************************//**
   * @test verify a logging facility, which can be used to ensure
   *       some events happened while running test code.
   *       
   * @see event-log.hpp
   */
  class TestEventLog_test : public Test
    {
      void
      run (Arg)
        {
          verify_simpleUsage();
          verify_backwardMatch();
          verify_negatedMatch();
          verify_logJoining();
        }
      
      
      void
      verify_simpleUsage ()
        {
          EventLog log(this);
          CHECK (isnil (log));
          
          log.event("α");
          log.event("β");
          CHECK (!isnil(log));
          
          CHECK (log.verify("α"));
          CHECK (log.verify("β"));
          CHECK (not log.verify("γ"));
          
          CHECK (log.verify("α").before("β"));
          CHECK (not log.verify("β").before("α"));
          
          CHECK (join(log) == "Rec(EventLogHeader| ID = "+idi::instanceTypeID(this)+" ), "
                           +  "Rec(event|{α}), "
                           +  "Rec(event|{β})");
        }
      
      
      void
      verify_backwardMatch ()
        {
          EventLog log("baked beans");
          log.event("spam");
          log.event("ham");
          
          CHECK (log.verify("ham").after("spam").after("beans"));
          CHECK (log.verify("ham").after("beans").before("spam").before("ham"));
          CHECK (not log.verify("spam").after("beans").after("ham"));
        }
      
      
      void
      verify_negatedMatch ()
        {
          EventLog log("eggs");
          log.event("spam");
          log.event("ham");
          log.event("spam");
          
          CHECK (log.ensureNot("baked beans"));
          CHECK (log.ensureNot("ham").before("eggs"));
          CHECK (log.ensureNot("spam").after("spam").before("eggs"));
          CHECK (not log.ensureNot("spam").before("spam").after("eggs").before("ham"));
        }
      
      
      void
      verify_logJoining ()
        {
          EventLog log1("spam");
          EventLog log2("ham");
          
          log1.event("baked beans");
          log2.event("eggs");
          
          CHECK (log1.verify("spam").before("baked beans"));
          CHECK (log2.verify("ham").before("eggs"));
          
          CHECK (log1.ensureNot("ham"));
          CHECK (log1.ensureNot("eggs"));
          CHECK (log2.ensureNot("spam"));
          CHECK (log2.ensureNot("baked beans"));
          
          EventLog copy(log2);
          copy.event("bacon");
          CHECK (copy.verify("ham").before("eggs").before("bacon"));
          CHECK (log2.verify("ham").before("eggs").before("bacon"));
          CHECK (log1.ensureNot("bacon"));
          
          CHECK (log1 != log2);
          CHECK (copy == log2);
          
          log2.joinInto(log1);
          
          CHECK (log1 == log2);
          CHECK (copy != log2);
          
          CHECK (log1.verify("logJoin|{ham}").after("baked beans"));
          CHECK (log1.verify("logJoin|{ham}").after("EventLogHeader| ID = ham").before("eggs").before("bacon").before("logJoin"));
          
          log2.event("sausage");
          CHECK (log1.verify("sausage").after("logJoin").after("spam"));
          
          CHECK (copy.ensureNot("logJoin"));
          CHECK (copy.ensureNot("sausage"));
          CHECK (copy.verify("joined|{spam}").after("EventLogHeader"));
          
          copy.event("spam tomato");
          CHECK (log1.ensureNot("spam tomato"));
          CHECK (log2.ensureNot("spam tomato"));
          CHECK (copy.verify("joined|{spam}").before("spam tomato"));
          
          
          CHECK (join(log1) == string(
                               "Rec(EventLogHeader| ID = spam ), "
                               "Rec(event|{baked beans}), "
                               "Rec(EventLogHeader| ID = ham ), "
                               "Rec(event|{eggs}), "
                               "Rec(event|{bacon}), "
                               "Rec(logJoin|{ham}), "
                               "Rec(event|{sausage})"));
          
          CHECK (join(copy) == string(
                               "Rec(EventLogHeader| ID = ham ), "
                               "Rec(joined|{spam}), "
                               "Rec(event|{spam tomato})"));
        }
      
      
      /** @test prints TODO */
      void
      checkTODO ()
        {
#if false ///////////////////////////////////////////////////////////////////////////////////////////////TICKET #975
          log.verifyEvent("ctor");
          log.verify("ctor").arg("dummy");
          
          CHECK ("dummy" == log.getID().getSym());
          CHECK ("ID<gui::model::test::MockElm>-dummy" = string(log.getID()));
          
          VERIFY_ERROR (ASSERTION, log.verifyCall("reset"));
          
          log.reset();
          log.verify("reset");
          log.verifyCall("reset");
          log.verifyEvent("reset");
          log.verify("reset").after("ctor");
          log.verify("ctor").before("reset");
          VERIFY_ERROR (ASSERTION, log.verify("reset").before("ctor"));
          VERIFY_ERROR (ASSERTION, log.verify("ctor").after("reset"));
          
          log.verify("reset").before("reset");
          log.verify("reset").beforeEvent("reset");
          log.verifyCall("reset").before("reset");
          log.verifyCall("reset").beforeEvent("reset");
          VERIFY_ERROR (ASSERTION, log.verifyCall("reset").afterCall("reset"));
          VERIFY_ERROR (ASSERTION, log.verifyCall("reset").afterEvent("reset"));
          VERIFY_ERROR (ASSERTION, log.verifyEvent("reset").afterEvent("reset"));
          
          CHECK (!log.isTouched());
          CHECK (!log.isExpanded());
          
          log.noteMsg("dolorem ipsum quia dolor sit amet consectetur adipisci velit.");
          log.verifyNote("Msg");
          log.verifyCall("noteMsg");
          log.verifyCall("noteMsg").arg("lorem ipsum");
          log.verifyCall("noteMsg").argMatch("dolor.+dolor\\s+");
          log.verifyMatch("Rec\\(note.+kind = Msg.+msg = dolorem ipsum");
          
          EventLog log = log.getLog();
          log.verify("ctor")
             .before("reset")
             .before("lorem ipsum");
          
          MockElm foo("foo"), bar;
          foo.verify("ctor").arg("foo");
          bar.verify("ctor").arg();
          
          bar.ensureNot("foo");
          log.ensureNot("foo");
          log.ensureNot("foo");
          VERIFY_ERROR (ASSERTION, foo.ensureNot("foo"));
          
          log.joinInto(bar).joinInto(foo);
          log.verifyEvent("logJoin").arg(bar.getID())
             .beforeEvent("logJoin").arg("foo");
          
          log.verifyEvent("logJoin").arg(bar.getID())
              .beforeEvent("logJoin").arg("foo");
          log.verify("ctor").arg("foo");
          log.verify("ctor").arg("foo");
          log.verify("ctor").arg("dummy")
             .before("ctor").arg(bar.getID())
             .before("ctor").arg("foo");
          
          log.kill();
          foo.noteMsg("dummy killed");
          log.verifyEvent("dtor").on("dummy")
             .beforeCall("noteMsg").on("foo");
           // and when actually no exception is raised, this is an ASSERTION failure
           VERIFY_ERROR (ASSERTION, VERIFY_ERROR (EXCEPTION, dontThrow() ));
#endif    ///////////////////////////////////////////////////////////////////////////////////////////////TICKET #975
        }
    };
  
  LAUNCHER (TestEventLog_test, "unit common");
  
  
}}} // namespace lib::test::test

