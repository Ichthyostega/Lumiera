/*
  DISPLAY-FACADE.h  -  accessing a display for outputting frames

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

/** @file display-facade.h
 ** Experimental Interface, allowing the Dummy-Player to access the 
 ** video display widget in the GUI. While, generally speaking, the GUI
 ** controls the application and thus acts on its own, it might expose some
 ** services to the lower layers.
 ** 
 ** In the Dummy-Player design study, the lumiera::Display interface serves
 ** to hand over calculated frames to the GUI for displaying them in a viewer.
 ** 
 ** @deprecated This is a first draft as of 1/2009, and likely to be superseded
 **  by a better design, where rather the \em provider of an output facility
 **  registers with the OutputManager in the core.
 ** 
 ** @see [corresponding implementation](display-service.hpp)
 ** @see gui::GuiFacade
 ** @see dummy-player-facade.h
 ** 
 */


#ifndef GUI_INTERFACE_DISPLAY_H
#define GUI_INTERFACE_DISPLAY_H

#include "include/display-handles.h"





#ifdef __cplusplus  /* ============== C++ Interface ================= */

#include "include/interfaceproxy.hpp"
#include "lib/handle.hpp"




namespace lumiera {
  
  
  /**************************************************************//**
   * Interface for outputting frames to an (already allocated) viewer
   * or display. The viewer is addressed by an "slot" handle, which the 
   * client receives from some other public interface. This reflects
   * the fact that is's not up to the client to create an display or
   * viewer; rather they are provided by some internal facility and
   * the client code is handed out an display handle in the course
   * of a larger interaction, like starting a player. This way,
   * when the client code actually is about to produce output, 
   * he can allocate the slot and obtain a Displayer (functor)
   * for pushing the frames out. (Users of the C Language Interface
   * have to carry out these steps manually and also have to care
   * for cleaning up and deallocating the slot).
   * 
   * @note This is a first draft version of a rather important interface.
   *       The current version as of 1/2009 just serves a mockup player
   *       implementation. You can expect this interface to change
   *       considerably if we get at devising the real player.
   * 
   * @see dummy-player-facade.hpp
   * @see gui::PlaybackController 
   * 
   */
  class Display
    {
    public:
      /** get an implementation instance of this service */
      static lumiera::facade::Accessor<Display> facade;
      
      
      /**
       * Functor for pushing frames to the display.
       * While one client is holding such a Sink handle,
       * the corresponding DisplayerSlot is locked for 
       * exclusive use by this client.
       */
      class Sink
        : public lib::Handle<lumiera_displaySlot>
        {
          
        public:
          /** push a frame up to the display, calling
           *  through the CL Interface.
           *  @see DisplayService::allocate */
          inline void
          operator() (LumieraDisplayFrame frame)
            {
              impl().put_ (&impl(),frame);
            }
        };
      
      
      
      /** allocate an already existing display/viewer for output
       *  @return a functor representing the frame sink */
      virtual Sink getHandle(LumieraDisplaySlot)   =0;
      
      
    protected:
      virtual ~Display();
    };
  
  
  
  
} // namespace lumiera



extern "C" {
#endif /* =========================== CL Interface ===================== */


#include "common/interface.h"

LUMIERA_INTERFACE_DECLARE (lumieraorg_Display, 0
                          , LUMIERA_INTERFACE_SLOT (void, allocate,(LumieraDisplaySlot)                     )
                          , LUMIERA_INTERFACE_SLOT (void,  release,(LumieraDisplaySlot)                     )
                          , LUMIERA_INTERFACE_SLOT (void,      put,(LumieraDisplaySlot, LumieraDisplayFrame))
);


#ifdef __cplusplus
}
#endif
#endif
