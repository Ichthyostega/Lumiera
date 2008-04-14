/*
  LifeCycle(Test)  -  checking the lifecycle callback hooks provided by Appconfig
 
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



#include "common/test/run.hpp"
#include "common/util.hpp"

#include "lib/appconfig.hpp"




namespace lumiera
  {
  namespace test
    {
    
    uint basicInit (0);
    uint customCallback (0);
    
    void basicInitHook () { ++basicInit; }
    void myCallback() { ++customCallback; }

    Symbol MY_MAGIC_MEGA_EVENT = "dial M for murder";
    
    
    namespace // register them to be invoced by lifecycle event id 
      {
      LifecycleHook _schedule1 (ON_BASIC_INIT, &basicInitHook);         
      LifecycleHook _schedule2 (MY_MAGIC_MEGA_EVENT, &myCallback);   
    }
    
    
    /** @test the global lifecycle hooks got registered,
     *        the ON_BASIC_INIT hook has been already called,
     *        while our custom callback can be trigged at our will
     */
    class LifeCycle_test : public Test
      {
        virtual void 
        run (Arg arg)
          {
            ASSERT (basicInit, "the basic-init callback hasn't been invoked automatically");
            ASSERT (1 == basicInit, "the basic-init callback has been invoked more than once");
            
            ASSERT (!customCallback);
            Appconfig::lifecycle (MY_MAGIC_MEGA_EVENT);
            ASSERT ( 1 == customCallback);
          }
        
      };
    
      LAUNCHER (LifeCycle_test, "function common");

      
  } // namespace test
    
} // namespace util

