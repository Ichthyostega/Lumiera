/*
  PLUG.hpp  -  LocatingPin for requesting connection to some Pipe

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

*/


/** @file plug.hpp
 ** @todo result of the very first code generation from UML in 2008. Relevance not clear yet... 
 */


#ifndef MOBJECT_SESSION_PLUG_H
#define MOBJECT_SESSION_PLUG_H

#include "proc/mobject/session/wish.hpp"



namespace proc {
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
  
}}} // namespace proc::mobject::session
#endif
