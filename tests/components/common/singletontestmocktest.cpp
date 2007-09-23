/*
  SingletonTestMock(Test)  -  using Singleton for injecting Test-Mocks
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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
#include "common/singleton.hpp"
#include "common/util.hpp"
#include "nobugcfg.h"

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <iostream>

using boost::lexical_cast;
using boost::format;
using util::isnil;
using std::string;
using std::cout;


namespace cinelerra
  {
  namespace test
    {
    

    /**
     * Client Class normally to be instantiated as Singleton.
     * But for tests, this class should be replaced by a Mock....
     */
    class TargetObj
      {
        int callCnt;
        char* typid;
        format msg;
        
      public:
        TargetObj(char* ty="TargetObj") 
          : callCnt (0), typid(ty), msg ("%s::doIt() call=%d\n")
          {
            TRACE (singleton, "ctor %s", typid);
          }
        virtual ~TargetObj()
          {
            TRACE (singleton, "dtor %s", typid);
          }
        
        void doIt ()
          {
            ++callCnt;
            cout << msg % typid % callCnt;
          }
        int getCnt () {return callCnt; }
        
      };
      
      
    /**
     * Mock-1 to replace the Client Class...
     */
    struct Mock_1 : TargetObj
      {
        Mock_1() : TargetObj("Mock_1") {};
      };
      
    /**
     * Mock-2 to replace the Client Class...
     */
    struct Mock_2 : TargetObj
      {
        Mock_2() : TargetObj("Mock_2") {};
      };
      
      
    
    
    
    
    
    
    
    
    /*******************************************************************
     * @test inject a Mock object into the Singleton Factory, 
     *       to be returned and used in place of the original object.
     * Expected results: Mock(s) called, no memory leaks.
     * @see  cinelerra::Singleton
     * @see  cinelerra::singleton::Static
     */
    class SingletonTestMock_test : public Test
      {
        Singleton<TargetObj> instance;
        
        virtual void run(Arg arg) 
          {
            string scenario = isnil(arg)? "default" : arg[1];
            
            if (scenario == "default")
              injectBoth();
            else 
            if (scenario == "noMock")
              noMock();
            else 
            if (scenario == "onlyMock")
              onlyMock();
            else 
            if (scenario == "firstMock")
              firstMock();
          }

        
        /** @test complete use sequence: first access the Client Object, 
         *        then replace it by two different mocks, and finally
         *        restore the original Client Object 
         */
        void injectBoth ()
          {
            TargetObj* tartar = &instance();
            tartar->doIt();
            tartar->doIt();
            ASSERT (tartar->getCnt() == 2);
            
            instance.injectSubclass (new Mock_1);
            tartar = &instance();
            tartar->doIt();
            tartar->doIt();
            tartar->doIt();
            tartar->doIt();
            tartar->doIt();
            ASSERT (tartar->getCnt() == 5);
            
            instance.injectSubclass (new Mock_2);
            tartar = &instance();
            tartar->doIt();
            ASSERT (tartar->getCnt() == 1);
            
            instance.injectSubclass (0); // unshaddowing original instance
            tartar = &instance();
            ASSERT (tartar->getCnt() == 2);
            tartar->doIt();
            ASSERT (tartar->getCnt() == 3);
          }

        
        /** @test just use Singleton Factory normally without any Mock.
         */
        void noMock ()
          {
            TargetObj& tartar = instance();
            tartar.doIt();
          }

        
        /** @test inject the Mock prior to using the Singleton Factory,
         *        thus the original Client Object shouldn't be created. 
         */
        void onlyMock ()
          {
            instance.injectSubclass (new Mock_1);
            TargetObj& tartar = instance();
            tartar.doIt();
          }

        
        /** @test inject the Mock prior to using the Singleton Factory,
         *        but then reset the Mock, so following calls should
         *        create the original Client Object. 
         */
        void firstMock ()
          {
            instance.injectSubclass (new Mock_1);
            TargetObj* tartar = &instance();
            tartar->doIt();
            tartar->doIt();
            ASSERT (tartar->getCnt() == 2);
            
            instance.injectSubclass (0);
            tartar = &instance();
            tartar->doIt();
            ASSERT (tartar->getCnt() == 1);
          }
      };
    
      
    
    /** Register this test class... */
    LAUNCHER (SingletonTestMock_test, "unit common");
    
    
    
  } // namespace test

} // namespace cinelerra
