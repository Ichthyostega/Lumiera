/*
  WORK-SITE-TRAIL.hpp  -  manage history of visited work sites

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file work-site-trail.hpp
 ** History of work sites visited by the user in recent past.
 ** The immediate consequence of introducing the concept of a WorkSite is the
 ** expectation for the user to navigate between several such locations within
 ** the interface space. This establishes a context of _least recently visited_
 ** locations, which can be used to resolve various contextual questions. Most
 ** notably, managing such a history allows us to _navigate back._
 ** 
 ** @todo WIP 2/2017 early draft / foundations of "interaction control"
 ** 
 ** @see work-site.hpp
 ** @see interaction-director.hpp
 ** @see ui-bus.hpp
 */


#ifndef GUI_INTERACT_WORK_SITE_TRAIL_H
#define GUI_INTERACT_WORK_SITE_TRAIL_H

#include "gui/gtk-base.hpp"
#include "lib/nocopy.hpp"

//#include <string>
//#include <memory>


namespace gui {
namespace interact {
  
//  using std::unique_ptr;
//  using std::string;
  
//  class GlobalCtx;
  
  
  
  /**
   * Establish and manage a history of recent [work sites](\ref WorkSite).
   * 
   * @todo initial draft as of 2/2017 -- actual implementation has to be filled in
   */
  class WorkSiteTrail
    : util::NonCopyable
    {
      
    public:
      WorkSiteTrail();
     ~WorkSiteTrail();
      
    private:
      
    };
  
  
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_WORK_SITE_H*/
