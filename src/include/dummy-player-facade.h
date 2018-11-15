/*
  DUMMY-PLAYER-FACADE.h  -  access point to a dummy test player

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file dummy-player-facade.h
 *  @deprecated left-over from an early design draft (2009)
 */


#ifndef STEAM_INTERFACE_DUMMYPLAYER_H
#define STEAM_INTERFACE_DUMMYPLAYER_H

#include "include/display-facade.h"
#include "include/display-handles.h"



#ifdef __cplusplus  /* ============== C++ Interface ================= */

#include "common/subsys.hpp"
#include "lib/depend.hpp"
#include "lib/handle.hpp"




namespace steam   {
  namespace play {
    
    class ProcessImpl;
} }


namespace lumiera {
    
    
    /**************************************************************//**
     * Experimental Interface Steam-Layer (or maybe the vault?): 
     * Global access point for starting a dummy playback, generating
     * some test image data for the GUI to display in a viewer window.
     * 
     * This is a mockup service we created 1/2009 to collect some
     * experiences regarding integration of the application layers.
     * Lumiera is not yet able actually to deliver rendered video data.
     * 
     * In hindsight, this design study highlighted some relevant problems
     * with our interface layout and the way we create bindings to the
     * implementation. The moment we attempt to use other abstractions
     * within an interface (as we do here with the Process interface),
     * we're running into serious maintenance and library dependency problems.
     * @deprecated obsoleted design from 2009 and not operative anymore (2018)
     */
    class DummyPlayer
      {
      public:
        /** provide a descriptor for lumiera::AppState,
         *  wired accordingly to allow main to deal with
         *  the dummy player as independent subsystem. */
        static lumiera::Subsys& getDescriptor();
        
        /** get an implementation instance of this service */
        static lib::Depend<DummyPlayer> facade;
        
        class ProcessImplementationLink;
        
        /**
         * Playback process, front-end to be used by client code.
         * This handle represents a continuous playback process, which has been started
         * with a specific  output size, format and framerate. It is a handle to a calculation process,
         * which is about to produce a stream of frames and push them to the viewer widget,
         * specified by a LumieraDisplaySlot when starting this process.
         * 
         * The Lifecycle of the referred playback process is managed automatically
         * through this handle (by ref count). Client code is supposed to use the
         * API on this handle to control the playback mode.
         * 
         * @see handle.hpp 
         * @see dummy-player-service.cpp implementation
         */
        class Process
          : public lib::Handle<ProcessImplementationLink>
          {
          public:
            void play(bool); ///< play/pause toggle
          };
        
        /**
         * Mediator to allow the client to communicate with
         * the Process implementation via the Process handle,
         * without having to map each implementation-level function
         * into the dummy player interface. We can't access the
         * implementation in Steam-Layer without this indirection
         * through a VTable, since a direct call would require
         * us to link against liblumierasteam.so
         */
        class ProcessImplementationLink
          : public lumiera_playprocess
          {
          public:
              virtual ~ProcessImplementationLink(); ///< this is an interface

              virtual Process createHandle()    =0; ///< activate the Process-frontend and link it to the process implementation
              virtual void doPlay(bool yes)     =0; ///< forward the play/pause toggle to the play process implementation
          };
        
        
        
        
        /** create a new playback process outputting to the given viewer/display */
        virtual Process start(LumieraDisplaySlot viewerHandle)   =0;
        
        
      protected:
        virtual ~DummyPlayer();
        friend class lib::DependencyFactory<DummyPlayer>;
      };
    
    
} // namespace lumiera




extern "C" {
#endif /* =========================== CL Interface ===================== */


#include "common/interface.h"

LUMIERA_INTERFACE_DECLARE (lumieraorg_DummyPlayer, 0
                          , LUMIERA_INTERFACE_SLOT (LumieraPlayProcess, startPlay, (LumieraDisplaySlot)      )
                          , LUMIERA_INTERFACE_SLOT (void,               togglePlay,(LumieraPlayProcess, bool))
                          , LUMIERA_INTERFACE_SLOT (void,               terminate, (LumieraPlayProcess)      )
);


#ifdef __cplusplus
}
#endif
#endif
