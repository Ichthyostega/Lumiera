/*
  PLAY-SERVICE.hpp  -  interface: render- and playback control

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file play-service.hpp
 ** Player subsystem......
 **
 ** @see lumiera::DummyPlayer
 ** @see gui::PlaybackController usage example 
 */


#ifndef PROC_PLAY_PLAY_SERVICE_H
#define PROC_PLAY_PLAY_SERVICE_H


#include "lib/error.hpp"
#include "include/play-facade.h"
#include "common/interface-facade-link.hpp"

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
//#include <string>


namespace proc {
namespace play {

  using std::string;
  using lumiera::facade::InterfaceFacadeLink;
//using lumiera::Subsys;
//using lumiera::Display;
//using lumiera::DummyPlayer;
  using boost::scoped_ptr;
  
  
//  class DummyImageGenerator;
//  class TickService;
  class ProcessTable;
  
  
  
  /******************************************************
   * Implementation access point: Player subsystem.
   * The PlayService is the primary way for clients to
   * get into using Lumiera's Play/Output Subsystem.
   * It allows to \em perform a timeline or model object.
   * Behind the scenes, this will create and outfit a
   * PlayProcess, which is accessible through the
   * Play::Controller returned as frontend/handle.
   * 
   * @internal the PlayService is instantiated and owned
   * by the OutputDirector, which acts as a central hub
   * for the Player subsystem. Clients should always
   * access this functionality through the
   * lumiera::Play facade interface. 
   */
  class PlayService
    : public lumiera::Play
    , boost::noncopyable
    {
      InterfaceFacadeLink<lumiera::Play> facadeAccess_;
      scoped_ptr<ProcessTable>           pTable_;
      
      
      /** Implementation: build a PlayProcess */
      virtual Controller connect(ModelPorts, Output);
      
      
    public:
      PlayService();   /////TODO Subsys::SigTerm terminationHandle);
      
     ~PlayService();    /////TODO notifyTermination_(&error_); }
      
    };
  
  
  
  
}} // namespace proc::play
#endif
