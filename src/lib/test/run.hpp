/*
  RUN.hpp  -  helper class for grouping, registering and invoking testcases

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>
                        Hermann Vosseler <Ichthyostega@web.de>

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

/** @file run.hpp
 ** Simplistic test class runner. Allows for writing unit tests as subclass of
 ** test::Test . They may be installed for automatic invocation through test::Suite
 ** by defining a Launcher instance, which can be done conveniently by the macro LAUNCHER
 ** 
 ** @see HelloWorld_test
 ** @see test::Suite
 ** @see testrunner.cpp
 ** @see random.hpp
 ** @see main.cpp
 */


#ifndef TESTHELPER_RUN_H
#define TESTHELPER_RUN_H

#include "lib/error.hpp"
#include "lib/random.hpp"

#include "lib/test/suite.hpp"
#include "lib/util.hpp"

#include <memory>
#include <string>


namespace test {
  
  using std::string;
  using std::shared_ptr;
  
  using Arg = std::vector<string> &;
  
  
  
  /**
   * Abstract Base Class for all testcases.
   * Typically, such testcases are created by a Launcher.
   */
  class Test
    {
    public:
      virtual ~Test()            = default;             ///< this is an interface
      virtual void run(Arg arg)  = 0;
      
      void seedRand();                                  ///< draw a new random seed from a common nucleus, and re-seed the default-Gen.
      lib::Random makeRandGen();                        ///< build a dedicated new RandomGen, seeded from the default-Gen
      static string firstTok (Arg);                     ///< conveniently pick the first token from the argument line
      static uint   firstVal (Arg, uint =1);            ///< conveniently use some number given as argument, with optional default
    };
  
  
  
  /** interface: generic testcase creating functor. */
  class Launcher
    {
    public:
      virtual ~Launcher()    = default;
      virtual shared_ptr<Test> makeInstance()  =0;
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
      Launch (string testID, string groups)
        {
          Suite::enrol (this,testID,groups);
        }
      
      virtual shared_ptr<Test>
      makeInstance ()  override
        {
          return shared_ptr<Test> (new TEST );
        }
    };
    
} // namespace test

// make those global for convenience....
using ::test::Arg;
using ::test::Test;
using ::test::Launch;
using lib::rani;
using lib::ranHash;
using lib::ranRange;
using lib::ranNormal;
using lib::defaultGen;

// and provide shortcut for registration
#define LAUNCHER(_TEST_CLASS_, _GROUPS_) \
  /** Register _TEST_CLASS_ to be invoked in some test suites (groups) _GROUPS_ */  \
  Launch<_TEST_CLASS_> run_##_TEST_CLASS_##_(STRINGIFY(_TEST_CLASS_), _GROUPS_);


#endif /*TESTHELPER_RUN_H*/
