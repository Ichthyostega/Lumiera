/*
  AUTO.hpp  -  Media Object holding automation data

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


/** @file auto.hpp
 ** @todo result of the very first code generation from UML in 2008. Relevance not clear yet... 
 */


#ifndef STEAM_MOBJECT_SESSION_AUTO_H
#define STEAM_MOBJECT_SESSION_AUTO_H

#include "steam/mobject/session/meta.hpp"
#include "steam/mobject/paramprovider.hpp"



namespace steam {
namespace mobject {
namespace session {
  
  
  /**
   * Automation data for some parameter 
   * (i.e. the parameter is a time varying function)
   */
  template<class VAL>
  class Auto : public Meta, public ParamProvider<VAL>
    {
      string
      initShortID()  const
        {
          return buildShortID("Auto");
        }
    
    public:
      //////////////////////////////TICKET #566
    
      virtual VAL getValue () ;
    };



}}} // namespace steam::mobject::session
#endif
