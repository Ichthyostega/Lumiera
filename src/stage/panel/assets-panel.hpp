/*
  assets-panel.hpp  -  Definition of the assets panel            

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


/** @file assets-panel.hpp
 ** A (dockable) panel to organise the assets of a project.
 ** Assets are descriptor objects maintained within the Session model.
 ** They are used to represent
 ** - media and clips
 ** - processors, effects etc.
 ** - structural elements like timeline, tracks, sequences etc.
 ** - metadata descriptors
 */



#ifndef GUI_PANEL_ASSETS_PANEL_H
#define GUI_PANEL_ASSETS_PANEL_H


#include "stage/panel/panel.hpp"

namespace gui  {
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
  
  
}}// namespace gui::panel
#endif /*GUI_PANEL_ASSETS_PANEL_H*/
