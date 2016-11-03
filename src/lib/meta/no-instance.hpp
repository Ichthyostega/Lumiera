/*
  NO-INSTANCE.hpp  -  marker for pure metaprogramming types

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


#ifndef LIB_META_NO_INSTANCE_H
#define LIB_META_NO_INSTANCE_H

#include <boost/static_assert.hpp>

  
namespace lib {
namespace meta{
  
  /** 
   * An Entity never to be instantiated.
   * Marker baseclass for elements used for metaprogramming only.
   * Every attempt to instantiate such an element will cause an
   * compilation failure
   */
  template<class X>
  struct NoInstance
    {
      NoInstance()                   { BOOST_STATIC_ASSERT(!sizeof(X)); }
      NoInstance (NoInstance const&) { BOOST_STATIC_ASSERT(!sizeof(X)); }
    };
  
}} // namespace lib::meta
#endif
