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

#include "common/cmdline.hpp"

#include <string>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/utility.hpp>



namespace test
  {
  using std::string;
  using std::ostream;
  

  
  /**
   * Support for selecting and configuring testcases
   * via commandline arguments. A preconfigured wrapper
   * around boost::program_options, with the ability
   * to tolerate unknown options. The commandline
   * to be parsed is taken wrapped into a Cmdline
   * instance; after parsing this commandline
   * vector will contain only the remaining
   * unrecognized parts.
   */
  class TestOption : private boost::noncopyable
    {
    public:
      TestOption (util::Cmdline& cmdline);
      const string getTestgroup ();
      const string getTestID ();
      const bool getDescribe ();
      
    private:
      boost::program_options::options_description syntax;
      boost::program_options::variables_map parameters;
      
      friend ostream& operator<< (ostream&, const TestOption&);
    };
  
  /** for outputting the help messages. */
  ostream& operator<< (ostream& os, const TestOption& to);
 
  
} // namespace test
#endif
