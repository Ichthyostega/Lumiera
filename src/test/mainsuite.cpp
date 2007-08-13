/*
  mainsuite.cpp  -  "the" cinelerra3 self test suite

  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
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


#include "test/helper/suite.hpp"


/** run all tests or any single test specified in the first
 *  cmd line argument.
 *  Note: to ease debugging, we don't catch any exceptions. 
 */
int main (int argc, char* argv[])
  {
    test::Suite suite (test::Suite::ALLGROUP);
    suite.run(argc,argv);
    return 0;
  }
