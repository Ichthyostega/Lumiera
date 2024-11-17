/*
  Testrunner  -  execute a suite of test objects, possibly filtered by category

   Copyright (C)
     2007,2008,       Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file testrunner.cpp
 ** Lumiera unit test suite
 ** A simple test runner application.
 ** The build system will link the individual shared libraries with the test code
 ** together with this translation unit into the executable `target/test-suite`
 */

#include "include/lifecycle.h"
#include "lib/test/testoption.hpp"
#include "lib/test/suite.hpp"

using lumiera::LifecycleHook;
using lumiera::ON_GLOBAL_INIT;
using lumiera::ON_GLOBAL_SHUTDOWN;


/** run all tests or any single test specified in the first
 *  command line argument.
 *  @return exit code signalling if any exception was caught
 *          at the level of individual test cases
 *  @throws any further exceptions raised outside of test cases
 *  @throws any library and external exceptions (not lumiera::Exception)  
 */
int main (int argc, const char* argv[])
{
  lib::Cmdline args (argc,argv);
  test::TestOption optparser (args);
  test::Suite suite (optparser.getTestgroup(), optparser.optSeed());
  LifecycleHook::trigger (ON_GLOBAL_INIT);
  
  if (optparser.shouldDescribe())
    suite.describe();
  else
    optparser.handleHelpRequest() || suite.run (args);

  LifecycleHook::trigger (ON_GLOBAL_SHUTDOWN);
  return suite.getExitCode();
}
