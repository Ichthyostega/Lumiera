/*
  BasicSetup  -  elementary self-configuration of the application

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file basic-setup.cpp
 ** Implementation of self-configuration and bootstrap functionality.
 ** This allows the application to "find its parts" at startup.
 ** @note as of 2016, BasicSetup::BasicSetup(string) also reads
 **       and evaluates a simple `*.ini` file to provide a
 **       preliminary mechanism for application configuration.
 **       This implementation is based on Boost program options
 ** 
 ** @see searchpath.hpp
 */


#include "common/basic-setup.hpp"
#include "lib/searchpath.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"

extern "C" {
#include <unistd.h>
}
#include <boost/filesystem.hpp>
#include <fstream>


namespace lumiera {
  
  using std::string;
  using std::ifstream;
  
  namespace fsys = boost::filesystem;
  namespace opt = boost::program_options;
  
  namespace { // details of the bootstrap process...
    
    
    // Helper to locate a module using a search path spec
    using lib::resolveModulePath;
    
    /** use the general mechanism for resolving a search path
     *  to get the absolute path of the \c setup.ini */
    string
    resolve (fsys::path iniSpec)
    {
      string searchpath = iniSpec.parent_path().string();                     ///////////TICKET #896
      return resolveModulePath (iniSpec.filename(), searchpath);
    }
    
  }//(End) implementation details
  
  
  
  /**
   * Creating the BasicSetup object performs the
   * initial self-configuration of the Lumiera Application.
   * For this, the `setup.ini` file is located relative to the
   * current application executable, read in and parsed into a
   * map of setup variables.
   */
  BasicSetup::BasicSetup (string bootstrapIni)
    : syntax("Lumiera installation and platform configuration")
    , settings()
    {
      syntax.add_options()
        ("Lumiera.gui",        opt::value<string>(),
                               "name of the Lumiera GUI plugin to load")
        ("Lumiera.modulepath", opt::value<string>(),
                               "search path for loadable modules. "
                               "May use $ORIGIN to refer to the EXE location")
        ("Lumiera.configpath", opt::value<string>(),
                               "search path for extended configuration. "
                               "Extended Config system not yet implemented "
                               "Ignored as of 2/2011")
        ("Lumiera.title",      opt::value<string>(),
                               "title of the Lumiera Application, e.g. for windows")
        ("Lumiera.version",    opt::value<string>(),
                               "Application version string")
        ("Lumiera.website",    opt::value<string>(),
                               "URL of the Lumiera website")
        ("Lumiera.authors",    opt::value<string>(),
                               "names of Lumiera authors, for 'about' dialog. Separated by '|'")
        ("Lumiera.copyright",  opt::value<string>(),
                               "year(s) for the author's copyright claim")

        ("Gui.stylesheet",     opt::value<string>(),
                               "name of the GTK stylesheet to use. Will be searched in resource path")
        ("Gui.iconpath",       opt::value<string>(),
                               "search path for icons")
        ("Gui.resourcepath",   opt::value<string>(),
                               "general search path for UI resources")
        ;
      
      ifstream configIn (resolve(bootstrapIni).c_str());
      
      
      opt::parsed_options parsed = opt::parse_config_file (configIn, syntax);
      
      opt::store (parsed, settings);
      opt::notify(settings);
    }

  
  
} // namespace lumiera
