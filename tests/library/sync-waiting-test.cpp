/*
  SyncWaiting(Test)  -  check the monitor object based wait/notification

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file sync-waiting-test.cpp
 ** unit test \ref SyncWaiting_test
 */


#include "lib/test/run.hpp"
#include "lib/error.hpp"

#include "lib/thread.hpp"
#include "lib/sync.hpp"

#include <functional>

using std::bind;
using test::Test;


namespace lib {
namespace test{
  
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
    
    
    /** demonstrates how to wait on a simple boolean flag */
    class SyncOnBool
      : public Token,
        public Sync<NonrecursiveLock_Waitable>
      {
      protected:
        volatile bool got_new_data_;
        
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
    
    
    /** this variant demonstrates how to wait on an condition
     *  defined in terms of a (bool) member function
     */
    class SyncOnMemberPredicate
      : public SyncOnBool
      {
        bool checkTheFlag() { return this->got_new_data_; }
        
      public:
        void getIt()
          {
            Lock await(this, &SyncOnMemberPredicate::checkTheFlag);
            sum_ += input_;
          }
        
      };
    
  } // (End) test classes and data....
  
  
  
  
  
  
  
  
  
  
  /************************************************************************//**
   * @test concurrent waiting and notification, implemented via object monitor.
   * This test covers the second part of the monitor pattern, which builds upon
   * the locking part, additionally using an embedded condition. We provide
   * several pre-configured ways of specifying the condition to wait upon.
   * - check a boolean flag
   * - evaluate a member function as predicate
   * 
   * @see SyncLocking_test
   * @see sync.hpp
   */
  class SyncWaiting_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          SyncOnBool use_sync_var;
          waitPingPong (use_sync_var);
          
          SyncOnMemberPredicate use_member_pred;
          waitPingPong (use_member_pred);
        }
      
      
      /** 
       * Helper actually performing the test: 
       * creates two threads and let them block and wait until a start value is given.
       * When awakened, each thread should add the start value to a common sum field.
       * @param tok object containing the monitor and condition to be tested. 
       */
      void 
      waitPingPong (Token& tok)
        {
          typedef ThreadJoinable Thread;  //////////////////////////////////////WIP
          
          Thread ping ("SyncWaiting ping", bind (&Token::getIt, &tok));
          Thread pong ("SyncWaiting pong", bind (&Token::getIt, &tok));
          
          CHECK (ping.isValid());
          CHECK (pong.isValid());
          CHECK (0 == tok.result());
          
          usleep (100000); // if the threads don't block correctly, they've missed their chance by now... 
          
          // kick off the notification cascade...
          uint val = (rand() % 1000);
          tok.provide (val);
          
          // wait for the two Threads to finish their handshake
          pong.join();
          ping.join();
          
          CHECK (2*val == tok.result());
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (SyncWaiting_test, "unit common");
  
  
  
}} // namespace lib::test
