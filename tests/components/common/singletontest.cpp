/*
  Singleton(Test)  -  unittest for our Singleton template
 
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


#include "common/testtargetobj.hpp"

#include "common/test/run.hpp"
#include "common/singleton.hpp"
#include "common/util.hpp"
#include "nobugcfg.h"

#include <boost/lexical_cast.hpp>
#include <iostream>

using boost::lexical_cast;
using util::isnil;
using std::string;
using std::cout;


namespace cinelerra
  {
  namespace test
    {
    
    class ObjFactory;
    

    /**
     * Target object to be instantiated as Singleton
     * Allocates a variable amount of additional heap memory
     * and prints diagnostic messages.
     */
    class TargetObj : public TestTargetObj
      {
      public:
        static int cnt;
        static void setCountParam (uint c) { TargetObj::cnt = c; }
      protected:
        TargetObj () : TestTargetObj(cnt) {}
      };
    
    int TargetObj::cnt = 0;
    
    
    
    
    
    
    
    /*******************************************************************
     * @test implement a Singleton class using our Singleton Template.
     * Expected results: no memory leaks.
     * @see  cinelerra::Singleton
     * @see  cinelerra::singleton::Static
     * @see  cinelerra::singleton::Heap
     */
    class Singleton_test : public Test
      {
        typedef TargetObj& (*InstanceAccessFunc) (void);
        InstanceAccessFunc instance;
        
        virtual void run(Arg arg) 
          {
            uint num= isnil(arg)? 1 : lexical_cast<uint>(arg[1]);
            TargetObj::setCountParam(num);
            
            testStaticallyAllocatedSingleton ();
            testHeapAllocatedSingleton ();
          }

        
        /** @test parametrize the Singleton creation such as to create 
         *        the single TargetObj instance as a static variable. 
         */
        void testStaticallyAllocatedSingleton ()
          {
            instance = &Singleton<TargetObj>::instance;
            useInstance ();
          }
        
        /** @test parametrize the Singleton creation such as to create 
         *        the single TargetObj instance allocated on the Heap
         *        and deleted automatically at application shutdown.
         */
        void testHeapAllocatedSingleton ()
          {
            instance = &Singleton<TargetObj,singleton::Heap>::instance;
            useInstance ();
          }
        
        
        
        void useInstance ()
          {
            TargetObj& t1 = instance();
            TargetObj& t2 = instance();
            
            ASSERT ( &t1 == &t2, "not a Singleton, got two different instances." ); 
            
            cout << "calling a non-static method on the Singleton instance\n" 
                 << string (t1) << "\n";
          } 
      };
    
      
    
    /** Register this test class... */
    LAUNCHER (Singleton_test, "unit common");
    
    
    
  } // namespace test

} // namespace cinelerra
