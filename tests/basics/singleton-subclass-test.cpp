/*
  SingletonSubclass(Test)  -  actually creating a subclass of the Singleton Type

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

/** @file singleton-subclass-test.cpp
 ** unit test \ref SingletonSubclass_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include "test-target-obj.hpp"
#include "lib/depend.hpp"
#include "lib/depend-inject.hpp"

#include <boost/lexical_cast.hpp>

using boost::lexical_cast;
using util::isSameObject;
using util::_Fmt;
using util::isnil;
using std::string;


namespace lib {
namespace test{
  
  using LERR_(LIFECYCLE);
  
  
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
      
      friend class lib::DependencyFactory<Interface>;
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
  
  
  
  
  /***************************************************************//**
   * @test specific dependency-injection setup, to create a singleton
   *       subclass (implementation class) instance, without coupling
   *       the caller to the concrete type.
   * @remark Expected results: an instance of the subclass is created.
   * @see  lib::Depend
   * @see  lib/depend-inject.hpp
   */
  class SingletonSubclass_test : public Test
    {
      
      virtual void
      run(Arg arg)
        {
          uint num= isnil(arg)? 1 : lexical_cast<uint>(arg[1]);
          
          cout << _Fmt("using the Singleton should create TargetObj(%d)...\n") % num;
          
          Interface::setCountParam(num);
          
          // configuration to use the subclass on demand
          DependInject<Interface>::useSingleton<Impl>();
          
          // define an instance of the Singleton factory as always...
          Depend<Interface> instance;
          
          // Now use the Singleton factory...
          // Note: we get the Base type
          Interface& t1 = instance();
          Interface& t2 = instance();
          
          CHECK (isSameObject (t1, t2), "not a Singleton, got two different instances." );
          CHECK ( INSTANCEOF (Impl,&t1));         // got the subclass as expected
          CHECK ("Implementation" == t2.identify());
          
          cout << "calling a non-static method on the Singleton-"
               << t1.identify() << endl
               << t1            << endl;
          
          verify_error_detection ();
        }
      
      
      
      void
      verify_error_detection ()
        {
          VERIFY_ERROR (LIFECYCLE, DependInject<Interface>::useSingleton<Impl_XXX>() );
          
          Depend<Interface> newFactory;
          CHECK ( INSTANCEOF (Impl, &newFactory() )); // works as before
          
//////////does not compile due to incompatible baseclass
//        DependInject<Interface>::useSingleton<Unrelated>();
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (SingletonSubclass_test, "unit common");
  
  
  
}} // namespace lib::test
