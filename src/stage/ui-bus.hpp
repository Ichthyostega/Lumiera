/*
  UI-BUS.hpp  -  UI model and control backbone

  Copyright (C)         Lumiera.org
    2009,               Joel Holdsworth <joel@airwebreathe.org.uk>
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file ui-bus.hpp
 ** Under construction: the top level UI controller.
 ** The Lumiera GTK GUI is built around a generic backbone structure known as **UI-Bus**.
 ** This is a messaging system and taps into any widget of more than local relevance.
 ** To that end, any globally relevant (custom) widget, and all secondary controllers
 ** inherit from the stage::model::Tangible base. The top-level stage::UiBus element is
 ** a front-end and framework component managed by the [GTK-main](\ref GtkLumiera::main).
 ** 
 ** @warning as of 12/2016, this is still totally a mess. This \em will remain
 **          the one-and-only master controller of the UI, but I am determined
 **          to change the architecture and implementation technique altogether.
 **          For the time being, we keep the controller::Controller in place, as
 **          written by Joel Holdsworth, while building the new UI-Bus frontend
 **          to take on this central role eventually.
 ** 
 ** # Rationale
 ** The UI-Bus acts as a **mediating backbone**, impersonating the role
 ** of the _Model_ and the _Controller_ in the [MVC-Pattern] in common UI architecture.
 ** 
 ** The MVC-Pattern as such is fine, and probably the best we know for construction of
 ** user interfaces. But it doesn't scale well towards the integration into a larger and
 ** more structured system. There is a tension between the Controller in the UI and other
 ** parts of an application, which as well need to be _in control._ And, even more important,
 ** there is a tension between the demands of UI elements for support by a model, and the
 ** demands to be placed on a core domain model of a large scale application. This tension is
 ** resolved by enacting these roles while transforming the requests and demands into _Messages._
 ** 
 ** Through this architectural decision, we introduce the distinction between the _local, tangible
 ** UI "mechanics"_ on one side, and the common, _generic interaction patterns_ on the other side.
 ** The former, the mere "mechanics" of the UI shall be kept simple and reduced to immediate
 ** feedback and reactions to operating some interface controls. Any actual operations and
 ** actions relevant to the application as a whole, are to be sent as messages into the
 ** UI-Bus. The interface code can assume some "core services" to be available _somewhere;_
 ** these core services will receive the messages, act on them and _respond asynchronously._
 ** 
 ** # Bus interactions
 ** The UI-Bus has a star shaped topology, with a central "bus master" hub, the ["Nexus"](\ref Nexus),
 ** which maintains a routing table. Attachment and detachment of elements can be managed automatically,
 ** since all of the UI-Bus operations _perform within the UI event thread._
 ** 
 ** We distinguish between _up-link messages,_ directed towards some central service
 ** (presentation state management or command invocation) and _down-link messages,_
 ** directed towards individual elements. The interactions at the bus are closely interrelated
 ** with the [elementary UI-Element operations](\ref tangible.hpp).
 ** 
 ** - *act*: send a \ref GenNode representing the action
 **   - the ID is either a globally registered command-ID or an explicitly
 **     [opened](\ref steam::control::SessionCommand::cycle(Symbol,string const&)) command instance ID.
 **   - the payload is a Record<GenNode> holding the actual command arguments
 **   - on reception, an _instance_ (anonymous clone copy) of the command is created, bound
 **     with the arguments and handed over to the SteamDispatcher to be enqueued for execution.
 ** - *note*: send a [GenNode] representing the _state mark;_
 **   some (abstracted) presentation state manager is expected to listen to these messages,
 **   possibly recording state to be restored later. The contents of the _state mark_ message
 **   are implementation defined; knowledge about these is shared between individual widget
 **   implementations and (partially, to some degree) the presentation state manager.
 ** - *mark*: down-link communication to _feed back_ state updates or
 **   to replay previously recorded _state marks._
 **   
 ** @note The *mark* verb can also be used as an (future) extension point to send _generic messages_ --
 **   possibly even to broadcast them to interested subjects, which have been registered with the
 **   \ref Nexus as targeted receivers...
 **   
 ** @warning deliberately the UI-Bus is **not threadsafe**.
 **          Only [Tangible] elements performing in the UI-event thread are allowed to talk to the bus.
 ** 
 ** @see bus-controller.hpp
 ** @see bus-term.hpp
 ** @see ctrl/nexus.hpp
 ** @see ctrl/core-service.hpp
 ** 
 ** [MVC-Pattern]: http://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93controller
 ** 
 ** @todo as of 1/2015, this header needs to be reshaped ////////////////////TICKET #959
 ** 
 */


#ifndef STAGE_UI_BUS_H
#define STAGE_UI_BUS_H


#include "stage/gtk-base.hpp"  //////////////////////////////////////////////////////TODO remove any GTK dependency if possible
#include "stage/ctrl/playback-controller.hpp"
#include "lib/nocopy.hpp"

#include <memory>


using std::unique_ptr;



namespace stage {
  ///////////////////////////////////////////////////////////////////////////////////TICKET #959 : scheduled for termination....
  namespace model {
    class Project;
  } // namespace model
    
  namespace controller { 
    
    /**
     * @todo needs to be reshaped for communication with Steam-Layer ////////////////TICKET #959
     */
    class Controller
      {
        model::Project&    project_;
        ctrl::PlaybackController playback_;
        
      public:
        Controller (model::Project&);
      
        ctrl::PlaybackController& get_playback_controller();
      };
  
}// namespace stage::controller
  ///////////////////////////////////////////////////////////////////////////////////TICKET #959 : scheduled for termination....
  
  namespace ctrl {
    class StateManager;
    class CoreService;
    class BusTerm;
  }
  
  
  /**
   * Backbone of the Lumiera GTK GUI.
   * This is the Interface and Lifecycle front-end.
   * When an instance of this class is created, the backbone becomes operative
   * and can then be used to attach the active workspace::UiManager and similar entities.
   * When it goes away, all backbone services are forced to shut down and disconnect,
   * which means, that at this point, the entire UI must be decommissioned. Effectively
   * this also means that the UiBus object must be run within the GTK event thread
   * and must not be accessed from anywhere else.
   */
  class UiBus
    : util::NonCopyable
    {
      unique_ptr<ctrl::CoreService> coreService_;
      
    public:
      UiBus();
     ~UiBus();
      
      ctrl::BusTerm&      getAccessPoint();
      ctrl::StateManager& getStateManager();
    };
  
  
  
}// namespace stage
#endif /*STAGE_UI_BUS_H*/

