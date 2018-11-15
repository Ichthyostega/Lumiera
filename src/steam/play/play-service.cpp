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


/** @file play-service.cpp
 ** Implementation of core parts of the Player subsystem
 */


#include "lib/error.hpp"
#include "include/play-facade.hpp"
#include "steam/play/play-service.hpp"
#include "steam/play/play-process.hpp"
#include "steam/play/render-configurator.hpp"
#include "steam/play/output-manager.hpp"
#include "lib/util-foreach.hpp"


#include <string>
//#include <memory>
////#include <functional>
#include <memory>



namespace lumiera {
  
  
  Play::~Play() { } // emit VTables here...
  
  /** static storage for the facade access front-end */
  lib::Depend<Play> Play::facade;
  
}//(End) namespace lumiera


namespace proc {
namespace play {

//using std::string;
//using lumiera::Subsys;
//using std::bind;
  using lib::Sync;
  using lib::RecursiveLock_NoWait;
  using std::weak_ptr;
  using std::bind;
//using std::function;
  using std::placeholders::_1;
  using util::remove_if;
  using util::and_all;

  
  namespace { // hidden local details of the service implementation....
  
    
    
    
    
    
    
  } // (End) hidden service impl details
  
  using lumiera::Play;
  
  typedef POutputManager Output;
  
  
  
  class ProcessTable
    : public Sync<RecursiveLock_NoWait>
    {
      typedef weak_ptr<PlayProcess> Entry;
      typedef std::vector<Entry> ProcTable;
      
      /** @note holding just weak references
       *  to any ongoing playback processes */
      ProcTable processes_;
      
    public:
      
      Play::Controller
      establishProcess (PlayProcess* newProcess)
        {
          Play::Controller frontend;
          try {
              frontend.activate (newProcess, bind (&ProcessTable::endProcess, this, _1 ));
            }
          catch(...)
            {
              delete newProcess;
              throw;
            }
          
          Lock sync(this);
          processes_.push_back (frontend); // keeping a weak-reference
          return frontend;
        }
      
      
      bool
      isActive()  const
        {
          return not and_all (processes_, isDead);
        }
      
    private:
      void
      endProcess (PlayProcess* dyingProcess)
        {
                                     /////////////////////////////////////////////TICKET #867 : somehow ensure sane abort of all attached calculation efforts
          delete dyingProcess;
          
          Lock sync(this);
          remove_if (processes_, isDead);
        }
      
      static bool
      isDead (Entry const& e)
        {
          return e.expired();
        }
    };
  
  
    
  
  PlayService::~PlayService()
    {
      if (pTable_->isActive())
        {
          UNIMPLEMENTED ("block waiting for any ongoing play processes. Ensure the process table is empty -- OR -- hand it over to some kind of cleanup service");
        }
      ENSURE (!pTable_->isActive());
    }
  
  
  /** bring up the global render- and playback service.
   *  This service allows to create individual PlayProcess instances
   *  to \em perform a timeline or similar model object, creating
   *  rendered data for output. Client code is assumed to access
   *  this service through the lumiera::Play facade.
   */
  PlayService::PlayService()
    : pTable_(new ProcessTable)
    { }
  
  
  
  
  
  /**
   * @note this is the core operation of the play and render service
   * 
   * Invoking this function investigates the given exit nodes of the
   * render nodes network and retrieves actual output destinations
   * through the given OutputManager. The goal is to configure a
   * PlayProcess, based on the renderengine and the collection of
   * OutputSlot instances retrieved for each of the given exit nodes.
   * Running this PlayProcess will activate the render engine to deliver
   * calculated media data to the outputs.
   */
  Play::Controller
  PlayService::connect (ModelPorts dataGenerators, POutputManager outputPossibilities)
  {
    Timings playbackTimings (lib::time::FrameRate::PAL);       //////////////////////////////////////////////////////TICKET #875
    
    return pTable_->establishProcess(
            PlayProcess::initiate(dataGenerators, 
                buildRenderConfiguration(outputPossibilities, playbackTimings)));
  }
  
  
  
  /** */
  
  
  
  LUMIERA_ERROR_DEFINE (CANT_PLAY, "unable to build playback or render process for this configuration");
 


}} // namespace proc::play


namespace lumiera {
  
  /* ==== convenience shortcuts for creating a PlayProcess ==== */
  
  /**
   * Generic point-of-Service for starting playback.
   * Activating this service will "perform" the given exit points
   * of the model, by "pulling" calculated data from these ports and
   * feeding the results into suitable external outputs.
   * @return a state machine front-end to control the ongoing
   *         play/render process.
   */
  Play::Controller
  Play::perform (ModelPorts ports, Output output)
  {
    return this->connect (ports, output);
  }
  
  
  /**
   * 
   */
  Play::Controller
  Play::perform (Pipes, Output)
  {
    UNIMPLEMENTED ("build PlayProcess based on a set of pipes");
  }
  
  
  /**
   * 
   */
  Play::Controller
  Play::perform (Timeline)
  {
    UNIMPLEMENTED ("build PlayProcess for a Timeline");
  }
  
  
  /**
   * 
   */
  Play::Controller
  Play::perform (Viewer)
  {
    UNIMPLEMENTED ("build PlayProcess directly for a Viewer element");
  }
  
  
  /**
   * 
   */
  Play::Controller
  Play::perform (Fork)
  {
    UNIMPLEMENTED ("build PlayProcess for a single Fork");
  }
  
  
  /**
   * 
   */
  Play::Controller
  Play::perform (Clip)
  {
    UNIMPLEMENTED ("build virtual Timeline and PlayProcess to show a single Clip");
  }
  
  
}
