/*
  AbstractMO  -  abstract base class for all Media Objects

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file abstractmo.cpp
 ** @todo result of the very first code generation from UML in 2008. Relevance not clear yet... 
 */


#include "steam/mobject/session/abstractmo.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"

using util::_Fmt;
using util::isnil;

namespace steam {
namespace mobject {
namespace session {
  
  /** default/fallback implementation of equality
   *  using literal object identity (same address).
   *  Required to enable handling by lumiera::P
   */
  bool
  AbstractMO::operator== (const MObject& oo)  const
  {
    return (this == &oo);
  }
  
  
  
  string
  AbstractMO::buildShortID (lib::Literal typeID, string suffix)  const
  {
    static size_t i=0;
    
    REQUIRE (!isnil (typeID));
    if (!isnil (suffix))
      return typeID+"."+suffix;
    else
      return _Fmt{"%s.%03d"} % typeID % (++i);
  }
  
  
}}} // namespace mobject::session
