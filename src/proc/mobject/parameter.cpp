/*
  Parameter  -  representation of an automatable effect/plugin parameter

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

* *****************************************************/


/** @file parameter.cpp
 ** Implementation functions of the Parameter abstraction.
 ** 
 ** @todo as of 2016, we still need to work out our parameter/automation concept.
 */


#include "lib/error.hpp"
#include "proc/mobject/parameter.hpp"
#include "proc/mobject/paramprovider.hpp"

namespace proc {
namespace mobject {
  
  
  template<class VAL>
  VAL
  Parameter<VAL>::getValue ()
  {
    UNIMPLEMENTED ("Any idea how to handle various kinds of parameter types");
  }
  
  
  
}} // namespace mobject
