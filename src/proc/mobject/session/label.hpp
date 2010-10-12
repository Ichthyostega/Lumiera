/*
  LABEL.hpp  -  a user visible Marker or Tag
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#ifndef MOBJECT_SESSION_LABEL_H
#define MOBJECT_SESSION_LABEL_H

#include "proc/mobject/session/meta.hpp"
#include "lib/symbol.hpp"



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
  
  
  
}} // namespace mobject::session
#endif
