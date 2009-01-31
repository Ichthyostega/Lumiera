/*
  DISPLAY-SERVICE.hpp  -  service providing access to a display for outputting frames
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/

/** @file display-service.hpp
 ** A public service provided by the GUI, implementing the gui::GuiNotification facade interface.
 ** The purpose of this service is to push state update and notification of events from the lower
 ** layers into the Lumiera GUI. Typically, this happens asynchronously and triggered by events
 ** within the lower layers.
 ** 
 ** This service is the implementation of a layer separation facade interface. Clients should use
 ** proc::play::DummyPlayer#facade to access this service. This header defines the interface used
 ** to \em provide this service, not to access it.
 **
 ** @see gui::GuiFacade
 ** @see guifacade.cpp starting this service 
 */


#ifndef GUI_DISPLAY_SERVICE_H
#define GUI_DISPLAY_SERVICE_H


#include "include/display-facade.h"
#include "common/instancehandle.hpp"
#include "lib/singleton-ref.hpp"

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <string>


namespace gui {

  using std::string;
  using lumiera::Display;
  
  
//  class DummyImageGenerator;
  
  
  /********************************************************************
   * Actual implementation of a single displayer slot. Internally,
   * it is connected* via the Glib::Dispatcher to output frames 
   * to a viewer widget executing within the GTK event thread.
   */
  class DisplayerSlot
    : public Display::Displayer,
      boost::noncopyable
    {
      
    public:
      DisplayerSlot() ;
      
      /* Implementation-level API to be used by DisplayService */
      
    };
  
  
  
  /******************************************************
   * Actual implementation of the DisplayService.
   * Creating an instance of this class automatically
   * registers the interface lumieraorg_Display with
   * the Lumiera Interface/Plugin system and creates
   * a forwarding proxy within the application core to
   * route calls through this interface.
   * \par
   * In addition to the Display interface, this class 
   * implements an additional service for the GUI, 
   * allowing actually to set up display slots, which
   * then can be handed out to client code in the
   * course of the play process for outputting frames.
   */
  class DisplayService
    : boost::noncopyable
    {
      
      string error_;
      
      
      /* === Interface Lifecycle === */
      
      typedef lumiera::InstanceHandle< LUMIERA_INTERFACE_INAME(lumieraorg_Display, 0)
                                     , lumiera::Display
                                     > ServiceInstanceHandle;
      
      lib::SingletonRef<DisplayService> implInstance_;
      ServiceInstanceHandle serviceInstance_;
      
    public:
      DisplayService();
      
     ~DummyPlayerService() { }  ///TODO
      
      
      
      /** allocate and lock the given display slot  */
      Display::Displayer getHandle(LumieraDisplaySlot)   =0;
      
    };
  
  
  
  
} // namespace gui
#endif
