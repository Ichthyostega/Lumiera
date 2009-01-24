/*
  SingletonSubclass(Test)  -  actually creating a subclass of the Singleton Type
 
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
#include "lib/singletonsubclass.hpp"

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
    class Interface : public TestTargetObj
      {
      public:
        static int cnt;
        static void setCountParam (uint c) { Interface::cnt = c; }
        
        virtual string identify()          { return "Interface"; }
        
      protected:
        Interface () : TestTargetObj(cnt) {}
        virtual ~Interface() {}
        
        friend class singleton::StaticCreate<Interface>;
        friend class singleton::HeapCreate<Interface>;
      };
    
    int Interface::cnt = 0;
    
    
    class Impl : public Interface
      {
      public:
        virtual string identify() { return "Implementation"; }
      };
    

    // for checking the safety.....
    class Impl_XXX : public Impl { };
    class Unrelated { };
    
    
    
    
    /*******************************************************************
     * @test specialised variant of the Singleton Factory, for creating
     *       subclasses (implementation classes) without coupling the 
     *       caller to the concrete class type.
     * Expected results: an instance of the subclass is created.
     * @see  lumiera::Singleton
     * @see  lumiera::SingletonSubclassFactory
     * @see  lumiera::singleton::Adapter
     */
    class SingletonSubclass_test : public Test
      {
        
        virtual void run(Arg arg) 
          {
            uint num= isnil(arg)? 1 : lexical_cast<uint>(arg[1]);
            
            cout << format("using the Singleton should create TargetObj(%d)...\n") % num;
            
            Interface::setCountParam(num);
            
            // marker to declare the concrete type to be created
            singleton::UseSubclass<Impl> typeinfo;                       
            
            // define an instance of the Singleton factory,
            // Specialised to create the concrete Type passed in
            SingletonSubclassFactory<Interface> instance (typeinfo);
            
            // Now use the Singleton factory...
            // Note: we get the Base type
            Interface& t1 = instance();
            Interface& t2 = instance();
            
            ASSERT ( &t1 == &t2, "not a Singleton, got two different instances." ); 
            
            cout << "calling a non-static method on the Singleton-" 
                 << t1.identify() << "\n"
                 << string (t1)   << "\n";
            
#ifdef DEBUG
            /////////////////////////////////////////////////////////////////////////////////////////////TODO: better way of detecting debug builds
            verify_error_detection ();
#endif
          } 
        
        
        
        void verify_error_detection ()
          {
            
            singleton::UseSubclass<Impl_XXX> more_special_type;                       
            
            try
              {
                SingletonSubclassFactory<Interface> instance (more_special_type);
                cout << "was able to re-configure the SingletonSubclassFactory "
                        "with another type. This should be detected in debug mode\n";
              }
            catch (...)
              {
                ASSERT (lumiera_error () == error::LUMIERA_ERROR_ASSERTION);
              }

            
            // Note: the following won't compile, because the "subclass" isn't a subclass...
            //
            // singleton::UseSubclass<Unrelated> yet_another_type;                       
            // SingletonSubclassFactory<Interface> instance (yet_another_type);
          }
      };
    
      
    
    /** Register this test class... */
    LAUNCHER (SingletonSubclass_test, "unit common");
    
    
    
  } // namespace test

} // namespace lumiera
