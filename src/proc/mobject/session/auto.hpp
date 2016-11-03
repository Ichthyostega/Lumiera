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
 ** TODO auto.hpp
 */


#ifndef PROC_MOBJECT_SESSION_AUTO_H
#define PROC_MOBJECT_SESSION_AUTO_H

#include "proc/mobject/session/meta.hpp"
#include "proc/mobject/paramprovider.hpp"



namespace proc {
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



}}} // namespace proc::mobject::session
#endif
