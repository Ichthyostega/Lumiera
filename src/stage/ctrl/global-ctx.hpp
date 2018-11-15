/*
  GLOBAL-CTX.hpp  -  Context of global UI top-level entities

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


/** @file global-ctx.hpp
 ** Dependency context to hold all the global UI top-level entities.
 ** There is a small number of management facilities, responsible for conducting all the
 ** global concerns of the Lumiera UI. The circle of these _top level managers_ is quite cohesive,
 ** insofar each knows each other and is aware of each others responsibilities. When starting the UI,
 ** this global context is established and wired in one shot, and any failure here immediately terminates
 ** the UI-Layer. It is the UiManager's responsibility to install this management circle and this task is
 ** what effectively brings the UI into operative state.
 ** 
 ** Towards the outside, the interface exposed by these managers is rather narrow; basically the parts
 ** comprising the UI are to be wired at startup and expected to react based on events from then on.
 ** Shutdown of the GUI is effected by terminating the GTK event loop. Each of the top-level managers
 ** serves a distinct purpose and will be addressed through a dedicated API, even by the collaborating
 ** other top-level managers.
 ** 
 ** The global UI context is comprised of the following members
 ** - connection to the [UI-Bus](\ref ui-bus.hpp)
 ** - the UiManager
 ** - the InteractionDirector
 ** - the WindowLocator
 ** - the Wizard
 ** 
 ** @see gtk-lumiera.hpp
 ** @see ui-bus.hpp
 ** @see ui-manager.hpp
 ** @see interaction-director.hpp
 */


#ifndef GUI_CTRL_GLOBAL_CTX_H
#define GUI_CTRL_GLOBAL_CTX_H

#include "stage/gtk-base.hpp"
#include "stage/ui-bus.hpp"
#include "stage/ctrl/ui-manager.hpp"
#include "stage/ctrl/window-locator.hpp"
#include "stage/interact/wizard.hpp"
#include "stage/interact/interaction-director.hpp"
#include "lib/nocopy.hpp"


namespace gui {
namespace ctrl {
  
  using interact::InteractionDirector;
  
  
  /**
   * A global circle of top-level UI management facilities.
   * Creating an instance of this context makes the Lumiera UI operative.
   * All entities installed and wired here are mutually dependent and aware
   * of each partner's role; failure to create anyone will terminate the UI.
   * 
   * @remark the UiManager is responsible to install this top-level context
   */
  class GlobalCtx
    : util::NonCopyable
    {
      
    public:
      UiBus&     uiBus_;
      UiManager& uiManager_;
      
      WindowLocator       windowLoc_;
      InteractionDirector director_;
      interact::Wizard    wizard_;
      
      
    public:
      /**
       * Establish the top-level UI context of the Lumiera user interface.
       */
      GlobalCtx (UiBus& bus, UiManager& manager)
        : uiBus_{bus}
        , uiManager_{manager}
        , windowLoc_{*this}
        , director_{*this}
        , wizard_{*this}
        { }
      
    private:
    };
  
  
  
}}// namespace gui::ctrl
#endif /*GUI_CTRL_GLOBAL_CTX_H*/
