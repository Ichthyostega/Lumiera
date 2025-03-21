/*
  TESTOPTION.hpp  -  handle cmdline for invoking Testsuite

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file testoption.hpp
 ** Commandline options for our unittest test-suite executable.
 */


#ifndef TESTHELPER_TESTOPTION_H
#define TESTHELPER_TESTOPTION_H

#include "lib/cmdline.hpp"
#include "lib/nocopy.hpp"

#include <string>
#include <iostream>
#include <optional>
#include <boost/program_options.hpp>
#include <boost/utility.hpp>



namespace test {
  
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
   * unrecognised parts.
   */
  class TestOption
    : util::NonCopyable
    {
    public:
      TestOption (lib::Cmdline& cmdline);
      const string getTestgroup();
      const string getTestID();
      bool handleHelpRequest();
      bool shouldDescribe();
      
      std::optional<uint64_t> optSeed();
      
    private:
      boost::program_options::options_description syntax;
      boost::program_options::variables_map parameters;
      
      friend ostream& operator<< (ostream&, const TestOption&);
    };
  
    
  /** for outputting the help messages. Forward accumulated 
   *  help messages from all contained option definitions */
  ostream& operator<< (ostream& os, const TestOption& to);
 
  
} // namespace test
#endif
