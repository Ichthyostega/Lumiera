/*
  TESTOPTION.hpp  -  handle cmdline for invoking Testsuite
 
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
 
*/


#ifndef TESTHELPER_TESTOPTION_H
#define TESTHELPER_TESTOPTION_H

#include <string>
#include <vector>
#include <boost/program_options.hpp>



namespace test
  {
  using std::string;
  using std::vector;
  
  typedef boost::program_options::variables_map VarMap;

  
  /**
   * Support for selecting and configuring testcases
   * via commandline arguments. A preconfigured wrapper
   * around boost::program_options, with the ability
   * to tolerate unknown options and get an vector<string>
   * of everything remaining in the commandline after
   * parsing the known options.
   */
  class TestOption
    {
    public:
      TestOption (int argc, const char* argv[]);
      explicit TestOption (string cmdline);
      const string& getTestgroup ();
      const string& getTestID ();
      vector<string>& remainingCmdline ();
      
      operator string const ();
      
    private:
      VarMap vm;
      vector<string> cmdline;
      
      void parseOptions (int argc, const char* argv[]);
    };
  
  
  
} // namespace test
#endif
