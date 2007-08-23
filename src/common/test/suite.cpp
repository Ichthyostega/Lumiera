/*
  Suite  -  helper class for running collections of tests
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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


#include <map>
#include <vector>
#include <memory>
#include <tr1/memory>
#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include "common/cmdline.hpp"
#include "common/test/suite.hpp"
#include "common/test/run.hpp"
#include "common/error.hpp"
#include "common/util.hpp"

#include "nobugcfg.h"  


namespace test
  {
  using std::map;
  using std::vector;
  using std::auto_ptr;
  using std::tr1::shared_ptr;
  using boost::algorithm::trim;

  using util::isnil;
  using util::contains;
  
  typedef map<string, Launcher*> TestMap;
  typedef shared_ptr<TestMap>  PTestMap;
  typedef map<string,PTestMap> GroupMap;

  
  
  /** helper to collect and manage the test cases.
   *  Every testcase class should create a Launch instance
   *  which causes a call to Suite::enroll(), so we can add a
   *  pointer to this Launcher into a map indexed by the
   *  provided testIDs and groupIDs.
   *  This enables us to build a Suite instance for any 
   *  requested group and then instantiiate and invoke
   *  individual testcases accordingly.
   */ 
  class Registry
    {
      auto_ptr<GroupMap> groups;
    public:
      Registry() : groups(new GroupMap ) {};
      PTestMap& getGroup (string grpID) { return (*groups)[grpID]; }; 
      void add2group (Launcher* test, string testID, string groupID);
    };
    
  void 
  Registry::add2group (Launcher* test, string testID, string groupID)
    {
      REQUIRE( test );
      REQUIRE( !isnil(testID) );
      REQUIRE( !isnil(groupID) );
      
      PTestMap& group = getGroup(groupID);
      if (!group)
        group.reset( new TestMap );
      (*group)[testID] = test;
    }
  
  Registry testcases;
  
  
  
  
  /** register the given test-launcher, so it can be later accessed
   *  either as a member of one of the specified groups, or direcly
   *  by its testID. Any test is automatically added to the groupID
   *  #ALLGROUP
   *  @param groups List of group-IDs selected by whitespace
   */
  void 
  Suite::enroll (Launcher* test, string testID, string groups)
    {
      REQUIRE( test );
      REQUIRE( !isnil(testID) );
      
      std::istringstream ss(groups);
      string group;
      while (ss >> group )
        testcases.add2group(test, testID, group);
      
      // Magic: allways add any testcas to groupID="ALL"
      testcases.add2group(test,testID, ALLGROUP);
    }
  
  /** "magic" groupID containing all registered testcases */
  const string Suite::ALLGROUP = "ALL";
  
  
  
  /** create a suite comprised of all the testcases 
   *  previously @link #enroll() registered @endlink with this
   *  this group. 
   *  @see #run() running tests in a Suite 
   */
  Suite::Suite(string groupID) 
    : groupID_(groupID)
    {
      REQUIRE( !isnil(groupID) );
      TRACE(test, "Test-Suite( groupID=%s )\n", groupID.c_str () );
      
      if (!testcases.getGroup(groupID))
        throw cinelerra::error::Invalid ();
        //throw "empty testsuite";     /////////// TODO Errorhandling!
    }
    
#define VALID(test,testID) \
  ASSERT ((test), "NULL testcase laucher for test '%s' found in testsuite '%s'", groupID_.c_str(),testID.c_str());
  
  
  /** run all testcases contained in this Suite.
   *  The first argument in the commandline, if present, 
   *  will select one single testcase with a matching ID.
   *  In case of invoking a single testcase, the given cmdline
   *  will be forwarded to the testcase, after removind the
   *  testcaseID from cmdline[0]. Otherwise, every testcase 
   *  in this suite is invoked with a empty cmdline vector.
   *  @param cmdline ref to the vector of commandline tokens  
   */
  void 
  Suite::run (Arg cmdline)
    {
      PTestMap tests = testcases.getGroup(groupID_);
      if (!tests)
        throw cinelerra::error::Invalid (); ///////// TODO: pass error description
      
      if (0 < cmdline.size())
        {
          string& testID (cmdline[0]);
          trim(testID);
          if ( contains (*tests, testID))
            {
              // first cmdline argument denotes a valid testcase registered in 
              // this group: invoke just this test with the remaining cmdline
              Launcher* test = (*tests)[testID];
              cmdline.erase (cmdline.begin());
              VALID (test,testID);
              (*test)()->run(cmdline);
              return;
        }   }
      
      // no test-ID was specified.
      // instantiiate all tests cases and execute them.
      for ( TestMap::iterator i=tests->begin(); i!=tests->end(); ++i )
        {
          std::cout << "\n  ----------"<< i->first<< "----------\n";
          Launcher* test = (i->second);
          VALID (test, i->first);
          (*test)()->run(cmdline); // actually no cmdline arguments
        }
    }
  
  
  /** print to stdout an ennumeration of all testcases in this suite,
   *  in a format suitable for use with Cehteh's ./test.sh 
   */
  void
  Suite::describe ()
    {
      util::Cmdline noCmdline("");
      PTestMap tests = testcases.getGroup(groupID_);
      ASSERT (tests);
      
      std::cout << "TESTING \"Component Test Suite: " << groupID_ << "\" ./test-components\n\n";

      for ( TestMap::iterator i=tests->begin(); i!=tests->end(); ++i )
        {
          string key (i->first);
          std::cout << "\n\n";
          std::cout << "TEST \""<<key<<"\" "<<key<<" <<END\n";
          Launcher* test = (i->second);
          VALID (test, i->first);
          (*test)()->run(noCmdline); // run it to insert test generated output
          std::cout << "END\n";
        }

      

    }



} // namespace test
