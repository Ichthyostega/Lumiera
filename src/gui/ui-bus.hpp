/*
  UI-BUS.hpp  -  UI model and control backbone

  Copyright (C)         Lumiera.org
    2009,               Joel Holdsworth <joel@airwebreathe.org.uk>
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file ui-bus.hpp
 ** Under construction: the top level UI controller.
 ** The Lumiera GTK GUI is built around a generic backbone structure known as **UI-Bus**.
 ** This is a messaging system and taps into any widget of more than local relevance.
 ** To that end, any globally relevant (custom) widget, and all secondary controllers
 ** inherit from the gui::model::Tangible base. The top-level gui::UiBus element is
 ** a front-end and framework component managed by the [GTK-main][GtkLumiera::main].
 ** 
 ** @warning as of 12/2015, this is still totally a mess. This \em will remain
 **          the one-and-only master controller of the UI, but I am determined
 **          to change the architecture and implementation technique altogether.
 **          For the time being, we keep the controller::Controller in place, as
 **          written by Joel Holdsworth, while building the new UI-Bus frontend
 **          to take on this central role eventually.
 ** 
 ** 
 ** @todo as of 1/2015, this needs to be reshaped ////////////////////TICKET #959
 */


#ifndef GUI_UI_BUS_H
#define GUI_UI_BUS_H


#include "gui/gtk-lumiera.hpp"  //////////////////////////////////////////////////////TODO remove any GTK dependency if possible
#include "gui/ctrl/playback-controller.hpp"

#include <boost/noncopyable.hpp>


namespace gui {
  ///////////////////////////////////////////////////////////////////////////////////TICKET #959 : scheduled for termination....
  namespace model {
    class Project;
  } // namespace model
    
  namespace controller { 
    
    /**
     * @todo needs to be reshaped for communication with Proc-Layer /////////////////TICKET #959
     */
    class Controller
      {
        model::Project&    project_;
        PlaybackController playback_;
        
      public:
        Controller (model::Project&);
      
        PlaybackController& get_playback_controller();
      };
  
}// namespace gui::controller
  ///////////////////////////////////////////////////////////////////////////////////TICKET #959 : scheduled for termination....
  
  
  /**
   * Backbone of the Lumiera GTK GUI.
   * This is the Interface and Lifecycle front-end.
   * When an instance of this class is created, the backbone becomes operative
   * and is linked to the active gui::WindowManager. When it goes away, the
   * backbone service switches into disabled mode, awaiting disconnection
   * of all remaining clients. After that, it dissolves into nothingness.
   */
  class UiBus
    : boost::noncopyable
    {
    public:
      UiBus();
     ~UiBus();
     
    };
  
  
  
}// namespace gui
#endif /*GUI_UI_BUS_H*/

