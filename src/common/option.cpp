/*
  Option  -  handle cmdline for starting the Lumiera application

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


/** @file option.cpp
 ** Implementation of commandline argument handling,
 ** based on lib Boost »program options«
 */


#include "lib/error.hpp"
#include "lib/util.hpp"
#include "common/option.hpp"



typedef boost::program_options::options_description Syntax;
typedef boost::program_options::variables_map VarMap;

namespace op = boost::program_options;

using lib::VectS;


namespace lumiera {
  
  /** set up an options parser to use the application commandline.
   *  Recognises the following options
   *  \code
   *  --help
   *  [--session] FILENAME
   *  --script FILENAME
   *  --headless
   *  --port #
   *  \endcode
   *  @todo describe the actual options
   */
  Option::Option (lib::Cmdline& cmdline)
    : syntax("Lumiera, the non linear video editor.\nSupported parameters"),
      parameters()
    {
      syntax.add_options()
        ("help,h",      "produce help message")
        ("session,f",   op::value<string>(),
                        "session file to load (UNIMPLEMENTED)")
        ("script,s",    op::value<VectS>(),
                        "execute the given script (UNIMPLEMENTED)")
        ("headless",    op::bool_switch(),
                        "start without GUI")
        ("port,p",      op::value<int>(),
                        "open renderfarm node at given port (UNIMPLEMENTED)")
        ("define,def,D",op::value<VectS>(),
                        "enter definition into config system (UNIMPLEMENTED)")
        ;
      
      // the name of an session file to open...
      op::positional_options_description posopt;
      posopt.add("session", 1);   // ... can be given as 1st positional parameter
      
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
      
      if (isHelp())
        {
          cerr << *this;
          exit(-1);
        }
      if (isConfigDefs())
        {
          UNIMPLEMENTED ("feed definitions from commandline to Config system");
        }
    }
  
  
  
  /** syntax help requested? */
  bool Option::isHelp ()        { return (parameters.count("help")); }
  
  /** should an existing session file be loaded? */
  bool Option::isOpenSession () { return (parameters.count ("session")); }
  
  /** additional Config defs to fed to config system? */
  bool Option::isConfigDefs ()  { return (parameters.count ("define")); }
  
  
  /** @return the name of the session file to open */
  const string 
  Option::getSessName ()
    {
      ASSERT (parameters.count ("session"));
      return parameters["session"].as<string>();
    }
  
  /** @return an (maybe empty) vector 
   *  containing all specified scripts to run. */
  const VectS
  Option::getScripts ()
    {
      return parameters["script"].as<VectS>();
    }
  
  /** @return an (maybe empty) vector 
   *  containing any additional Config definitions to set. */
  const VectS
  Option::getConfigDefs ()
    {
      return parameters["define"].as<VectS>();
    }
  
  /** @return \c true if --headless switch was given */
  bool 
  Option::isHeadless ()
    {
      return parameters["headless"].as<bool>();
    }
  
  /** @return the port number for a render node server
   *          or 0 if --port was not specified */
  int
  Option::getPort ()
    {
      if (parameters.count ("port"))
        return parameters["port"].as<int>();
      else
        return 0;
    }
  
  

  ostream& 
  operator<< (ostream& os, const Option& ops)
    {
      return os << ops.syntax;
    }

  
  
} // namespace lumiera
