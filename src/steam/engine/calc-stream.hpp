/*
  CALC-STREAM.hpp  -  abstraction representing a series of scheduled calculations

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


/** @file calc-stream.hpp
 ** A core abstraction within the render engine to represent an ongoing calculation.
 ** Within the Lumiera render engine, the rendering process is modelled in a functional way,
 ** as a _stream of calculations_ -- which eventually result in a stream of data frames.
 ** Typically, streams in this context are conceived as unlimited and open-ended, and
 ** can be represented by a generator or continuation function. Moreover, the invocation
 ** of this "continue-the-stream"-function is embedded into the other rendering calculations
 ** as a series of planning jobs. Whose purpose is, to create further future calculation jobs,
 ** including future planning jobs.
 ** 
 ** All of this structure thus exists only dynamically, always in ongoing evaluation.
 ** For the interface level we thus create a symbolic placeholder, the CalcStream object
 ** 
 ** @see engine-service.hpp
 ** @todo this draft was created in 2013 just to the point of defining the skeleton of the engine.
 ** @todo as of 2016, this is considered the way to go and will be completed _eventually_
 */



#ifndef STEAM_ENGINE_CALC_STREAM_H
#define STEAM_ENGINE_CALC_STREAM_H


#include "lib/error.hpp"
#include "steam/play/timings.hpp"
#include "steam/play/output-slot.hpp"
#include "steam/engine/render-drive.hpp"
//#include "include/dummy-player-facade.h"
//#include "include/display-facade.h"
//#include "common/instancehandle.hpp"
//#include "lib/singleton-ref.hpp"
//
//#include <string>
#include <vector>
#include <memory>


namespace steam {
namespace engine{
  
  namespace error = lumiera::error;

//    using std::string;
//    using lumiera::Subsys;
//    using lumiera::Display;
//    using lumiera::DummyPlayer;

  class RenderEnvironment;

  
  
  
  
  
  /*****************************************************//**
   * A calculation stream groups and abstracts a series of
   * calculation jobs, delivering frames into the configured
   * OutputSlot in a timely fashion. Behind the scenes, this
   * "stream of calculations" will be translated into several
   * jobs enqueued with the scheduler in the Vault Layer.
   * The implementation of the \link Dispatcher frame dispatch
   * step \endlink cares to create and configure these jobs
   * and to manage the necessary dependencies and callbacks.
   * 
   * Regarding the implementation, a CalcStream is an const
   * value object holding the metadata necessary to manage
   * the underlying jobs. The only way to create a CalcStream
   * properly is to retrieve it from the  factory functions
   * of the EngineService. At that point, the corresponding
   * jobs will already be configured and enqueued.
   */
  class CalcStream
    {
      std::shared_ptr<RenderDrive> drive_;
                                       //////////////////////////////////////////////////////////////////////TICKET #1301 : need to pass-on the output sink association (and the ModelPort)
      
    protected:
      CalcStream (RenderEnvironment& abstractEngine)
        : drive_{}/////////////////////////////////////////////////TODO
        { }
      
      friend class EngineService;
      
     
      void
      sendToOutput (play::DataSink)
        {
          UNIMPLEMENTED ("set up dispatcher to start calculating and feeding to the given output sink");
        }
      
      
    public:
      CalcStream()
        : drive_{}
        { }
      
     ~CalcStream() { }
      
      // using standard copy operations
     
     
      
    };
  
  typedef std::vector<CalcStream> CalcStreams;         //////////////////////////////////////////////////////TICKET #1297 : probably unnecessary, since we intend to connect always one Feed per ModelPort (and handle multiple channels internally, in the processing nodes)
      

  
  
  
}}// namespace steam::engine
#endif /*STEAM_ENGINE_CALC_STREAM_H*/
