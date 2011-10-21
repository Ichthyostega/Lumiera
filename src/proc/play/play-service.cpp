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
#include "proc/play/play-process.hpp"
#include "lib/util.hpp"


#include <string>
//#include <memory>
//#include <tr1/functional>
#include <tr1/memory>
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
  using lib::Sync;
  using lib::RecursiveLock_NoWait;
  using std::tr1::weak_ptr;
  using std::tr1::bind;
//using std::tr1::function;
  using std::tr1::placeholders::_1;
  using util::remove_if;
  
  
  namespace { // hidden local details of the service implementation....
  
    
    
    
    
    
    
  } // (End) hidden service impl details
  
  
  class ProcessTable
    : public Sync<RecursiveLock_NoWait>
    {
      typedef weak_ptr<PlayProcess> Entry;
      typedef std::vector<Entry> ProcTable;
      
      ProcTable processes_;
      
    public:
      
      lumiera::Play::Controller
      establishProcess (PlayProcess* newProcess)
        {
          lumiera::Play::Controller frontend;
          
          Lock sync(this);
          frontend.activate (newProcess, bind (&ProcessTable::endProcess, this, _1 ));
          processes_.push_back (frontend);
          return frontend;
        }
      
    private:
      void
      endProcess (PlayProcess* dyingProcess)
        {
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
  
  
  using lumiera::Play;
  
  
  PlayService::~PlayService() { }
  
  
  /** bring up the global render- and playback service.
   *  This service allows to create individual PlayProcess instances
   *  to \em perform a timeline or similar model object, creating
   *  rendered data for output. Client code is assumed to access
   *  this service through the lumiera::Play facade. 
   */
  PlayService::PlayService()
    : facadeAccess_(*this, "Player")
    , pTable_(new ProcessTable)
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
  PlayService::connect (ModelPorts dataGenerators, Output outputDestinations)
  {
    return pTable_->establishProcess(
            PlayProcess::initiate(dataGenerators, outputDestinations));
  }

  
  
  LUMIERA_ERROR_DEFINE (CANT_PLAY, "unable to build playback or render process for this configuration");
 


}} // namespace proc::play


namespace lumiera {
  
  /* ==== convenience shortcuts for creating a PlayProcess ==== */
  
  /**
   * 
   */
  Play::Controller
  Play::perform(Pipes, Output)
  {
    UNIMPLEMENTED ("build PlayProcess based on a set of pipes");
  }
  
  
  /**
   * 
   */
  Play::Controller
  Play::perform(Timeline)
  {
    UNIMPLEMENTED ("build PlayProcess for a Timeline");
  }
  
  
  /**
   * 
   */
  Play::Controller
  Play::perform(Viewer)
  {
    UNIMPLEMENTED ("build PlayProcess directly for a Viwer element");
  }
  
  
  /**
   * 
   */
  Play::Controller
  Play::perform(Track)
  {
    UNIMPLEMENTED ("build PlayProcess for a single Track");
  }
  
  
  /**
   * 
   */
  Play::Controller
  Play::perform(Clip)
  {
    UNIMPLEMENTED ("build virtual Timeline and PlayProcess to show a single Clip");
  }
  
  
}
