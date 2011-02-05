/*
  OPTION.hpp  -  handle cmdline for starting the Lumiera application

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

*/


#ifndef LUMIERA_OPTION_H
#define LUMIERA_OPTION_H

#include "lib/cmdline.hpp"

#include <string>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/utility.hpp>



namespace lumiera {
  
  using std::string;
  using std::ostream;
  using lib::VectS;
  

  
  /**
   * Frontend for handling the Lumiera application
   * commandline arguments. A preconfigured wrapper
   * around boost::program_options, with the ability
   * to tolerate unknown options. The commandline
   * to be parsed is taken wrapped into a Cmdline
   * instance; after parsing this commandline
   * vector will contain only the remaining
   * unrecognised parts.
   */
  class Option : private boost::noncopyable
    {
    public:
      Option (lib::Cmdline& cmdline);
      
      const string getSessName();
      const VectS  getScripts();
      const VectS  getConfigDefs();
      int          getPort();
      
      bool isOpenSession ();
      bool isConfigDefs();
      bool isHeadless();
      bool isHelp();
      
      
    private:
      boost::program_options::options_description syntax;
      boost::program_options::variables_map parameters;
      
      friend ostream& operator<< (ostream&, const Option&);
    };
  
    
  /** for outputting the help messages. Forward accumulated 
   *  help messages from all contained option definitions */
  ostream& operator<< (ostream& os, const Option& to);
 
  
} // namespace lumiera
#endif
