/*
  Cmdline  -  abstraction of the usual commandline, a sequence of strings 

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



#include "lib/util.hpp"
#include "include/logging.h"
#include "lib/cmdline.hpp"

#include <boost/regex.hpp>
#include <boost/algorithm/string/join.hpp>

using boost::regex;
using boost::smatch;
using boost::regex_search;
using boost::algorithm::join;

using util::noneg;


#include <iostream>

namespace lib {
  
  
  /** create as a tokenised <i>copy</i> of the current commandline.
   *  Note that argv[0] is always ignored. */
  Cmdline::Cmdline (int argc, const char** argv)
    : vector<string> (noneg(argc-1))
    {
      for (int i=1; i<argc; ++i)
        {
          ASSERT (argv[i]);
          (*this)[i-1] = argv[i];
        }
    }
   
    
  /** create by tokenising a string 
   *  (e.g. "fake" cmdline, separated by whitespace) 
   */ 
  Cmdline::Cmdline (const string cmdline)
    {
      regex tokendef("[^ \r\n\t]+");
      smatch match;
      string::const_iterator it = cmdline.begin();
      string::const_iterator end = cmdline.end();
      
      while (regex_search(it, end, match, tokendef))
        {
          string ss(match[0]);
          this->push_back(ss);
          it = match[0].second;
        }
      
    }
  
  
  /** conversion to string by joining the tokens */
  Cmdline::operator string () const
    {
      return join(*this," ");
    }
  
  
  
} // namespace lib
