/*
  CMDLINE.hpp  -  abstraction of the usual commandline, a sequence of strings 

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


/** @file cmdline.hpp
 ** Class to encapsulate the typical C-style commandline definition.
 ** A Cmdline object takes the typical `int argc, int** argv` and _copies_
 ** the referred data into a vector of strings. Thus `Cmdline` is a way to
 ** express explicitly on APIs that we are consuming commandline contents,
 ** and, moreover, it offers a way more sane interface to deal with those.
 ** @see CmdlineWrapper_test
 */


#ifndef LIB_CMDLINE_H
#define LIB_CMDLINE_H

#include <vector>
#include <string>



namespace lib {
  
  using std::string;
  using std::vector;
  using std::ostream;
  
  typedef vector<string> VectS;


  /**
   * Abstraction of the usual `int argc, int** argv`-Commandline,
   * to be able to treat it as a vector of strings. Inherits from
   * vector<std::string>, but provides convenient conversions to
   * string (joining delimited by space)... 
   */
  class Cmdline : public VectS
    {
    public:
      Cmdline (int argc, const char** argv);
      explicit Cmdline (const string cmdline);
      
      operator string () const;
      VectS& operator= (const VectS& source) { return VectS::operator= (source); }

      // inherited ctors
      template <class In>
      Cmdline (In first, In last) : VectS (first,last) {}
      Cmdline () : VectS () {} 
      
    };
   
   
   
} // namespace lib
#endif
