/*
  Singleton(Test)  -  unittest for our Singleton template

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file singleton-test.cpp
 ** unit test \ref Singleton_test
 */


#include "lib/test/run.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include "test-target-obj.hpp"
#include "lib/depend.hpp"

#include <boost/lexical_cast.hpp>

using boost::lexical_cast;
using util::isSameObject;
using util::_Fmt;
using util::isnil;
using std::string;


namespace lib {
namespace test{
  
  
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
      
      friend class lib::DependencyFactory<TargetObj>;
    };
  
  int TargetObj::cnt = 0;
  
  
  
  
  
  
  
  /***************************************************************//**
   * @test implement a Singleton class using our Singleton Template.
   * Expected results: single instance created in static memory,
   * single instance properly destroyed, no memory leaks.
   * @see  lib::Depend
   * @see  lib::DependencyFactory::InstanceHolder
   */
  class Singleton_test : public Test
    {
      
      virtual void
      run (Arg arg)
        {
          uint num{firstVal (arg)};
          
          Depend<TargetObj> singleton;
          
          cout << _Fmt("testing TargetObj(%d) as Singleton\n") % num;
          TargetObj::setCountParam(num);
          TargetObj& t1 = singleton();
          TargetObj& t2 = singleton();
          
          CHECK (isSameObject(t1, t2), "not a Singleton, got two different instances." );
          
          cout << "calling a non-static method on the Singleton instance" <<endl
               << t1 << endl;
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (Singleton_test, "unit common");
  
  
  
}} // namespace lib::test
