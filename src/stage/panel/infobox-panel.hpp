/*
  infobox-panel.hpp  -  A dockable panel to expose information and parameters

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file infobox-panel.hpp
 ** A (dockable) panel to display and manage information and parameters.
 ** Such an »Info Box« typically exposes detail settings from some other component
 ** currently selected, and allows to access those in a non modal fashion.
 ** 
 ** @todo as of 10/2017 this is used as space to host the information and error log
 */



#ifndef STAGE_PANEL_INFOBOX_PANEL_H
#define STAGE_PANEL_INFOBOX_PANEL_H


#include "stage/panel/panel.hpp"

#include <memory>

namespace stage  {
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
       * @todo just used as place for the error log as of 10/2017.
       *       More to come...
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
  
  
}}// namespace stage::panel
#endif /*STAGE_PANEL_INFOBOX_PANEL_H*/
