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
#include "test/helper/suite.hpp"
#include "test/helper/run.hpp"

#include <iostream>   /////////////////////////TODO: Debug
#include <sstream>

namespace test
  {
  using std::map;
  using std::vector;
  using std::auto_ptr;
  using std::tr1::shared_ptr;
  
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
    
  void Registry::add2group (Launcher* test, string testID, string groupID)
    {
      // TODO: ASSERT test!=null, testID.length > 0 ...
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
  void Suite::enroll (Launcher* test, string testID, string groups)
    {
      // TODO learn to use NoBug for logging 
      std::cerr << "enroll( testID=" << testID << ")\n";
      // TODO: ASSERT test!=null, testID.length() > 0...
      
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
      std::cerr << "Suite( groupID="<< groupID << ")\n";
      if (!testcases.getGroup(groupID))
        throw "empty testsuite";     /////////// TODO Errorhandling!
    }
    
  
  /** run all testcases contained in this Suite.
   *  The first argument in the commandline, if present, will select
   *  one single testcase with a matching ID. 
   */
  void Suite::run (int argc, char* argv[])
    {
      /////////////////////////////////////////////////////TODO:DEBUG
      std::cerr << "Suite::run( (" << argc << "[" ;
      for ( int i=0; i<argc; ++i )
        std::cerr << argv[i] << ",";
      std::cerr << "]\n";
      /////////////////////////////////////////////////////TODO:DEBUG
      
      PTestMap tests = testcases.getGroup(groupID_);
      //TODO ASSERT tests!=null
      
      if (argc >= 2)
        {
          if (Launcher* test = (*tests)[argv[1]])
            {
              // first cmdline argument denotes a valid
              // testcase registered in this group: 
              // go ahead and invoke just this test.
              if (argc > 2)
                { // pass additional cmdline as vector
                  vector<string> arglist(argc-2);
                  for ( int i=2; i<argc; ++i )
                    arglist.push_back(string(argv[i]));
                  
                  // run single Testcase with provided arguments
                  (*test)()->run(&arglist);
                }
              else
                (*test)()->run(0); // without additional argumens
              
              return;
            }
        }
      
      // no test-ID was specified.
      // instantiiate all tests cases and execute them.
      for ( TestMap::iterator i=tests->begin(); i!=tests->end(); ++i )
        if (i->second)
          {
            std::cout << "  ----------"<< i->first<< "----------\n";
            Launcher& test = *(i->second);
            test()->run(0); // without cmdline arguments
          }
      
    }



} // namespace test
