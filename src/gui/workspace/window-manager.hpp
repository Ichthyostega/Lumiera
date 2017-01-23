/*
  WINDOW-MANAGER.hpp  -  Global UI Manager

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

*/


/** @file window-manager.hpp
 ** Manager for all application windows and resources.
 ** This file defines the global UI Manager class. The central WindowManager
 ** instance is owned by the GtkLumiera object and initialised in GTK-main.
 ** The WindowManager has the ability to create new windows integrated with
 ** the application framework, to provide Icons and other resources and
 ** to set and access a general UI theme.
 ** 
 ** @see gtk-lumiera.hpp
 */


#ifndef GUI_WINDOW_MANAGER_H
#define GUI_WINDOW_MANAGER_H

#include "gui/gtk-base.hpp"

#include <boost/noncopyable.hpp>
#include <cairomm/cairomm.h>
#include <string>
#include <memory>


namespace gui {
  
  namespace model      { class Project; }           ////////////////////////////////////////////////////TICKET #1048 : rectify UI lifecycle
  namespace controller { class Controller; }        ////////////////////////////////////////////////////TICKET #1048 : rectify UI lifecycle
  
namespace workspace {
  
  class WorkspaceWindow;
  
  using std::shared_ptr;
  using std::string;
  
  
  
  /**
   * The centralised manager of all the windows,
   * icons and resources within Lumiera's GUI.
   */
  class WindowManager
    : boost::noncopyable
    {
      string iconSearchPath_;
      string resourceSerachPath_;
      
      
    public:
      
      /**
       * Creates a new window connected to a specified project and controller
       * @param source_project The project to connect the window to.
       * @param source_controller The controller to connect the window to.
       */
      void newWindow (gui::model::Project&, gui::controller::Controller&);
      
      
      
    private:
      
      /** Event handler for when a window has been closed */
      bool on_window_closed(GdkEventAny* event);
      
      
      /**
       * On creating and closing of any window, handle enabling or disabling of the
       * Window/Close Window menu option.
       * 
       * It should be enabled when there is more than one window and disabled
       * otherwise.
       */
      void updateCloseWindowInMenus();
      
      
      
    private:
      
      std::list<shared_ptr<workspace::WorkspaceWindow>> windowList;
      
    };
  
  
  
}}// namespace gui::workspace
#endif /*GUI_WINDOW_MANAGER_H*/
