/*
  Cmdline  -  abstraction of the usual commandline, a sequence of strings 

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file cmdline.cpp
 ** Implementation of a wrapper / marker to handle "commandline arguments".
 */



#include "include/logging.h"
#include "lib/hash-standard.hpp"
#include "lib/cmdline.hpp"
#include "lib/format-util.hpp"

#include <regex>

using std::regex;
using std::smatch;
using std::regex_search;
using util::join;
using util::noneg;



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
      static regex TOKENDEF{"\\S+"};
      smatch match;
      string::const_iterator it = cmdline.begin();
      string::const_iterator end = cmdline.end();
      
      while (regex_search(it, end, match, TOKENDEF))
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
