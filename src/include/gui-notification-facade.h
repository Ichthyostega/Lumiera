/*
  GUI-NOTIFICATION-FACADE.h  -  access point for pushing information into the GUI

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file gui-notification-facade.h
 ** Major public Interface of the Lumiera GUI. While from a user's point of view,
 ** the GUI controls the application and thus acts on its own, in fact the UI sends
 ** command messages to the Session subsystem in Steam-Layer. These commands cause
 ** changes in the session, which result in notification and structure change messages
 ** being pushed up asynchronously back into the UI. The GuiNotification interface
 ** abstracts this ability of the UI to receive such update messages. It is implemented
 ** by the NotificationService within the GUI Layer, which causes actual tangible changes
 ** to happen in the UI in response to the reception of these messages.
 **
 ** @see notification-service.hpp implementation
 ** @see gui::GuiFacade
 ** @see main.cpp
 */


#ifndef STAGE_GUI_NOTIFICATION_H
#define STAGE_GUI_NOTIFICATION_H



#ifdef __cplusplus  /* ============== C++ Interface ================= */

#include "lib/depend.hpp"
#include "lib/idi/entry-id.hpp"

#include <string>


namespace lib {
  namespace diff {
    class GenNode;
    class MutationMessage;
} }
namespace gui {
  
  using std::string;
  using lib::diff::GenNode;
  using lib::diff::MutationMessage;
  
  using ID = lib::idi::BareEntryID const&;
  
  enum NotifyLevel {
      NOTE_INFO,   ///< possibly interesting info that can be safely ignored
      NOTE_WARN,   ///< something to be aware of, to be indicated unobtrusively
      NOTE_ERROR   ///< severe condition to be indicated prominently
  };
  
  
  /*****************************************************************//**
   * Global access point to push state update and notification of events
   * from the lower layers into the Lumiera GUI. Typically, this happens
   * asynchronously and triggered by events within the lower layers.
   * 
   * This is a layer separation façade interface. Clients should use
   * the embedded #facade factory, which yields a proxy routing any
   * calls through the lumieraorg_GuiNotification interface
   * @throws lumiera::error::State when interface is not opened
   */
  class GuiNotification
    {
    protected:
      virtual ~GuiNotification() {}   ///< this is an interface
      friend class lib::DependencyFactory<GuiNotification>;
      
    public:
      static lib::Depend<GuiNotification> facade;
      
      
      /** push a user visible notification text */
      virtual void displayInfo (NotifyLevel, string const& text) =0;
      
      /** highlight an element in the UI as problem location */
      virtual void markError (ID uiElement, string const& text)  =0;
      
      /** attach an warning or state information element */
      virtual void markNote  (ID uiElement, string const& text)  =0;
      
      /** send a generic _state mark_ message to some element */
      virtual void mark      (ID uiElement, GenNode&& stateMark) =0;
      
      /** push a diff message up into the user interface.
       * @remark this is the intended way how to populate or
       *  manipulate the contents of the user interface from
       *  lower layers. By sending a _diff message,_ any
       *  structural or content changes can be described
       *  without actually knowing the implementation of
       *  the UI model elements subject to this change.
       * @see diff-language.hpp
       */
      virtual void mutate (ID uiElement, MutationMessage&&)      =0;
      
      /** causes the GUI to shut down unconditionally
       * @param cause user visible explanation of the
       *        reason causing this shutdown
       * @warning since the UI is a subsystem, this call
       *        eventually terminates the whole application.
       */
      virtual void triggerGuiShutdown (string const& cause)      =0;
      
    };
  
  
  
} // namespace gui


extern "C" {
#endif /* =========================== CL Interface ===================== */

  
#include "common/interface.h"
#include "lib/hash-value.h"


LUMIERA_INTERFACE_DECLARE (lumieraorg_GuiNotification, 0,
                           LUMIERA_INTERFACE_SLOT (void, displayInfo,        (uint, const char*)),
                           LUMIERA_INTERFACE_SLOT (void, markError,          (const void*, const char*)),  ////////////TICKET #1175 : need a way to pass EntryID
                           LUMIERA_INTERFACE_SLOT (void, markNote,           (const void*, const char*)),
                           LUMIERA_INTERFACE_SLOT (void, mark,               (const void*, void*)),
                           LUMIERA_INTERFACE_SLOT (void, mutate,             (const void*, void*)),
                           LUMIERA_INTERFACE_SLOT (void, triggerGuiShutdown, (const char*)),
);


#ifdef __cplusplus
}// extern "C"
#endif
#endif /*STAGE_GUI_NOTIFICATION_H*/
