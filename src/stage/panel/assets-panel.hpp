/*
  assets-panel.hpp  -  Definition of the assets panel            

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file assets-panel.hpp
 ** A (dockable) panel to organise the assets of a project.
 ** Assets are descriptor objects maintained within the Session model.
 ** They are used to represent
 ** - media and clips
 ** - processors, effects etc.
 ** - structural elements like timeline, tracks, sequences etc.
 ** - metadata descriptors
 */



#ifndef STAGE_PANEL_ASSETS_PANEL_H
#define STAGE_PANEL_ASSETS_PANEL_H


#include "stage/panel/panel.hpp"

namespace stage  {
namespace panel{
  
  class AssetsPanel
    : public Panel
    {
    public:
      /** Build a new Asset-Panel
       * @param PanelManager The owner panel manager widget.
       * @param DockItem The GdlDockItem that will host this panel.
       */
      AssetsPanel (workspace::PanelManager&, Gdl::DockItem&);
      
      
      static const char* getTitle();
      static const gchar* getStockID();
      
      
    protected:
      Gtk::Notebook notebook_;
      
      Gtk::IconView media_;
      Gtk::IconView clips_;
      Gtk::IconView effects_;
      Gtk::IconView transitions_;
    };
  
  
}}// namespace stage::panel
#endif /*STAGE_PANEL_ASSETS_PANEL_H*/
