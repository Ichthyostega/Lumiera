/*
  Singleton(Test)  -  unittest for our Singleton template
 
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

#include "testtargetobj.hpp"
#include "lib/singleton.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <iostream>

using boost::lexical_cast;
using boost::format;
using util::isnil;
using std::string;
using std::cout;


namespace lumiera
  {
  namespace test
    {
    

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
        
        friend class singleton::StaticCreate<TargetObj>;
        friend class singleton::HeapCreate<TargetObj>;
      };
    
    int TargetObj::cnt = 0;
    
    
    
    
    
    
    
    /*******************************************************************
     * @test implement a Singleton class using our Singleton Template.
     * Expected results: no memory leaks.
     * @see  lumiera::Singleton
     * @see  lumiera::singleton::StaticCreate
     * @see  lumiera::singleton::HeapCreate
     */
    class Singleton_test : public Test
      {
        typedef boost::function<TargetObj& ()> InstanceAccessFunc;
        InstanceAccessFunc instance;
        
        virtual void run(Arg arg) 
          {
            uint num= isnil(arg)? 1 : lexical_cast<uint>(arg[1]);
            
            testStaticallyAllocatedSingleton (num++);
            testHeapAllocatedSingleton (num++);
          }

        
        /** @test parametrize the Singleton creation such as to create 
         *        the single TargetObj instance as a static variable. 
         */
        void testStaticallyAllocatedSingleton (uint num)
          {
            SingletonFactory<TargetObj> single;
            instance = single;
            useInstance (num, "statically allocated");
          }
        
        /** @test parametrize the Singleton creation such as to create 
         *        the single TargetObj instance allocated on the Heap
         *        and deleted automatically at application shutdown.
         */
        void testHeapAllocatedSingleton (uint num)
          {
            SingletonFactory<TargetObj,singleton::HeapCreate> single;
            instance = single;
            useInstance (num, "heap allocated");
          }
        
        
        
        void useInstance (uint num, string kind)
          {
            cout << format("testing TargetObj(%d) as Singleton(%s)\n") % num % kind;
            TargetObj::setCountParam(num);
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

} // namespace lumiera
