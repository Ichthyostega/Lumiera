/*
  WINDOW-LOCATOR.hpp  -  manage all top level windows

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file window-locator.hpp
 ** Manager for all top level application windows.
 ** The central WindowLocator is part of the [UI global context](\ref GlobalCtx) and thus
 ** initialised on startup of the UI. The WindowLocator allows to create new windows
 ** integrated with the application framework.
 ** 
 ** @see gtk-lumiera.hpp
 ** @see ui-manager.hpp
 */


#ifndef STAGE_CTRL_WINDOW_LOCATOR_H
#define STAGE_CTRL_WINDOW_LOCATOR_H

#include "stage/gtk-base.hpp"
#include "stage/ctrl/panel-locator.hpp"
#include "lib/depend-inject.hpp"
#include "lib/nocopy.hpp"

#include <memory>
#include <list>


namespace stage {
  
  namespace model      { class Project; }           ////////////////////////////////////////////////////TICKET #1048 : rectify UI lifecycle
  namespace controller { class Controller; }        ////////////////////////////////////////////////////TICKET #1048 : rectify UI lifecycle
  
namespace workspace { class WorkspaceWindow; }
namespace model {
  class ElementAccess;
}
namespace ctrl {
  
  class GlobalCtx;
  class ElemAccessDir;
  using std::list;
  
  
  
  /**
   * A centralised manager of all top level application windows.
   */
  class WindowLocator
    : ::util::NonCopyable
    {
      using PWindow = std::shared_ptr<workspace::WorkspaceWindow>;
      using Service_ElementAccess = lib::DependInject<model::ElementAccess>::ServiceInstance<ElemAccessDir>;
      
      GlobalCtx&    globalCtx_;
      list<PWindow> windowList_;
      PanelLocator  panelLoc_;
      
      Service_ElementAccess elementAccess_;
      
      
    public:
      WindowLocator (GlobalCtx&);
     ~WindowLocator ();
      
      bool empty()  const;
      
      void newWindow();
      void closeWindow();
      
      workspace::WorkspaceWindow& findActiveWindow();
      workspace::WorkspaceWindow& findFocusWindow();
      
      PanelLocator& locatePanel() { return panelLoc_; }
      
      
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
  WindowLocator::empty()  const
  {
    return windowList_.empty();
  }
  
  
  
}}// namespace stage::ctrl
#endif /*STAGE_CTRL_WINDOW_LOCATOR_H*/
