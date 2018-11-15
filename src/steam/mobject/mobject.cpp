/*
  MObject  -  Key Abstraction: A Media Object in the Session

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


/** @file mobject.cpp
 ** Top level implementation functions globally relevant to the session model
 */


#include "proc/mobject/mobject.hpp"
#include "proc/mobject/session/mobjectfactory.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/util.hpp"

using lib::time::Time;
using util::isnil;

namespace proc {
namespace mobject {
  
  
  
  /** Storage for the (single, static) MObject factory object.
   */
  session::MObjectFactory MObject::create;

  
  MObject::MObject()
    : length_(Time::ZERO)
    , shortID_()
    { }
  
  
  MObject::~MObject() { };
  
  
  
  string const&
  MObject::shortID()  const
  {
    if (isnil (shortID_))
      shortID_ = initShortID();
    return shortID_;
  }



}} // namespace mobject
