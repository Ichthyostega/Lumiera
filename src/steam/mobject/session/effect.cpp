/*
  Effect  -  Model representation of a pluggable and automatable effect.

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file session/effect.cpp
 ** @todo result of the very first code generation from UML in 2008. Relevance not clear yet... 
 */


#include "steam/mobject/session/effect.hpp"

namespace steam {
namespace mobject {
namespace session {
  
  /**
   * @TODO: clarify asset->mobject relation and asset dependencies; Ticket #255
   */
  asset::Proc const& 
  Effect::getProcAsset() const
  {
    UNIMPLEMENTED ("how to access the processing asset associated to a given Effect-MObject");
  }
  
  
  
}}} // namespace steam::mobject::session
