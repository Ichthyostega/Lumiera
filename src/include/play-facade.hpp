/*
  PLAYER-FACADE.h  -  access point to the Lumiera player subsystem

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file play-facade.hpp
 ** Public access point to the _playback service_ provided by the »play-out subsystem«
 ** @todo in theory this could be an external interface mapped via Lumiera's interface system.
 **       This would be much work however, and not serve any tangible goal in the current stage
 **       of development (2018). I take this as just another confirmation that turning everything
 **       into a plug-in does not quality as good architecture: if you want to do it right, it
 **       creates a lot of additional cost. And to do it just superficially would be like cheating.
 */


#ifndef STEAM_INTERFACE_PLAY_H
#define STEAM_INTERFACE_PLAY_H



#include "lib/depend.hpp"
#include "lib/handle.hpp"
#include "lib/iter-source.hpp"                 ////////////////////TICKET #493 : only using the IterSource base interface here
#include "lib/time/control.hpp"
#include "lib/time/timevalue.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/mobject/output-designation.hpp"
#include "steam/mobject/session/clip.hpp"
#include "steam/mobject/session/fork.hpp"
#include "steam/play/output-manager.hpp"
#include "steam/asset/timeline.hpp"
#include "steam/asset/viewer.hpp"



namespace steam {
namespace play {
  
  class PlayProcess;
}}


namespace lumiera {
  
  namespace time = lib::time;
  
  using std::weak_ptr;
    
    
    /**************************************************************//**
     * Interface to the Player subsystem of Lumiera (Steam-Layer).
     * Global access point for starting playback and render processes,
     * calculating media data by running the render engine.
     * 
     * @ingroup player
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
      protected:
        virtual ~Play();
        friend class lib::DependencyFactory<Play>;
      public:
        
        /** get an implementation instance of this service */
        static lib::Depend<Play> facade;
        
        
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
         * @ingroup player
         * @see handle.hpp 
         * @see player-service.cpp implementation
         */
        class Controller
          : public lib::Handle<steam::play::PlayProcess>
          {
          public:
            void play(bool);          ///< play/pause toggle
            void scrub(bool);         ///< scrubbing playback
            void adjustSpeed(double); ///< playback speed control
            void go(time::Time);      ///< skip to the given point in time
            
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
            
            operator weak_ptr<steam::play::PlayProcess>()  const;
          };
        
        
        using ModelPorts = lib::IterSource<steam::mobject::ModelPort>::iterator;
        using Pipes      = lib::IterSource<steam::mobject::OutputDesignation>::iterator;
        using Output     = steam::play::POutputManager;
        using Clip       = steam::mobject::session::PClip;
        using Fork       = steam::mobject::PFork;
        using Timeline   = steam::asset::PTimeline;
        using Viewer     = steam::asset::PViewer;
        
        
        /* ==== convenience shortcuts for common use cases ==== */
        Controller perform(ModelPorts, Output);
        Controller perform(Pipes, Output);
        Controller perform(Timeline);
        Controller perform(Viewer);
        Controller perform(Fork);
        Controller perform(Clip);
        
      protected:
        /** core operation: create a new playback process
         *  outputting to the given viewer/display  */
        virtual Controller connect(ModelPorts, Output)      =0;
      };
    
    
} // namespace lumiera
#endif /*STEAM_INTERFACE_PLAY_H*/
