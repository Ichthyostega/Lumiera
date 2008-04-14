/*
  mainsuite.cpp  -  "the" Lumiera self test suite

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>
                        Hermann Vosseler <Ichthyostega@web.de>

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
*/


#include "common/test/suite.hpp"
#include "common/test/testoption.hpp"
#include "lib/appconfig.hpp"

using lumiera::Appconfig;
using lumiera::ON_GLOBAL_INIT;
using lumiera::ON_GLOBAL_SHUTDOWN;


/** run all tests or any single test specified in the first
 *  cmd line argument.
 *  Note: to ease debugging, we don't catch any exceptions. 
 */
int main (int argc, char* argv[])
{
  util::Cmdline args (argc,argv);
  test::TestOption optparser (args);
  test::Suite suite (optparser.getTestgroup());
  Appconfig::lifecycle(ON_GLOBAL_INIT);
  
  if (optparser.getDescribe())
    suite.describe();
  else
    suite.run (args);

  Appconfig::lifecycle(ON_GLOBAL_SHUTDOWN);
  return 0;
}
