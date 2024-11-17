/*
  File  -  collection of functions supporting unit testing

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


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
