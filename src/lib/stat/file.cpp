/*
  File  -  collection of functions supporting unit testing

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


/** @file file.cpp
 ** Storage and implementation details for filesystem helpers.
 ** @todo 3/24 consider to establish some central location to define basic literals.
 */


#include "lib/stat/file.hpp"


namespace std::filesystem {
  const string UNIX_HOMEDIR_SYMBOL{"~"};
  lib::Literal UNIX_HOMEDIR_ENV{"HOME"};
}

namespace util {
  using fs::UNIX_HOMEDIR_SYMBOL;
  using fs::UNIX_HOMEDIR_ENV;
}
