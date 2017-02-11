/*
  WindowList  -  manage all top level windows

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>

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


/** @file window-list.cpp
 ** Implementation parts of the list to manage all top level windows.
 ** @see ui-manager.hpp
 */


#include "gui/workspace/ui-manager.hpp"
#include "gui/workspace/window-list.hpp"
#include "gui/workspace/workspace-window.hpp"
#include "lib/util.hpp"

#include <memory>
#include <list>

using util::cStr;
using std::list;


namespace gui {
namespace workspace {
  
  
  
  WindowList::WindowList (UiManager& uiManager)
    : uiManager_{uiManager}
    , windowList_{}
    { }
  
  
  void
  WindowList::newWindow ()
  { 
    PWindow window (new WorkspaceWindow{uiManager_});
    REQUIRE(window);
    
    window->signal_delete_event().connect(sigc::mem_fun(
      this, &WindowList::on_window_closed));
    
    windowList_.push_back(window);
    
    window->show();
  
    updateCloseWindowInMenus();
  }
  
  
  WorkspaceWindow&
  WindowList::findActiveWindow()
  {
    UNIMPLEMENTED ("search the window list and return the currently active window");
  }
  
  
  bool
  WindowList::on_window_closed (GdkEventAny* event)
  {
    REQUIRE(event);
    REQUIRE(event->window);
    
    list<PWindow>::iterator iterator{windowList_.begin()};
    
    while (iterator != windowList_.end())
      {
        PWindow workspace_window(*iterator);
        REQUIRE(workspace_window);
        
        Glib::RefPtr<Gdk::Window> window = workspace_window->get_window();
        REQUIRE(window);
        if (window->gobj() == event->window)
          {
            // This window has been closed
            iterator = windowList_.erase(iterator);
          }
        else
          iterator++;
      }
    
    if (windowList_.empty())
      {
        // All windows have been closed - we should exit
        Gtk::Main *main = Gtk::Main::instance();               ////////////////////////////////////////////////TICKET #1032 : use gtk::Application instead of gtk::Main
        REQUIRE(main);
        main->quit();
      }
    
    updateCloseWindowInMenus();
    
    // Unless this is false, the window won't close
    return false;
  }
  
  
  void
  WindowList::updateCloseWindowInMenus()
  {
    uiManager_.allowCloseWindow ( 1 < windowList_.size());
  }
  
  
  
  
}}// namespace gui::workspace
