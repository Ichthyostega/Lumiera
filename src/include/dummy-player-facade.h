/*
  DUMMY-PLAYER-FACADE.hpp  -  access point to a dummy test player
 
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


#ifndef PROC_INTERFACE_DUMMYPLAYER_H
#define PROC_INTERFACE_DUMMYPLAYER_H

#include "include/display-facade.h"


struct lumiera_playprocess_struct { };
typedef struct lumiera_playprocess_struct lumiera_playprocess;
typedef lumiera_playprocess* LumieraPlayProcess;




#ifdef __cplusplus  /* ============== C++ Interface ================= */

#include "common/subsys.hpp"
#include "include/interfaceproxy.hpp"
#include "lib/handle.hpp"

#include <boost/noncopyable.hpp>



namespace proc {
  namespace play {
    
    
    class ProcessImpl;
    
    
    /******************************************************************
     * Interface Proc-Layer (or maybe the backend?): 
     * Global access point for starting a dummy playback, generating
     * some test image data for the GUI to display in a viewer window.
     * 
     * This is a mockup service we created 1/2009 to collect some
     * experiences regarding integration of the application layers.
     * Lumiera is not yet able actually to deliver rendered video data.
     * 
     */
    class DummyPlayer
      {
      public:
        /** provide a descriptor for lumiera::AppState,
         *  wired accordingly to allow main to deal with
         *  the dummy player as independent subsystem. */
        static lumiera::Subsys& getDescriptor();
        
        /** get an implementation instance of this service */
        static lumiera::facade::Accessor<DummyPlayer> facade;
        
        
        /**
         * Continuous playback process, which has been started with a specific
         * output size, format and framerate. It is a handle to a calculation process,
         * which is about to produce a stream of frames to be retrieved by calling
         * the #getFrame function on this handle.
         * 
         * The Lifecycle of the referred playback process is managed automatically
         * through this handle (by ref count).
         * @see handle.hpp 
         * @see dummy-player-service.cpp implementation
         */
        class Process
          : public lib::Handle<ProcessImpl>
          {
          public:
            void        play(bool);
            void* const getFrame();
            
          };
        
        
        //////////////////TODO: define some dummy negotiation about size and framerate....
        
        virtual Process start(LumieraDisplaySlot viewerHandle)   =0;
        
        virtual ~DummyPlayer();
      };
    
    

  } // namespace play
  
} // namespace proc



extern "C" {
#endif /* =========================== CL Interface ===================== */


#include "common/interface.h"

LUMIERA_INTERFACE_DECLARE (lumieraorg_DummyPlayer, 0
                          , LUMIERA_INTERFACE_SLOT (LumieraPlayProcess, startPlay, (LumieraDisplaySlot)      )
                          , LUMIERA_INTERFACE_SLOT (void,               togglePlay,(LumieraPlayProcess, bool))
                          , LUMIERA_INTERFACE_SLOT (void,               terminate, (LumieraPlayProcess)      )
                          , LUMIERA_INTERFACE_SLOT (void *,             getFrame,  (LumieraPlayProcess)      )
);


#ifdef __cplusplus
}
#endif
#endif
