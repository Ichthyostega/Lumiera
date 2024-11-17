/*
  HelloWorld(Test)  -  how to use this test framework...

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file helloworldtest.cpp
 ** unit test \ref HelloWorld_test
 */


#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>

using boost::lexical_cast;
using util::isnil;
using std::cout;
using std::endl;


namespace lumiera{
namespace test   {
  
  /**************************************//**
   * Hellooooooo the world is just a test
   * @test demo of using the test framework
   */
  class HelloWorld_test : public Test
    {
      virtual void
      run (Arg arg)
        {
          uint num{firstVal (arg)};
          for ( ; 0 < num-- ; )
            greeting();
        }
      
      void
      greeting()
        {
          cout << "This is how the world ends..." <<endl;
        }
    };
  
  
  
  
  
  /** Register this test class to be invoked in some test groups (suites) 
   * @remarks this macro \c LUNCHER is defined in run.hpp to simplify
   *          the registration of test classes. It expands to the
   *          following static variable definition
   *          \code
   *          Launch<HelloWorld_test> run_HelloWorld_test("HelloWorld_test","unit common");
   *          \endcode
   */
  LAUNCHER (HelloWorld_test, "unit common");
  
  
  
}} // namespace lumiera::test
