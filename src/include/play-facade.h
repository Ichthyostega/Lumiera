/*
  PLAYER-FACADE.h  -  access point to the Lumiera player subsystem

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


#ifndef PROC_INTERFACE_PLAY_H
#define PROC_INTERFACE_PLAY_H




#ifdef __cplusplus  /* ============== C++ Interface ================= */

//#include "include/interfaceproxy.hpp"
#include "lib/handle.hpp"
#include "lib/iter-source.hpp"
#include "lib/time/control.hpp"
#include "lib/time/timevalue.hpp"
#include "include/interfaceproxy.hpp"
#include "proc/mobject/model-port.hpp"
#include "proc/mobject/output-designation.hpp"
#include "proc/mobject/session/clip.hpp"
#include "proc/mobject/session/track.hpp"
#include "proc/play/output-manager.hpp"
#include "proc/asset/timeline.hpp"
#include "proc/asset/viewer.hpp"



namespace proc {
namespace play {
  
  class PlayProcess;
}}


namespace lumiera {
  
  namespace time = lib::time;
    
    
    /******************************************************************
     * Interface to the Player subsystem of Lumiera (Proc-Layer). 
     * Global access point for starting playback and render processes,
     * calculating media data by running the render engine.
     * 
     * @todo WIP-WIP-WIP 6/2011
     * @note Lumiera is not yet able actually to deliver rendered data.
     * @todo there should be an accompanying CL Interface defined for
     *       the Lumiera interface system, so the player can be 
     *       accessed from external clients. This was left out
     *       for now, as we don't have either plugins, nor
     *       any script running capabilities yet. (5/2011)
     */
    class Play
      {
      public:
        
        /** get an implementation instance of this service */
        static lumiera::facade::Accessor<Play> facade;
        
        
        /**
         * Continuous playback process, which has been hooked up
         * and started with a fixed set of output slots. started with a specific
         * output size, format and framerate. It is a handle to a calculation process,
         * which is about to produce a stream of frames and push them to the outputs.
         * 
         * The Lifecycle of the referred playback process is managed automatically
         * through this handle (by ref count). Client code is supposed to use the
         * API on this handle to navigate and control the playback mode.
         * 
         * @see handle.hpp 
         * @see player-service.cpp implementation
         */
        class Controller
          : public lib::Handle<proc::play::PlayProcess>
          {
          public:
            void play(bool);          ///< play/pause toggle
            void scrub(bool);         ///< scrubbing playback
            void adjustSpeed(double); ///< playback speed control
            void go(lib::time::Time); ///< skip to the given point in time
            
            void controlPlayhead (time::Control<time::Time>     & ctrl);
            void controlDuration (time::Control<time::Duration> & ctrl);
            void controlLooping  (time::Control<time::TimeSpan> & ctrl);
            
            void useProxyMedia (bool);
            void setQuality (uint);
            
            bool is_playing()   const;
            bool is_scrubbing() const;
            double getSpeed()   const;
            uint getQuality()   const;
            bool usesProxy()    const;
          };
        
        
        typedef lib::IterSource<mobject::ModelPort>::iterator    ModelPorts;
        typedef lib::IterSource<mobject::OutputDesignation>::iterator Pipes;
        typedef proc::play::POutputManager Output;
        typedef mobject::session::PClipMO Clip;
        typedef mobject::PTrack  Track;
        typedef asset::PTimeline Timeline;
        typedef asset::PViewer Viewer;
        
        /** core operation: create a new playback process
         *  outputting to the given viewer/display  */
        virtual Controller connect(ModelPorts, Output)      =0;
        
        
        /* ==== convenience shortcuts for common use cases ==== */
        Controller perform(Pipes, Output);
        Controller perform(Timeline);
        Controller perform(Viewer);
        Controller perform(Track);
        Controller perform(Clip);
        
      protected:
        virtual ~Play();
      };
    
    
} // namespace lumiera




extern "C" {
#endif /* =========================== CL Interface ===================== */


// #include "common/interface.h"
////////////////////////////////////TODO define a C binding for the Interface system here


#ifdef __cplusplus
}
#endif
#endif
