/*
  Factory(Test)  -  unittest for the object creating factory
 
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


#include "common/testtargetobj.hpp"

#include "common/test/run.hpp"
#include "common/factory.hpp"
#include "common/util.hpp"

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
     * Target object to be created by the Test-Factory.
     * Allocates a variable amount of additional heap memory
     * and prints diagnostic messages. Note we provide a
     * static member TargetObj::create  for the client 
     * code to generate smart ptr wrapped instances.
     */
    class TargetObj : public TestTargetObj
      {
      public:
        TargetObj (uint cnt) : TestTargetObj(cnt) {}
        
        static ObjFactory create;
      };
    
    
    
    /** Test-Factory specialized to create TargetObj instances
     *  using the 1-argument constructor TargetObj::TargetObj(int).
     *  It will create boost::shared_ptr instances, because 
     *  factory::RefcountPtr was parametrized with this smart pointer type.
     */ 
    class ObjFactory : public factory::RefcountPtr<TargetObj>
      {
      public:
        /** specialized Factory method for creating TargetObj instances.
         *  Here, we invoke a special constructor, but basically we could
         *  do everything we want, creating instances of sub classes,
         *  registering objects etc. Further, we could have used a
         *  custom allocator or a special deleter function. 
         */
        PType operator() (uint param) { return wrap (new TargetObj(param) ); }
        
      };
    
    
    /** shorthand for the created smart-pointer class, 
     *  here it's a (refcounting) boost::shared_ptr
     */
    typedef ObjFactory::PType pTarget;

    ObjFactory TargetObj::create;
    
    
    
    
    
    
    /*******************************************************************
     * @test the basic object creation Factory behaviour: We declared
     *       a static field TargetObj::create to be a ObjFactory. So,
     *       by invoking this functor, we get a boost::shared_ptr
     *       wrapping a new TargetObj instance. From this we copy
     *       further shared-ptrs, invoke a member function and
     *       finally, when leaving the scope, our TargetObj
     *       will be destroyed automatically.
     * @see  cinelerra::Factory
     */
    class Factory_test : public Test
      {
        virtual void run(Arg arg) 
          {
            uint num= isnil(arg)? 1 : lexical_cast<uint>(arg[1]);
            
            pTarget p1 (TargetObj::create (num));
            pTarget p2 (p1);
            pTarget p3 = p2;
            
            cout << "now the smart-ptr has refcount=" << p1.use_count() << "\n" 
                 << string (*p3) << "\n";
          } 
      };
    
    
    /** Register this test class... */
    LAUNCHER (Factory_test, "unit common");
    
    
    
  } // namespace test

} // namespace cinelerra
