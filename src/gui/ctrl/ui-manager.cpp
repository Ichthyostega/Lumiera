/*
  UiManager  -  Global UI Manager

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
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

* *****************************************************/


/** @file ui-manager.cpp
 ** Implementation of global concerns regarding a coherent UI and global state.
 ** Especially, the wiring of top-level components is done here, as is the
 ** basic initialisation of the interface and global configuration on
 ** UI toolkit level.
 ** 
 */


#include "gui/gtk-base.hpp"
#include "gui/config-keys.hpp"
#include "gui/ctrl/ui-manager.hpp"
#include "gui/ctrl/global-ctx.hpp"
#include "gui/ctrl/actions.hpp"
#include "gui/workspace/style-manager.hpp"
#include "lib/searchpath.hpp"
#include "lib/util.hpp"

#include <gdlmm.h>
  
using util::cStr;
using util::isnil;


namespace gui {
namespace ctrl {
  
  using Gtk::IconSize;
  using Gtk::IconFactory;
  
  using workspace::StyleManager;
  
  
  namespace { // dummy command line for GTK
    int argc =0;
  }
  
  
  
  // dtors via smart-ptr invoked from here...
  UiManager::~UiManager()
    { }
  
  
  /**
   * Initialise the GTK framework libraries
   */
  ApplicationBase::ApplicationBase()
    : Gtk::UIManager()
    , gtkMain_(&argc, nullptr)
    {
      Glib::thread_init();
      Gdl::init();
    }
  
  
  /**
   * Initialise the interface globally on application start.
   * Setup the main application menu and bind the corresponding actions.
   * Register the icon configuration and sizes and lookup all the icons.
   * @see lumiera::Config
   */
  UiManager::UiManager (UiBus& bus)
    : ApplicationBase()
    , globals_{new GlobalCtx{bus, *this}}
    , actions_{new Actions{*globals_}}
    , styleManager_{new StyleManager{}}
    {
      actions_->populateMainActions (*this);
    }
  
  
  /** 
   * @remarks this function is invoked once from the main application object,
   *          immediately prior to starting the GTK event loop. */
  void
  UiManager::createApplicationWindow()
  {
    if (globals_->windowList_.empty())
      globals_->windowList_.newWindow();
  }
  

  void
  UiManager::performMainLoop()
  {
    gtkMain_.run(); // GTK event loop
  }
  
  
  void
  UiManager::terminateUI()
  {
    gtkMain_.quit();
  }
  
  void
  UiManager::updateWindowFocusRelatedActions()
  {
          ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1076  find out how to handle this properly
    actions_->updateActionState (globals_->windowList_.findActiveWindow());
  }

  
  void
  UiManager::allowCloseWindow (bool yes)
  {
    this->get_action("/MenuBar/WindowMenu/WindowCloseWindow")
             ->set_sensitive (yes);
  }
  
  
}}// namespace gui::ctrl
