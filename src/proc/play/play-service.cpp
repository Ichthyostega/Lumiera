/*
  PlayService  -  interface: render- and playback control

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

* *****************************************************/


#include "proc/play/play-service.hpp"
#include "lib/singleton.hpp"


#include <string>
//#include <memory>
//#include <tr1/functional>
//#include <boost/scoped_ptr.hpp>



namespace lumiera {
  
  
  Play::~Play() { } // emit VTables here...
  
  
  
}//(End) namespace lumiera


namespace proc {
namespace play {

//using std::string;
//using lumiera::Subsys;
//using std::auto_ptr;
//using boost::scoped_ptr;
//using std::tr1::bind;
  
  
  namespace { // hidden local details of the service implementation....
  
    
    
    
    
    
    /* ================== define an lumieraorg_DummyPlayer instance ======================= */
    
    
  } // (End) hidden service impl details
  
  
  using lumiera::Play;
  
  
  /** bring up the global render- and playback service.
   *  This service allows to create individual PlayProcess instances
   *  to \em perform a timeline or similar model object, creating
   *  rendered data for output. Client code is assumed to access
   *  this service through the lumiera::Play facade. 
   */
  PlayService::PlayService()   /////TODO Subsys::SigTerm terminationHandle);
    : facadeAccess_(*this, "Player")
    { }
  
  
  
  /**
   * @note this is the core operation of the play and render service
   * 
   * Invoking this function investigates the given exit nodes of the
   * render nodes network and retrieves actual output destinations
   * through the given OutputManager. The goal is to configure an 
   * PlayProcess, based on the renderengine and the collection of
   * OutputSlot instances retrieved for each of the given exit nodes.
   * Running this PlayProcess will activate the render engine to deliver
   * calculated media data to the outputs. 
   */
  Play::Controller
  PlayService::connect(ModelPorts dataGenerators, Output outputDestinations)
  {
    UNIMPLEMENTED ("build a PlayProcess");
  }


}} // namespace proc::play
