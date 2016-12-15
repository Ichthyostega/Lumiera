/*
  DispatcherLooper(Test)  -  verify loop control and timing functionality of ProcDispatcher

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "proc/control/command-queue.hpp"
//#include "proc/control/command.hpp"
//#include "proc/control/command-registry.hpp"
//#include "lib/test/event-log.hpp"

//#include "proc/control/test-dummy-commands.hpp"

//#include <cstdlib>


namespace proc {
namespace control {
namespace test    {
  
  
//  using std::function;
//  using std::rand;
  
  
  namespace { // test fixture...
    
    
  }//(End) test fixture
  
//  typedef shared_ptr<CommandImpl> PCommandImpl;
//  typedef HandlingPattern const& HaPatt;
  
  
  
  
  
  
  /******************************************************************************//**
   * @test verify encapsulated control logic of ProcDispatcher.
   *       - fusing of conditions for the pthread waiting condition
   *       - detection and handling of work states
   *       - management of builder run triggers
   * 
   * @see proc::control::Looper
   * @see DispatcherLoop
   * @see CommandQueue_test
   */
  class DispatcherLooper_test : public Test
    {
      
      
      virtual void
      run (Arg)
        {
          verifyBasics();
        }
      
      
      void
      verifyBasics()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DispatcherLooper_test, "unit controller");
  
  
}}} // namespace proc::control::test
