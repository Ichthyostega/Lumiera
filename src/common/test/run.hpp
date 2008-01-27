/*
  RUN.hpp  -  helper class for grouping, registering and invoking testcases
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
                        Hermann Vosseler <Ichthyostega@web.de>
 
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
 
*/


#ifndef TESTHELPER_RUN_H
#define TESTHELPER_RUN_H

#include "pre.hpp"


#include "nobugcfg.h"

#include "common/test/suite.hpp"
#include "common/util.hpp"

#include <vector>
#include <string>


namespace test
  {

  using std::string;
  using std::auto_ptr;

  typedef std::vector<string> & Arg; 
  
  
  
  /**
   * Abstract Base Class for all testcases.
   * Typically, such testcases are created by a Launcher.
   */
  class Test
    {
    public:
      virtual ~Test() {}
      virtual void run(Arg arg) = 0;
    };
    
    
    
  /** interface: generic testcase creating functor. */  
  class Launcher
    {
    public:
      virtual ~Launcher() {}
      virtual auto_ptr<Test> operator() () = 0;
    };
    
  
  /**
   * Helper class for running a collection of tests.
   * Launch objects are functors, which create on
   * invocation an instance of the Test class
   * they were created with. Creating such a
   * Test Launcher internally registers this
   * testcase with the Testsuite-Class,
   * optionally under several groups
   * (=categories, suite selections).
   * @note a subclass of Launcher
   */
  template<class TEST>
  class Launch : public Launcher
    {
    public:
      Launch (string testID, string groups)  { Suite::enroll (this,testID,groups); };
      virtual auto_ptr<Test> operator() ()  { return auto_ptr<Test> (new TEST ); };
    };   
    
} // namespace test

// make them global for convienience 
using ::test::Arg;
using ::test::Test;
using ::test::Launch;

// and provide shortcut for registration
#define LAUNCHER(_TEST_CLASS_, _GROUPS_) \
  /** Register _TEST_CLASS_ to be invoked in some test suites (groups) _GROUPS_ */    \
  Launch<_TEST_CLASS_> run_##_TEST_CLASS_##_(STRINGIFY(_TEST_CLASS_), _GROUPS_); 


#endif
