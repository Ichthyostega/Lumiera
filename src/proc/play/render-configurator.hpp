/*
  RENDER-CONFIGURATOR.hpp  -  strategy to help building an actual render calculation stream

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

/** @file render-configurator.hpp
 ** Strategy holding all the detail knowledge necessary to establish
 ** a running render CalculationStream. 
 ** 
 ** @see engine::EngineService
 ** @see engine::Feed
 ** @see engine::PlayService
 */


#ifndef PROC_PLAY_RENDER_CONFIGURATOR_H
#define PROC_PLAY_RENDER_CONFIGURATOR_H


#include "lib/error.hpp"
//#include "include/dummy-player-facade.h"
//#include "include/display-facade.h"
//#include "common/instancehandle.hpp"
//#include "lib/singleton-ref.hpp"
#include "proc/mobject/model-port.hpp"
//#include "proc/play/output-manager.hpp"
//#include "lib/iter-source.hpp"
//#include "lib/util.hpp"
//
//#include <boost/noncopyable.hpp>
//#include <boost/scoped_ptr.hpp>
//#include <string>
#include <vector>


namespace proc {
namespace play {

//    using std::string;
//    using lumiera::Subsys;
//    using lumiera::Display;
//    using lumiera::DummyPlayer;
//  using util::isnil;
  using proc::mobject::ModelPort;
  
//  typedef proc::play::POutputManager POutputManager;
//  typedef lib::IterSource<ModelPort>::iterator ModelPorts;
  
  namespace error = lumiera::error;
  
  
  /** Strategy for configuring the render process */
  class RenderConfigurator
    {
      
    public:
      virtual ~RenderConfigurator();  ///< this is an interface
      
      
      typedef std::vector<engine::CalcStream> RenderStreams;  ////TODO this belongs rather into class Feed
      
      virtual RenderStreams buildCalculationStreams (ModelPort, OutputSlot&)  =0;
      
    };
    
  
  
  
}} // namespace proc::play
#endif
