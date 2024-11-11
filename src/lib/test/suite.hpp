/*
  SUITE.hpp  -  helper class for running collections of tests

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

*/

/** @file suite.hpp
 ** Building and running a suite of tests, implemented as test classes.
 ** This simple test suite runner is intended to be linked into a standalone C++ application,
 ** allowing to invoke individual tests by ID, invoking groups of tests and producing a
 ** report of all registered tests. Registration of individual testcases happens 
 ** automatically through static test::Launcher instances.
 ** 
 ** @todo as of 9/09, the implementation seems OKish but a bit clumsy. See Ticket #289 
 ** 
 ** @see HelloWorld_test
 ** @see test::Test
 ** @see test::TestOption
 ** @see run.hpp
 ** @see testrunner.cpp
 */


#ifndef TESTHELPER_SUITE_H
#define TESTHELPER_SUITE_H

#include <vector>
#include <string>
#include <optional>



namespace test {
  
  using std::string;
  using opt_uint64 = std::optional<uint64_t>;
  
  // Forward declarations for run.hpp
  class Test;
  class Launcher;
  
  typedef std::vector<string> & Arg; 
  
  
  /**
   * Enables running a collection of tests.
   * An internal registration service #enrol() is provided
   * for the individual Test - instances to be recognised as
   * testcases. The groupID passed to the constructor selects
   * all testcases declared as belonging to this Group.
   */
  class Suite
    {
      string groupID_;
      int exitCode_;
      
    public:
      Suite (string groupID, opt_uint64 seed);
      bool run (Arg cmdline);
      void describe ();
      int getExitCode ()  const;
      static void enrol (Launcher *test, string testID, string groups);
      
      static const string ALLGROUP;
      static const int TEST_OK;
      static const int EXCEPTION_THROWN;
    };
  
  
} // namespace test
#endif
