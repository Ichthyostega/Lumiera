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
#include "lib/test/test-helper.hpp"
#include "lib/format-util.hpp"
#include "lib/error.hpp"

#include <iostream>
#include <string>

using util::join;
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
  
  
  void doThrow() { throw Error("because I feel like it"); }
  int dontThrow() { return 2+2; }
  
  
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
        }
      
      
      void
      verify_simpleUsage ()
        {
          EventLog log(this);
          CHECK (isnil (log));
          
          log.event("α");
          log.event("β");
          CHECK (!isnil(log));
          
          log.verify("α");
          log.verify("β");
          VERIFY_ERROR (ASSERTION, log.verify("γ"));
          
          log.verify("α").before("β");
          VERIFY_ERROR (ASSERTION, verify("β").before("α"));
          
          CHECK (join(log) == "LOG::TestEventLog_test"+Log::instanceHash(this)
                           +  ", Rec(event|  |{α}), Rec(event|  |{β})");
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
          
          log.join(bar).join(foo);
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

