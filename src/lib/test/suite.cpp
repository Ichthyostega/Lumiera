/*
  Suite  -  helper class for running collections of tests

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


/** @file suite.cpp
 ** Implementation of a simple test runner / test suite framework for unit testing.
 */



#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/format-cout.hpp"
#include "lib/test/suite.hpp"
#include "lib/test/run.hpp"
#include "lib/cmdline.hpp"
#include "lib/random.hpp"
#include "lib/util.hpp"

#include <boost/algorithm/string.hpp>
#include <optional>
#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include <map>


namespace test {
  
  using std::map;
  using std::vector;
  using std::optional;
  using std::shared_ptr;
  using std::string_literals::operator ""s;
  using boost::algorithm::trim;
  
  using util::isnil;
  using util::contains;
  using util::toString;
  using util::typeStr;
  using lib::SeedNucleus;
  using lib::Random;
  
  typedef map<string, Launcher*> TestMap;
  typedef shared_ptr<TestMap>  PTestMap;
  typedef map<string,PTestMap> GroupMap;
  
  
  
  namespace {
    /**
     * helper to collect and manage the test cases.
     * Every testcase class should create a Launch instance,
     * which causes a call to Suite::enrol(), so we can add a pointer
     * to this Launcher into a map indexed by the provided testIDs and groupIDs.
     * This enables us to build a Suite instance for any requested group
     * and then instantiate and invoke individual testcases accordingly.
     */
    class Registry
      {
        GroupMap groups_;
        
      public:
        Registry() { };
        
        PTestMap&
        getGroup (string grpID)
          {
            return groups_[grpID];
          };
        
        void
        add2group (Launcher* test, string testID, string groupID)
          {
            REQUIRE( test );
            REQUIRE( !isnil(testID) );
            REQUIRE( !isnil(groupID) );
            
            PTestMap& group = getGroup(groupID);
            if (!group)
              group.reset( new TestMap );
            (*group)[testID] = test;
          }
      };
    
    class SuiteSeedNucleus
      : public SeedNucleus
      {
      public:
        /** optionally a fixed random seed to inject in each invoked test */
        opt_uint64 fixedSeed;
        
        uint64_t
        getSeed()  override
          {
            auto seed = fixedSeed? *fixedSeed : lib::entropyGen.u64();
            auto kind = fixedSeed? "!fix" : "rand";
            NOTICE (test, "     ++>>> SEED(%s) <<<: %s", kind, toString(seed).c_str());
            return seed;
          }
      };
    
    /* ===== global implementation state ===== */
    SuiteSeedNucleus suiteSeed;
    Registry testcases;
  }
  
  
  
  
  /** register the given test-launcher, so it can be later accessed
   *  either as a member of one of the specified groups, or directly
   *  by its testID. Any test is automatically added to the groupID
   *  #ALLGROUP
   *  @param test the Launcher object used to run this test
   *  @param testID unique ID to refer to this test (will be used as std::map key)
   *  @param groups List of group-IDs selected by whitespace
   */
  void
  Suite::enrol (Launcher* test, string testID, string groups)
  {
    REQUIRE( test );
    REQUIRE( !isnil(testID) );
    
    std::istringstream ss(groups);
    string group;
    while (ss >> group )
      testcases.add2group(test, testID, group);
    
    // Magic: always add any testcase to groupID="ALL"
    testcases.add2group(test,testID, ALLGROUP);
  }
  
  /** "magic" groupID containing all registered testcases */
  const string Suite::ALLGROUP = "ALL";
  
  /** exit code returned when any individual test threw */
  const int Suite::EXCEPTION_THROWN = 5;
  const int Suite::TEST_OK = 0;

  
  
  /** create a suite comprised of all the testcases
   *  previously @link #enrol() registered @endlink with this this group.
   *  @see #run() running tests in a Suite
   */
  Suite::Suite (string groupID, opt_uint64 optSeed)
    : groupID_(groupID)
    , exitCode_(0)
  {
    REQUIRE( !isnil(groupID) );
    TRACE(test, "Test-Suite( groupID=%s )\n", groupID.c_str () );
    
    // Seed random number generator
    std::srand (std::time (nullptr));
    
    suiteSeed.fixedSeed = optSeed;
    
    if (!testcases.getGroup(groupID))
      throw lumiera::error::Invalid ("empty testsuite");
  }
  
  
  int
  Suite::getExitCode ()  const
    {
      return exitCode_;
    }
  
  
  
#define IS_VALID(test,testID) \
  ASSERT ((test), "NULL testcase launcher for test '%s' found in testsuite '%s'", groupID_.c_str(),testID.c_str());
  
  
  namespace { // internal helper for launching with error logging
    
    int
    invokeTestCase (Test& theTest, Arg cmdline)
    {
      try {
          INFO (test, "++------------------- invoking TEST: %s", cStr(typeStr (theTest)));
          theTest.run (cmdline);
          return Suite::TEST_OK;
        }
      catch (lumiera::Error& failure)
        {
          lumiera_err errorID = lumiera_error(); // reset error flag
          cerr << "*** Test Failure " << theTest << endl;
          cerr << "***            : " << failure.what() << endl;
          ERROR (test,     "Error state %s", errorID);
          WARN  (progress, "Caught exception %s", failure.what());
          return Suite::EXCEPTION_THROWN;
    }   }
  }
  
  

  void
  Test::seedRand()
  {
    lib::defaultGen.reseed (suiteSeed);
  }
  
  
  Random
  Test::makeRandGen()
  {
    return Random{lib::seedFromDefaultGen()};
  }
  
  
  uint
  Test::firstVal (Arg arg, uint someNumber)
  {
    if (not isnil(arg))
      someNumber = boost::lexical_cast<uint> (arg[1]); // may throw
    return someNumber;
  }
  
  string
  Test::firstTok (Arg arg)
  {
    return isnil(arg)? util::BOTTOM_INDICATOR
                     : arg[1];
  }
  
  
  
  /** run all testcases contained in this Suite.
   *  The first argument in the commandline, if present,
   *  will select one single testcase with a matching ID.
   *  In case of invoking a single testcase, the given cmdline
   *  will be forwarded to the testcase, after removing the
   *  testcaseID from cmdline[0]. Otherwise, every testcase
   *  in this suite is invoked with a empty cmdline vector.
   *  @param cmdline ref to the vector of commandline tokens
   */
  bool
  Suite::run (Arg cmdline)
  {
    PTestMap tests = testcases.getGroup(groupID_);
    if (!tests)
      throw lumiera::error::Invalid ("No tests found for test group \""+groupID_+"\"");
    
    if (0 < cmdline.size())
      {
        string& testID (cmdline[0]);
        trim(testID);
        if ( contains (*tests, testID))
          {
            // first cmdline argument denotes a valid testcase registered in
            // this group: invoke just this test with the remaining cmdline
            Launcher* test = (*tests)[testID];
            IS_VALID (test,testID);
            
            // Special contract: in case the cmdline holds no actual arguments
            // beyond the test name, then it's cleared entirely.
            if (1 == cmdline.size()) cmdline.clear();                      // TODO this invalidates also testID -- really need to redesign the API ////TICKET #289
            
            exitCode_ |= invokeTestCase (*test->makeInstance(), cmdline);  // TODO confusing statement, improve definition of test collection datatype Ticket #289
            return true;
          }
        else
          throw lumiera::error::Invalid ("unknown test : "+testID);
      }
    
    // no test-ID was specified.
    // Instantiate all tests cases and execute them.
    for ( TestMap::iterator i=tests->begin(); i!=tests->end(); ++i )
      {
        cout << "\n  ----------"<< i->first<< "----------\n";
        Launcher* test = (i->second);
        IS_VALID (test, i->first);
        exitCode_ |= invokeTestCase (*test->makeInstance(), cmdline); // actually no cmdline arguments
      }
    return true;
  }
  
  
  /** print to stdout an enumeration of all testcases in this suite,
   *  in a format suitable for use with Cehteh's ./test.sh
   * @todo this function war rarely used and is kind of broken by the fact
   *  that Nobug-assertions terminate the program instead of throwing,
   *  rendering the feature to integrate output mostly useless.
   */
  void
  Suite::describe()
  {
    lib::Cmdline noCmdline("");
    PTestMap tests = testcases.getGroup(groupID_);
    ASSERT (tests);
    
    cout << "TESTING \"Component Test Suite: " << groupID_ << "\" ./test-components\n\n";
    
    for ( TestMap::iterator i=tests->begin(); i!=tests->end(); ++i )
      {
        string key (i->first);
        cout << "\n\n";
        cout << "TEST \""<<key<<"\" "<<key<<" <<END\n";
        Launcher* test = (i->second);
        IS_VALID (test, i->first);
        try
          {
            test->makeInstance()->run(noCmdline); // run it to insert test generated output
          }
        catch (...)
          {
            cout << "PLANNED ============= " << lumiera_error() << "\n";
          }
        cout << "END\n";
      }
  }
  
  
  
} // namespace test
