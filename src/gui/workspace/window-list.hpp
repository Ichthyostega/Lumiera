/*
  WINDOW-LIST.hpp  -  manage all top level windows

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


/** @file window-list.hpp
 ** Manager for all top level application windows.
 ** The central WindowList instance is owned by the GtkLumiera object and
 ** initialised in GTK-main. The WindowList allows to create new windows
 ** integrated with the application framework.
 ** 
 ** @see gtk-lumiera.hpp
 ** @see ui-manager.hpp
 */


#ifndef GUI_WINDOW_LIST_H
#define GUI_WINDOW_LIST_H

#include "gui/gtk-base.hpp"

#include <boost/noncopyable.hpp>
#include <memory>
#include <list>


namespace gui {
  
  namespace model      { class Project; }           ////////////////////////////////////////////////////TICKET #1048 : rectify UI lifecycle
  namespace controller { class Controller; }        ////////////////////////////////////////////////////TICKET #1048 : rectify UI lifecycle
  
namespace workspace {
  
  class GlobalCtx;
  class WorkspaceWindow;
  
  using std::list;
  
  
  
  /**
   * A centralised manager of all top level application windows.
   */
  class WindowList
    : boost::noncopyable
    {
      using PWindow = std::shared_ptr<WorkspaceWindow>;
      
      GlobalCtx&    globalCtx_;
      list<PWindow> windowList_;
      
      
    public:
      WindowList (GlobalCtx&);
      
      bool empty()  const;
      
      void newWindow();
      void closeWindow();
      
      WorkspaceWindow& findActiveWindow();
      WorkspaceWindow& findFocusWindow();
      
      
    private:
      
      /** Event handler for when a window has been closed */
      bool on_window_closed (GdkEventAny* event);
      
      
      /**
       * On creating and closing of any window, handle enabling or disabling of the
       * Window/Close Window menu option.
       * 
       * It should be enabled when there is more than one window and disabled
       * otherwise.
       */
      void updateCloseWindowInMenus();
      
      
      
    private:
      
    };
  
  
  
  inline bool
  WindowList::empty()  const
  {
    return windowList_.empty();
  }
  
  
  
}}// namespace gui::workspace
#endif /*GUI_WINDOW_LIST_H*/