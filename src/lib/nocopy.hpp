/*
  NOCOPY.hpp  -  some flavours of non-copyable entities

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file §§§
 ** TODO §§§
 */


#ifndef LIB_NOCOPY_H
#define LIB_NOCOPY_H

#include <boost/noncopyable.hpp>



namespace util {
  
  /**
   * any copy and copy construction prohibited
   */
  class no_copy
    : boost::noncopyable
    { };
  
  /**
   * classes inheriting from this mixin
   * may be created by copy-construction,
   * but any copy-assignment is prohibited.
   * @note especially this allows returning
   *       by-value from a builder function,
   *       while prohibiting any further copy
   */
  class no_copy_by_client
    {
     protected:
       ~no_copy_by_client() {}
        no_copy_by_client() {}
        no_copy_by_client (no_copy_by_client const&) {}
        no_copy_by_client const&
        operator=(no_copy_by_client const&) { return *this; }
    };
  
} // namespace util
#endif /*LIB_NOCOPY_H*/
