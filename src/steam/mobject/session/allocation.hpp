/*
  ALLOCATION.hpp  -  objective to place a MObject in a specific way

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


/** @file allocation.hpp
 ** @todo result of the very first code generation from UML in 2008. Relevance not clear yet...
 ** @deprecated WTF? looks like a leftover of some early brainstorming...
 */


#ifndef PROC_MOBJECT_SESSION_ALLOCATION_H
#define PROC_MOBJECT_SESSION_ALLOCATION_H

#include "steam/mobject/session/locatingpin.hpp"

#include <string>
using std::string;


namespace proc {
namespace mobject {
namespace session {
  
  
  /**
   * Interface (abstract): any objective, constraint or wish
   * of placing a MObject in a specific way.
   */
  class Allocation : public LocatingPin
    {
    protected:
      /** human readable representation of the condition
       *  characterising this allocation, e.g. "t >= 10"
       */
      string repr;
      
      virtual void intersect (LocatingSolution&)  const;
      
    public:
      const string& getRepr () const { return repr; }
      
      virtual Allocation* clone ()  const = 0;
    };
  
  
  
}}} // namespace proc::mobject::session
#endif
