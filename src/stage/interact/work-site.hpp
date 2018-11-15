/*
  WORK-SITE.hpp  -  a place within interface space where work is done

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


/** @file work-site.hpp
 ** Represent a place "in" the interface where the user is about to do something.
 ** The concept of a WorkSite is introduced within the Lumiera UI to allow for a
 ** coherent handling of user interactions above the level of individual widgets.
 ** We acknowledge that our users not just push some buttons; rather they are here
 ** to perform ongoing trails of work, which serve to create a new artistic structure,
 ** known as "a film". These trails of work follow their own, inner coherence and can
 ** be structured into several "passes", "concerns", "topics", "stages". The user typically
 ** has to switch frequently between several contexts, which is one of the major impediments
 ** to hamper the fluidity of "media work". To mitigate, we create work sites as abstract
 ** intermediary anchor points, to allow clustering of information, command bindings,
 ** presentation state, in the hope this helps to create a distinct, recognisable
 ** _location of ongoing work_ for the user to relate to, to leave and to come back later.
 ** 
 ** @todo WIP 2/2017 early draft / foundations of "interaction control"
 ** 
 ** @see interaction-director.hpp
 ** @see ui-bus.hpp
 */


#ifndef STAGE_INTERACT_WORK_SITE_H
#define STAGE_INTERACT_WORK_SITE_H

#include "stage/gtk-base.hpp"
#include "lib/nocopy.hpp"

//#include <string>
//#include <memory>


namespace gui {
namespace interact {
  
//  using std::unique_ptr;
//  using std::string;
  
//  class GlobalCtx;
  
  
  
  /**
   * A clustering of information and bindings,
   * related to a "location" where the user is about
   * to do ongoing editing work.
   * 
   * @todo initial draft as of 2/2017 -- actual implementation has to be filled in
   */
  class WorkSite
    : util::NonCopyable
    {
      
    public:
      WorkSite();
     ~WorkSite();
      
    private:
      
    };
  
  
  
}}// namespace gui::interact
#endif /*STAGE_INTERACT_WORK_SITE_H*/
