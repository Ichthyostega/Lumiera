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
 ** @see TICKET #1032 regarding replacements for the deprecated Gtk::Main
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/config-keys.hpp"
#include "stage/ctrl/ui-manager.hpp"
#include "stage/ctrl/global-ctx.hpp"
#include "stage/ctrl/actions.hpp"
#include "stage/ctrl/facade.hpp"
#include "stage/workspace/ui-style.hpp"
#include "lib/searchpath.hpp"
#include "lib/util.hpp"

#include <gdlmm.h>
  
using util::cStr;
using util::isnil;


namespace gui {
namespace ctrl {
  
  using Gtk::IconSize;
  using Gtk::IconFactory;
  
  using workspace::UIStyle;
  
  
  
  
  // dtors via smart-ptr invoked from here...
  UiManager::~UiManager() { }
  
  
  /**
   * Initialise the GTK framework libraries.
   * @remark in 5/2017 we abandoned the (deprecated) `Gtk::Main`,
   *         but we did not switch to `Gtk::Application`, rather we just
   *         incorporated the framework initialisation code directly into
   *         our own code base. This allows us to ignore all the shiny new
   *         D-Bus and desktop integration stuff.
   * @note confirmed this class calls the proper Gtk internals as of 2018
   */
  ApplicationBase::ApplicationBase()
    {
      Glib::thread_init();
      //---------------------------------------------copied from Gtk::Main
      gtk_init (nullptr, nullptr);
      Gtk::Main::init_gtkmm_internals();
      //---------------------------------------------copied from Gtk::Main
      Gdl::init();
    }
  
  ApplicationBase::~ApplicationBase()
    {
      //---------------------------------------------copied from Gtk::Main
      // Release the gtkmm type registration tables,
      // allowing Main to be instantiated again:
      Glib::wrap_register_cleanup();
      Glib::Error::register_cleanup();
      //---------------------------------------------copied from Gtk::Main
      
      // NOTE: GtkMainConnectionNode only serves the deprecated key_snooper API as of 2018
    }
  
  
  /**
   * @remark Creating the UiManager initialises the interface globally on application start.
   *         It wires the global services and attaches to the UI-Bus, defines the main
   *         application menu and binds the corresponding actions. Moreover, the StyleManager
   *         register the icon configuration and sizes and loads the icon definitions.
   */
  UiManager::UiManager (UiBus& bus)
    : ApplicationBase()
    , Gtk::UIManager()
    , globals_{new GlobalCtx{bus, *this}}
    , uiStyle_{new UIStyle{}}
    , actions_{new Actions{*globals_}}
    , facade_{} // note: not activated yet
    {
      actions_->populateMainActions (*this);
    }
  
  
  /** 
   * @remarks this function is invoked once from the main application object,
   *          immediately prior to starting the GTK event loop.
   */
  void
  UiManager::createApplicationWindow()
  {
    if (globals_->windowLoc_.empty())
      globals_->windowLoc_.newWindow();
  }
  

  /**
   * Run the GTK UI. Also _activate_ the UI the external interfaces.
   * @remarks this function is equivalent to calling Gtk::Main::run()
   *          In GTK-3.0.3, the implementation is already based on libGIO;
   *          after possibly handling command line arguments (which does not apply
   *          in our case), it invokes `g_main_loop_run()`, which in turn ends up
   *          polling the main context via `g_main_context_iterate()`, until the
   *          use count drops to zero. This is the "event loop".
   */
  void
  UiManager::performMainLoop()
  {
    facade_.reset (new Facade{globals_->uiBus_, *this});
    
    gtk_main(); // GTK event loop
    
    facade_.reset (nullptr);
  }
  
  
  /**
   * @note this function can be invoked from an UI event, since it just
   *       signals shutdown to the GTK event loop by invoking `gtk_main_quit()`.
   *       The latter will finish processing of the current event and then return
   *       from the UiManager::performmainLoop() call, which eventually causes the
   *       UI subsystem to signal termination to the Lumiera application as a whole.
   */
  void
  UiManager::terminateUI()
  {
    gtk_main_quit();
  }
  
  
  /**
   * @remarks moves the given operation into our private dispatcher queue and then
   *          schedules dequeuing and invocation into the UI event thread.
   * @param op a completely closed lambda or functor
   * @warning closure need to be by value or equivalent, since
   *          the operation will be executed within another call stack
   */
  void
  UiManager::schedule (Operation&& task)
  {
    UNIMPLEMENTED ("TICKET #1151 build a suitable dispatcher into the GTK loop");
  }

  
  void
  UiManager::updateWindowFocusRelatedActions()
  {
          ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1076  find out how to handle this properly
    actions_->updateActionState (globals_->windowLoc_.findActiveWindow());
  }

  
  void
  UiManager::allowCloseWindow (bool yes)
  {
    this->get_action("/MenuBar/WindowMenu/WindowCloseWindow")
             ->set_sensitive (yes);
  }
  
  
}}// namespace gui::ctrl
