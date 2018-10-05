/*
  infobox-panel.hpp  -  A dockable panel to expose information and parameters

  Copyright (C)         Lumiera.org
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

*/


/** @file infobox-panel.hpp
 ** A (dockable) panel to display and manage information and parameters.
 ** Such an »Info Box« typically exposes detail settings from some other component
 ** currently selected, and allows to access those in a non modal fashion.
 ** 
 ** @todo as of 8/2017 this is (ab)used as space for UI / Proc-Layer integration experiments
 */



#ifndef GUI_PANEL_INFOBOX_PANEL_H
#define GUI_PANEL_INFOBOX_PANEL_H


#include "gui/panel/panel.hpp"

#include <memory>

namespace gui  {
namespace widget {
  class ErrorLogDisplay;
}
namespace panel{
  
  class InfoBoxPanel
    : public Panel
    {
    public:
      /** Build a new InfoBox-Panel
       * @param PanelManager The owner panel manager widget.
       * @param DockItem The GdlDockItem that will host this panel.
       * 
       * @todo as of 8/2017 this is placeholder code for UI experiments...
       */
      InfoBoxPanel (workspace::PanelManager&, Gdl::DockItem&);
      
      
      static const char* getTitle();   ///< @deprecated need better design of the PanelManager /////TICKET #1026
      static const gchar* getStockID();
      
      widget::ErrorLogDisplay& getLog();
      
    private:
      Gtk::Box twoParts_;
      Gtk::ButtonBox buttons_;
      Gtk::Button buttonClear_, buttonClearInfo_, buttonClearErr_;
      Gtk::Frame frame_;
      Gtk::Expander logExpander_;
      
      std::unique_ptr<widget::ErrorLogDisplay> theLog_;
      
      void reflect_LogErrorState (bool);
    };
  
  
}}// namespace gui::panel
#endif /*GUI_PANEL_INFOBOX_PANEL_H*/
