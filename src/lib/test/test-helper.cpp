/*
  Test-Helper  -  collection of functions supporting unit testing

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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


#include "lib/test/test-helper.hpp"

#include <boost/format.hpp>

using boost::format;
using boost::str;


namespace lib {
namespace test{
  
  
  string
  showSizeof (size_t siz, const char* name)
  {
    static format fmt ("sizeof( %s ) %|30t|= %3d");
    return str (fmt % (name? name:"?") % siz);
  }
  
  
  /** @todo probably this can be done in a more clever way. Anyone...?
   */
  string
  randStr (size_t len)
  {
    static const string alpha ("aaaabbccddeeeeffgghiiiijjkkllmmnnooooppqqrrssttuuuuvvwwxxyyyyzz0123456789");
    static const size_t MAXAL (alpha.size());
    
    string garbage(len,'\0');
    size_t p = len;
    while (p)
      garbage[--p] = alpha[rand() % MAXAL];
    return garbage;
  }
  
  
  
}} // namespace lib::test
