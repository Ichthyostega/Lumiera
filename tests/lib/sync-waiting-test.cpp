/*
  SyncWaiting(Test)  -  check the monitor object based wait/notification
 
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
#include "include/error.hpp"

#include "lib/sync.hpp"

#include <glibmm.h>

#include <iostream>

using std::cout;
using test::Test;


namespace lib {
  namespace test {
  
    namespace { // private test classes and data...
    
      
      /** Interface defining the basic interaction pattern for this test */
      class Token
        {
        public:
            
          /** blocking concurrent operation */
          virtual void getIt()             =0;
          
          /** start the notification chain */
          virtual void provide (uint val)  =0;
          
          /** harvesting the result...*/
          uint result () { return sum_; }
          
          
        protected:
          volatile uint sum_, input_;
          
          virtual ~Token() {}
          
          Token() : sum_(0), input_(0) {}
        };
      
      
      class SyncOnBool
        : public Token,
          public Sync<NonrecursiveLock_Waitable>
        {
          bool got_new_data_;
          
        public:
          SyncOnBool() : got_new_data_ (false) {}
          
          void getIt()
            {
              Lock(this).wait (got_new_data_);
              sum_ += input_;
            }
          
          void provide (uint val)
            {
              Lock sync(this);
              input_ = val;
              got_new_data_ = true;
              sync.notifyAll();
            }
        };
      
    } // (End) test classes and data....
    
    
    
    
    
    
    
    
    
    
    /****************************************************************************
     * @test concurrent waiting and notification, implemented via object monitor.
     * This test covers the second part of the monitor pattern, which builds upon
     * the locking part an additionally uses an embedded condition. We provide
     * several pre-configured ways of specifying the condition to wait upon.
     * 
     * @see sync.hpp
     */
    class SyncWaiting_test : public Test
      {
        
        virtual void
        run (Arg) 
          {
            if (!Glib::thread_supported()) 
              Glib::thread_init();

            SyncOnBool use_sync_var;
            waitPingPong (use_sync_var);
          }
        
        
        /** 
         * Helper actually performing the test: 
         * creates two threads and let them block and wait cross-wise.
         * @param tok object containing the monitor and condition to be tested. 
         */
        void 
        waitPingPong (Token& tok)
          {
            Glib::Thread  *ping, *pong;
          
            ping = Glib::Thread::create(sigc::mem_fun(tok, &Token::getIt), true);
            pong = Glib::Thread::create(sigc::mem_fun(tok, &Token::getIt), true);
              
            ASSERT (ping);
            ASSERT (pong);
            ASSERT (0 == tok.result());
            
            sleep (1); // if the threads don't block correctly, they've missed their chance by now... 
            
            // kick off the notification cascade...
            uint val = (rand() % 1000);
            tok.provide (val);
            
            // wait for the two Threads to finish their handshake
            pong->join();
            ping->join();
            
            ASSERT (2*val == tok.result());
          }
      };
    
      
    
    /** Register this test class... */
    LAUNCHER (SyncWaiting_test, "unit common");
    
    
    
  } // namespace test

} // namespace lumiera
