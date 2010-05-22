/*
  AdviceBasics(Test)  -  basic behaviour of the Advice collaboration
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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
//#include "lib/test/test-helper.hpp"

#include "lib/advice.hpp"
//#include "lib/p.hpp"
//#include "proc/assetmanager.hpp"
//#include "proc/asset/inventory.hpp"
//#include "proc/mobject/session/clip.hpp"
//#include "proc/mobject/session/track.hpp"
//#include "lib/meta/trait-special.hpp"
//#include "lib/util-foreach.hpp"
//#include "lib/symbol.hpp"

//#include <iostream>
//#include <string>
#include <cstdlib>

//using lib::test::showSizeof;
//using lib::test::randStr;
//using util::isSameObject;
//using util::and_all;
//using util::for_each;
//using util::isnil;
//using lib::Literal;
//using lib::Symbol;
//using lumiera::P;
//using std::string;
using std::rand;
//using std::cout;
//using std::endl;



namespace lib {
namespace advice {
namespace test {
  
  namespace {
    
    class TheAdvised
      : private advice::Request<int>
      {
      public:
        TheAdvised (Literal topic =0)
          {
            rebind (topic);
          }
        
        void
        rebind (Literal topic)
          {
            defineBinding (topic);
          }
        
        bool
        got(int val)
          {
            return val == getAdvice();
          }
      };
    
    
    class TheAdvisor
      {
        advice::Provision<int> link_;
          
      public:
        TheAdvisor (Literal topic =0)
          {
            rebind (topic);
          }
        
        void
        rebind (Literal topic)
          {
            link_.defineBinding (topic);
          }
        
        void
        publish (int val)
          {
            link_.setAdvice (val);
          }
        
        void
        clear()
          {
            link_.retractAdvice(); 
          }
      };
  }
  
  
  
  /*******************************************************************************
   * @test proof-of-concept for the Advice collaboration.
   *       Advice allows data exchange without coupling the participants tightly.
   *       This test demonstrates the basic expected behaviour in a simple but
   *       typical situation: two unrelated entities exchange a piece of data
   *       just by referring to a symbolic topic ID.
   * 
   * @todo partially unimplemented and thus commented out ////////////////////TICKET #605
   * 
   * @see advice.hpp
   * @see AdviceSituations_test
   * @see AdviceMultiplicity_test
   * @see AdviceConfiguration_test
   * @see AdviceBindingPattern_test
   * @see AdviceIndex_test implementation test
   */
  class AdviceBasics_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleExchange();
          createCollaboration();
          overwriting_and_retracting();
        }
      
      
      /** @test the very basic usage situation: the advisor sets an information value
       *        and the advised entity picks it up. */
      void
      simpleExchange()
        {
          TheAdvised client;         // implicitly opens an request-for-advice
          CHECK (client.got (0));    // no advice yet --> getting the default int()
          
          TheAdvisor server;         // implicitly prepares an advice provision
          CHECK (client.got (0));    // but as no advice was provided yet, nothing happens
          
          int rr (1 + (rand() % 1000));
          
          server.publish (rr);       // now an match is detected, creating an advice channel
          CHECK (client.got (rr));   // ..so the client can pick up the provided advice value
        }
      
      
      /** @test multiple ways how to initiate the advice collaboration */
      void
      createCollaboration()
        {
          TheAdvised client1 ("topic1()");
          TheAdvisor server2 ("topic2()");
          
          int r1 (1 + (rand() % 1000));
          int r2 (1 + (rand() % 1000));
          
          server2.publish (r2);
          CHECK (client1.got(0));
          
          TheAdvised client2 ("topic2()");
          CHECK (client2.got(r2));
          
          TheAdvisor server1;
          CHECK (client1.got(0));
          
          server1.publish (r1);
          CHECK (client1.got(0));
          CHECK (client2.got(r2));
          
          server1.rebind ("topic1()");
          CHECK (client1.got(r1));
          CHECK (client2.got(r2));
        }
      
      
      /** @test changing the provided advice, finally retracting it,
       *        causing fallback on the default value. Any given advisor
       *        can connect to the advice system with multiple bindings
       *        consecutively. The connection has no identity beside this
       *        binding, so another server (advisor) can step into an
       *        existing connection and overwrite or retract the advice.
       *        Unless retracted, advice remains in the system,
       *        even after the advisor is gone.
       */
      void
      overwriting_and_retracting()
        {
          TheAdvised client1 ("topic1");
          TheAdvised client2 ("topic2");
          CHECK (client1.got(0));
          CHECK (client2.got(0));
          
          int r1 (1 + (rand() % 1000));
          int r2 (1 + (rand() % 1000));
          
          {
            TheAdvisor server("topic1()");
            CHECK (client1.got(0));
            CHECK (client2.got(0));
            
            server.publish (r1);
            CHECK (client1.got(r1));
            CHECK (client2.got(0));
            
            server.publish (r2);
            CHECK (client1.got(r2));
            CHECK (client2.got(0));
            
            server.rebind("topic2()");
            CHECK (client1.got(0));
            CHECK (client2.got(r2));
          }
          
          CHECK (client1.got(0));
          CHECK (client2.got(r2));
          
          {
            TheAdvisor anotherServer("topic1");
            CHECK (client1.got(0));
            CHECK (client2.got(r2));
            
            anotherServer.publish (r1);
            CHECK (client1.got(r1));
            CHECK (client2.got(r2));
          }
          
          CHECK (client1.got(r1));
          CHECK (client2.got(r2));
          
          {
            TheAdvisor yetAnotherServer("topic2");
            CHECK (client1.got(r1));
            CHECK (client2.got(r2));
            
            yetAnotherServer.publish (r1);
            CHECK (client1.got(r1));
            CHECK (client2.got(r1));
            
            yetAnotherServer.rebind("topic1");
            CHECK (client1.got(r1));
            CHECK (client2.got(0));
            
            yetAnotherServer.clear();
            CHECK (client1.got(0));
            CHECK (client2.got(0));
            
            yetAnotherServer.rebind("topic2");
            CHECK (client1.got(0));
            CHECK (client2.got(0));
            
            yetAnotherServer.publish (r1);
            CHECK (client1.got(0));
            CHECK (client2.got(r1));
          }
          
          CHECK (client1.got(0));
          CHECK (client2.got(r1));
          
          client1.rebind("topic2");
          CHECK (client1.got(r1));
          CHECK (client2.got(r1));
          
          client2.rebind("nonExistingTopic");
          CHECK (client1.got(r1));
          CHECK (client2.got(0));
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (AdviceBasics_test, "unit common");
  
  
}}} // namespace lib::advice::test
