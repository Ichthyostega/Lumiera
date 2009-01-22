/*
  DUMMY-PLAYER-SERVICE.hpp  -  service implementing a dummy test player
 
 
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

/** @file notification-service.hpp
 ** A public service provided by the GUI, implementing the gui::GuiNotification facade interface.
 ** The purpose of this service is to push state update and notification of events from the lower
 ** layers into the Lumiera GUI. Typically, this happens asynchronously and triggered by events
 ** within the lower layers.
 ** 
 ** This service is the implementation of a layer separation facade interface. Clients should use
 ** gui::GuiNotification#facade to access this service. This header defines the interface used
 ** to \em provide this service, not to access it.
 **
 ** @see gui::GuiFacade
 ** @see guifacade.cpp starting this service 
 */


#ifndef PROC_DUMMYPLAYER_SERVICE_H
#define PROC_DUMMYPLAYER_SERVICE_H


#include "include/dummy-player-facade.h"
#include "common/instancehandle.hpp"
#include "lib/singleton-ref.hpp"

#include <string>


namespace proc {
  
  using std::string;
  using lumiera::Subsys;
  
  
  class ProcessImpl
    : public DummyPlayer::Process
    {
      void        pause(bool doPlay);
      void* const getFrame();
      
      uint fps_;
      bool play_; 
      
    public:
      ProcessImpl() : fps_(0), play_(false) {}
      
      /* Implementation-level API to be used By DummyPlayerService */
      
      /** activate a playback process 
       *  with given specification */
      void setRate (uint fps);
      
      bool isActive () { return fps_ != 0; }
      bool isPlaying() { return play_; }
    };
   
  
  /******************************************************
   * Actual implementation of the GuiNotification service
   * within the Lumiera GTK GUI. Creating an instance of
   * this class automatically registers the interface
   * with the Lumiera Interface/Plugin system and creates
   * a forwarding proxy within the application core to
   * route calls through this interface.
   * 
   * @todo the ctor of this class should take references
   *       to any internal service providers within the
   *       GUI which are needed to implement the service. 
   */
  class DummyPlayerService
    : public DummyPlayer
    {
      
      /* === Implementation of the Facade Interface === */
      
      Process& start();
      
      
      /** for now we use an single inline Process...
       *  @todo actually implement multiple independent Playback processes!
       *  @todo I am aware holding this object inline may cause a segfault at shutdown! 
       */
      ProcessImpl theProcess_;
      
      string error_;
      Subsys::SigTerm notifyTermination_;
      
      
      
      /* === Interface Lifecycle === */
      
      typedef lumiera::InstanceHandle< LUMIERA_INTERFACE_INAME(lumieraorg_DummyPlayer, 0)
                                     , DummyPlayer
                                     > ServiceInstanceHandle;
      
      lib::SingletonRef<DummyPlayer> implInstance_;
      ServiceInstanceHandle serviceInstance_;
      
    public:
      DummyPlayerService(Subsys::SigTerm terminationHandle);
      
     ~DummyPlayerService() { notifyTermination_(&error_); }
      
    };
    
  
  
} // namespace proc
#endif
