/*
  SUITE.hpp  -  helper class for running collections of tests
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#ifndef TESTHELPER_SUITE_H
#define TESTHELPER_SUITE_H

#include <vector>
#include <string>



namespace test
  {
  using std::string;
  
  // Forward decls needed for run.hpp
  class Test;
  class Launcher;

  typedef std::vector<string> & Arg; 
  
  
  /**
   * Enables running a collection of tests.
   * An internal registration service #enroll() is provided
   * for the individual Test - inscances to be recognized as 
   * testcases. The groupID passed to the constructor selects
   * all testcases declared as belonging to this Group.
   */
  class Suite
    {
      string groupID_;
      
    public:
      Suite (string groupID);
      void run (Arg cmdline);
      void describe ();
      static void enroll (Launcher *test, string testID, string groups);
      
      static const string ALLGROUP;
    };
  
  
} // namespace test
#endif
