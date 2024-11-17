/*
  PLUG.hpp  -  LocatingPin for requesting connection to some Pipe

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file plug.hpp
 ** @todo result of the very first code generation from UML in 2008. Relevance not clear yet... 
 */


#ifndef MOBJECT_SESSION_PLUG_H
#define MOBJECT_SESSION_PLUG_H

#include "steam/mobject/session/wish.hpp"



namespace steam {
namespace asset { class Pipe; }

namespace mobject {
namespace session {
  
  /**
   * LocatingPin for requesting connection to some Pipe
   */
  class Plug : public Wish
    {
    protected:
      /** the Pipe this MObject wants to be connected to */
      asset::Pipe* outPipe;   ////////////////////////////////TODO: shared_ptr
      
    };
  
}}} // namespace steam::mobject::session
#endif
