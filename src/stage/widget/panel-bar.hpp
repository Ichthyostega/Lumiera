/*
  PANEL-BAR.hpp  -  container to place widgets into the GDL dock attachment area

  Copyright (C)         Lumiera.org
    2009,               Joel Holdsworth <joel@airwebreathe.org.uk>

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

/** @file panel-bar.hpp
 ** Custom container widget to allow placing toolbar buttons
 ** into the active docking header area of GDL docking panels.
 ** This arrangement allows us to save a significant amount
 ** of screen real estate
 ** 
 */


#ifndef STAGE_WIDGET_PANEL_BAR_H
#define STAGE_WIDGET_PANEL_BAR_H


#include "stage/widget/menu-button.hpp"

#include <gtkmm.h>


namespace gui {
  namespace panel {
    class Panel;
  }
  
  namespace widget {
  
  
  
  /**
   * A container widget for widgets
   * to be displayed on GDL panels grips.
   */
  class PanelBar
    : public Gtk::Box
    {
      /** reference to the owner panel */
      panel::Panel& panel_;
      
      /** The panel menu drop-down button widget,
       *  that will be displayed in the corner of the bar. */
      MenuButton panelButton_;
      
      /** pointer to the lock menu item.
       * @remarks This value will remain NULL until
       *    after setup_panel_button has been called. */
      Gtk::CheckMenuItem* lockItem_;
      
      /** The bar window.
       * @remarks This window is used only to set the cursor
       *    as an arrow for any child widgets. */
      Glib::RefPtr<Gdk::Window> window_;
      
      
    public:
      /** create a PanelBar with a given stock_id.
       * @param owner_panel The GDL panel that is the parent of this panel bar.
       * @param stock_id The stock id with a name and an icon for this panel.
       */
      PanelBar (panel::Panel& owner_panel, const gchar *stock_id);
      
      
      
    private:
      /**
       * Sets up panelButton, populating it with menu items.
       */
      void setupPanelButton();
      
      
      /**
       * An event handler for when a panel type is chosen.
       * @param type_index index of the panel description that will be instantiated.
       */
      void on_panel_type (int type_index);
      
      /**
       * An event handler for when the "Hide" menu item is clicked
       */
      void on_hide();
      
      /**
       * Event handler for when the "Lock" menu item is clicked
       */
      void on_lock();
      
      /**
       * Event handler for when the split panel menu item is clicked
       * @param split_direction The direction to split in.
       */
      void on_split_panel (Gtk::Orientation split_direction);
    };
  
  
  
}}// gui::widget
#endif /*STAGE_WIDGET_PANEL_BAR_H*/
