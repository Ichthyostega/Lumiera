/*
  BINDING.hpp  -  an association between session entities, constituting a scope
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/


#ifndef MOBJECT_SESSION_BINDING_H
#define MOBJECT_SESSION_BINDING_H

#include "proc/mobject/session/meta.hpp"



namespace mobject {
namespace session {
  
  
  /**
   * Association of two entities within the Session
   * deliberately linked together, thereby carrying additional mappings
   * between properties or facilities within the entities to be linked together.
   * An example would be the situation when a Sequence is linked either into
   * a Timeline or MetaClip. Usually, the Placement holding such a Binding
   * also constitutes a scope containing other nested objects.
   */
  class Binding : public Meta
    {
    public:
        //////////////////////////////TICKET #566
    };



}} // namespace mobject::session
#endif
