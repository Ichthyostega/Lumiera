/*
  LABEL.hpp  -  a user visible Marker or Tag

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


/** @file label.hpp
 ** A marker or reference point in the Session.
 ** Label MObjects can be [placed](\ref placement.hpp) at various locations and scopes,
 ** e.g. on the timeline, or relative to the media data of a clip. They can be used to give
 ** a visual clue for the user's orientation within the edit, or for navigation on the timeline,
 ** but also as an anchor point to place other elements with relative offset.
 ** 
 ** @todo result of the very first code generation from UML in 2008.
 ** @todo this is expected to become a very important facility eventually, so expec a lot of rework here... 
 */


#ifndef STEAM_MOBJECT_SESSION_LABEL_H
#define STEAM_MOBJECT_SESSION_LABEL_H

#include "steam/mobject/session/meta.hpp"
#include "lib/symbol.hpp"



namespace steam {
namespace mobject {
namespace session {
  
  using lib::Symbol;


  /**
   * Any sort of User visible Marker or Tag, used
   * to mark time positions and ranges, or specific
   * locations to attach other MObjects to.
   * 
   * @todo Placeholder code for now.
   * @todo planning to provide some kind of dynamic properties (map)
   */
  class Label : public Meta
    {
      ///////////TODO: timespan fields here or already in class Meta??
      
      Symbol typeID_;
      
      string
      initShortID()  const
        {
          return buildShortID("Label");
        }
      
      bool isValid()  const;
      
    public:
      Label (Symbol type)
        : typeID_(type)
        {
          throwIfInvalid();
        }
      
    };
  
  
  
}}} // namespace steam::mobject::session
#endif
