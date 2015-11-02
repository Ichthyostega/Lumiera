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
 ** The top level UI controller.
 ** 
 ** @todo as of 1/2015, this needs to be reshaped ////////////////////TICKET #959
 */


#ifndef GUI_UI_BUS_H
#define GUI_UI_BUS_H


#include "gui/gtk-lumiera.hpp"  //////////////////////////////////////////////////////TODO remove any GTK dependency if possible
#include "gui/ctrl/playback-controller.hpp"


namespace gui {
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
  
}}// namespace gui::controller
#endif /*GUI_UI_BUS_H*/

