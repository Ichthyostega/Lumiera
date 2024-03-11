/*
  TEMP-DIR.hpp  -  automatic allocation of a temporary working directory

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file temp-dir.hpp
 ** Manage a temporary directory for storage, with automated clean-up.
 ** @see TempDir_test
 */


#ifndef LIB_TEST_TEMP_DIR_H
#define LIB_TEST_TEMP_DIR_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/stat/file.hpp"
//#include <unordered_map>
//#include <iostream>
//#include <vector>
//#include <map>


namespace lib {
namespace test{
  
  /**
   * A RAII style temporary directory.
   */
  class TempDir
    : util::MoveOnly
    {
      fs::path loc_;
      
    public:
      TempDir() = default;
      
      
      fs::path
      makeFile()
        {
          UNIMPLEMENTED ("make temporary file");
        }
    };
  
  
}} // namespace lib::test
#endif /*LIB_TEST_TEMP_DIR_H*/
