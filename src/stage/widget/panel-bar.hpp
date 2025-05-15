/*
  PANEL-BAR.hpp  -  container to place widgets into the GDL dock attachment area

   Copyright (C)
     2009,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file panel-bar.hpp
 ** Custom container widget to allow placing toolbar buttons into the active
 ** docking header area of GDL docking panels.
 ** @remark This arrangement allows us to save a significant amount of screen real estate.
 ** 
 ** @todo still not completely ported to GTK-3 and not used as of 5/2025   /////////////////////////////////TICKET #937 : complete Port to GTK-3
 */


#ifndef STAGE_WIDGET_PANEL_BAR_H
#define STAGE_WIDGET_PANEL_BAR_H


#include "stage/gtk-base.hpp"
#include "stage/widget/menu-button.hpp"


namespace stage {
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
      void setupPanelButton();
      
      void on_panel_type (int type_index);
      void on_split_panel (Gtk::Orientation split_direction);
      
      void on_hide()  override;
      void on_lock();
    };
  
  
  
}}// stage::widget
#endif /*STAGE_WIDGET_PANEL_BAR_H*/
