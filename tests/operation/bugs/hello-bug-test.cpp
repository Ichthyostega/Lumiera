/*
  HelloBug(Test)  -  placeholder for running bug regression tests

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file hello-bug-test.cpp
 ** unit test \ref HelloBug_test
 */


#include "lib/test/run.hpp"

#include <iostream>

using std::cerr;
using std::endl;


namespace lumiera{
namespace test   {
  
  
  /**************************************//**
   * @todo come up with some more bugs
   * @test cover known regressions
   */
  class HelloBug_test
    : public Test
    {
      virtual void
      run (Arg)
        {
          cerr << "hello sunshine, no bugs whatsoever" <<endl;
        }
    };
  
  
  
  
  
  /** Register this test class... */
  LAUNCHER (HelloBug_test, "function regression");
  
  
  
}} // namespace lumiera::test
