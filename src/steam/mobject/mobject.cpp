/*
  MObject  -  Key Abstraction: A Media Object in the Session

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file mobject.cpp
 ** Top level implementation functions globally relevant to the session model
 */


#include "steam/mobject/mobject.hpp"
#include "steam/mobject/session/mobjectfactory.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/util.hpp"

using lib::time::Time;
using util::isnil;

namespace steam {
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
