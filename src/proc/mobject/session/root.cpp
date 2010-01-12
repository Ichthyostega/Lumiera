/*
  Root  -  root element of the high-level model, global session scope
 
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
 
* *****************************************************/


#include "proc/mobject/session/root.hpp"
#include "proc/mobject/session/defsmanager.hpp"


namespace mobject {
namespace session {
  
  /** */
  Root::Root (DefsManager& dM)
    : defaults_(dM)
    { }
  
  
  
  /** @todo validity self-check of the model root
   *        should do substantial checks; the idea is
   *        to perform a complete sanity check by delegating
   *        to the parts.
   *  @note beware of performance problems here!
   */
  bool
  Root::isValid()  const
  {
    return true; //////////////////TICKET #447
  }

  /////////////////////////////////TODO more to come.....
  
  
}} // namespace mobject::session
