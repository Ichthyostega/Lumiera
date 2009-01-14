/*
  LifeCycle(Test)  -  checking the lifecycle callback hooks provided by AppState
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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
#include "lib/util.hpp"

#include "include/lifecycle.h"




namespace lumiera {
  namespace test  {
    
    uint basicInit (0);
    uint customCallback (0);
    
    void basicInitHook () { ++basicInit; }
    void myCallback() { ++customCallback; }

    Symbol MY_DEADLY_EVENT = "dial M for murder";
    
    
    namespace // register them to be invoked by lifecycle event id 
      {
      LifecycleHook _schedule1 (ON_BASIC_INIT, &basicInitHook);         
      LifecycleHook _schedule2 (MY_DEADLY_EVENT, &myCallback);   
    }
    
    
    /** @test the global lifecycle hooks got registered,
     *        the ON_BASIC_INIT hook has been already called,
     *        while our custom callback can be trigged at our will
     */
    class LifeCycle_test : public Test
      {
        virtual void 
        run (Arg)
          {
            ASSERT (basicInit, "the basic-init callback hasn't been invoked automatically");
            ASSERT (1 == basicInit, "the basic-init callback has been invoked more than once");
            
            ASSERT (!customCallback);
            LifecycleHook::trigger  (MY_DEADLY_EVENT);
            ASSERT ( 1 == customCallback);
          }
        
      };
    
      LAUNCHER (LifeCycle_test, "function common");

      
  } // namespace test
    
} // namespace util

