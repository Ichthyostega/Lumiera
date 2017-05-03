/*
  PANEL.hpp  -  common base class for all docking panels            

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


/** @file panel.hpp
 ** Base class and interface for all dockable panels
 */



#ifndef GUI_PANEL_PANEL_H
#define GUI_PANEL_PANEL_H

#include "gui/gtk-base.hpp"
#include "gui/widget/panel-bar.hpp"

#include <gdlmm.h>

namespace gui {
  namespace workspace {
    class PanelManager;
    class WorkspaceWindow;
  }
  namespace panel {
  
  
  /**
   * The base class for all dockable panels.
   */
  class Panel
    : public Gtk::VBox
    {
    protected:
      
      /**
       * @param panel_manager owner panel manager widget
       * @param dock_item GdlDockItem that will host this panel
       * @param long_name title of this panel
       * @param stock_id ID of this panel
       */
      Panel (workspace::PanelManager&
            ,Gdl::DockItem&
            ,const gchar* longName
            ,const gchar* stockID);
      
    public:
     ~Panel();
      
      /** @return pointer to the underlying GdlDockItem structure */
      Gdl::DockItem& getDockItem();
      
      
      /** Shows or hides the panel. */
      void show(bool show = true);
      bool is_shown() const;
      
      void iconify();
      bool is_iconified() const;
      
      /** Locks or unlocks the panel against modifications */
      void lock(bool show = true);
      bool is_locked() const;
      
      
      /** @return the owner */
      workspace::PanelManager& getPanelManager();
      
      
      /** fires when the dock item gets hidden. */
      sigc::signal<void>& signal_hidePanel();
      
    protected:
      
      workspace::WorkspaceWindow& getWorkspaceWindow();
      
    private:
      
      /** event handler when dockItem is hidden. */
      void on_item_hidden();
      
    protected:
      
      /** The owner panel manager object. */
      workspace::PanelManager& panelManager_;
      
      /** owner dock item widget that will host the widgets in this panel. */
      Gdl::DockItem& dockItem_;
      
      /** signal that fires when the dock item is hidden. */
      sigc::signal<void> hidePanelSignal_;
      
      /** panel bar to attach to the panel grip.*/
      widget::PanelBar panelBar_;
    };
  
  
}}// namespace gui::panel
#endif /*GUI_PANEL_PANEL_H*/
