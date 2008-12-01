/*
  Option  -  handle cmdline for starting the Lumiera application
 
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
 
* *****************************************************/


#include "lumiera/option.hpp"

#include "include/error.hpp"



typedef boost::program_options::options_description Syntax;
typedef boost::program_options::variables_map VarMap;

namespace op = boost::program_options;

using util::VectS;

namespace lumiera {
  
  
  /** set up an options parser to use the application commandline.
   *  Recognises the following options
   *  \code
   *  --help
   *  \endcode
   *  @todo describe the actual options
   */
  Option::Option (util::Cmdline& cmdline)
    : syntax("Run a collection of test cases. Supported parameters"),
      parameters()
    {
      syntax.add_options()
        ("help,h",      "produce help message")
        ("group,g",     op::value<string>()->default_value(Suite::ALLGROUP),
                        "the group (selection) of testcases to execute")
        ("describe",    op::bool_switch(),
                        "enumerate all testcases in this Suite in a format usable with ./test.sh.")
        ("id",          op::value<VectS>(),
                        "an individual testcase to be called.\nIf not specified, run all.")
        ;
      
      // the testcase-ID is really an positional parameter
      op::positional_options_description posopt;
      posopt.add("id", -1);
      
      op::parsed_options parsed = 
        op::command_line_parser (cmdline)
          .options (syntax)
          .positional(posopt)
          .allow_unregistered()
          .run();  
      
      op::store (parsed, parameters);
      op::notify(parameters);   
      
      // remove all recognised options from original cmdline vector
      cmdline = op::collect_unrecognized(parsed.options, op::include_positional);
      
      if (parameters.count("help"))
        std::cerr << *this;
    }
  
  
  
  
  /** @return the Tests-Group as given on cmdline, or Suite::ALLGROUP as default
   */
  const string 
  Option::getTestgroup ()
    {
      ASSERT (parameters.count ("group"));
      return parameters["group"].as<string>();
    }
  
  /** @return ID of a single test to run, empty string if not specified
   */
  const string
  Option::getTestID ()
    {
      if (parameters.count ("id") &&
          parameters["id"].as<VectS>().size() > 0)
        return parameters["id"].as<VectS>()[0];
      else
        return string ();
    }
  
  /** @return \c true if --describe switch was given */
  const bool 
  Option::getDescribe ()
    {
      return parameters["describe"].as<bool>();
    }
  
  

  ostream& 
  operator<< (ostream& os, const Option& to)
    {
      return os << to.syntax;
    }

  
  
} // namespace lumiera
