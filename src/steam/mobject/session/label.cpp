/*
  Label  -  a user visible Marker or Tag  

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file label.cpp
 ** @todo result of the very first code generation from UML in 2008. Relevance not clear yet... 
 */


#include "steam/mobject/session/label.hpp"
#include "lib/util.hpp"

using util::isnil;

namespace steam {
namespace mobject {
namespace session {
  
  
  /** @todo real validity self-check for Label MObject */
  bool
  Label::isValid()  const
  {
    return not isnil (typeID_);
  }
  
  
  
}}} // namespace steam::mobject::session
