/*
  GENFUNC.hpp  -  generic identification functions

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file genfunc.hpp
 ** Generic function to build identification schemes.
 ** These template functions are meant as common extension point.
 ** The purpose is to streamline and disentangle the various identification schemes
 ** in use at various places within Lumiera. We strive to cover all the common basic
 ** usage situations through these functions
 ** - build a symbolic ID
 ** - build a classification record
 ** - create a readable yet unique ID
 ** - render an ID in human readable form
 ** - derive a hash function
 ** 
 ** @see EntryID
 **
 */



#ifndef LIB_IDI_GENFUNC_H
#define LIB_IDI_GENFUNC_H

#include "lib/hash-value.h"
//#include "lib/hash-standard.hpp"

#include <string>


namespace lib {
namespace idi {
  
  namespace { // integration helpers...
  } //(End)integration helpers...
  
  
  
  /********************************************************//**
   * A Mixin to add a private ID type to the target class,
   * together with storage to hold an instance of this ID,
   * getter and setter, and a templated version of the ID type
   * which can be used to pass specific subclass type info.
   */
  
  
  
  
  
}} // namespace lib::idi
#endif /*LIB_IDI_GENFUNC_H*/
