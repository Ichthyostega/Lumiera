/*
  CORE-SERVICE.hpp  -  service to address the application core from the UI

   Copyright (C)
     2015,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file core-service.hpp
 ** Dedicated service node within the UI-Bus to handle command invocation
 ** and presentation state. Mostly, the UI-Bus is just a star shaped network
 ** with one central [routing hub][ctrl::Nexus], and serves to distribute
 ** generic state and update messages. But there are some special messages
 ** which need central processing: The command preparation and invocation
 ** messages and the presentation state tracking messages (state marks).
 ** The Nexus is configured such as to forward these special messages
 ** to the [CoreService] terminal, which invokes the dedicated services.
 ** 
 ** # Lifecycle
 ** CoreService is a PImpl to manage all the technical parts of actual
 ** service provision. When it goes down, all services are decommissioned.
 ** A part of these lifecycle technicalities is to manage the setup of the
 ** [UI-Bus main hub](\ref stage::ctrl::Nexus), which requires some trickery,
 ** since both CoreService and Nexus are mutually interdependent from an
 ** operational perspective, since they exchange messages in both directions.
 ** 
 ** In fact, the CoreService even _holds and thus manages_ the Nexus as a
 ** private member, while the latter controls and connects all nodes attached
 ** to the bus at runtime, including CoreService. This crisscross arrangement
 ** ensures sane start-up and shutdown of the whole UI-Bus compound.
 ** 
 ** ## Bus connection and topology
 ** The CoreService plays a central role within the UI, since it represents
 ** _»the application core«_ from the UI layer's viewpoint. But it is not
 ** the bus manager or central router, a role fulfilled by ctrl::Nexus,
 ** the central UI-Bus hub. Every node which has been added into the
 ** routing table in Nexus, can be addressed as a _first class citizen,_
 ** that is, we're able to direct messages towards such an element, knowing
 ** only it's ID. But there is a twist: all connections to the Bus are made
 ** from [bus terminals](\ref ctrl::BusTerm), and each _node_, i.e. each
 ** [tangible model element](\ref model::Tangible) has a BusTerm member and
 ** thus inherits the ability to talk to the bus. But only when _actively_
 ** connected to the bus, a full link and entry in the routing table is
 ** established. The constructor of model::Tangible indeed makes such
 ** a connection right away, while any "free standing" BusTerm just
 ** knows how to talk to the Bus _upstream,_ without establishing
 ** a full link to receive also _downstream_ messages.
 ** 
 ** And _the fine point to note is_ that CoreService just incorporates
 ** a free standing BusTerm, without registering it with the Nexus.
 ** Doing so would be pointless, since CoreService in fact is not a
 ** regular Tangible, rather it fulfils a very special purpose within
 ** the UI. Most of the UI-Bus messages would not make much sense when
 ** directed towards the CoreService. Rather, CoreService _acts as upstream_
 ** for the Nexus, and thus gains the ability to respond to those few special
 ** messages, which can not be handled in a generic way on the Nexus:
 ** - *act* handles command invocation within the Session core, and
 **   is treated by [forwarding](\ref command-handler.hpp) it over the
 **   SessionCommand facade to the [Steam-Dispatcher](\ref steam-dispatcher.hpp)
 ** - *note* observes and captures presentation state note messages, which
 **   are to be handled by a central presentation state manager (TODO 1/17).
 ** 
 ** @see AbstractTangible_test
 ** @see BusTerm_test
 ** 
 */


#ifndef STAGE_CTRL_CORE_SERVICE_H
#define STAGE_CTRL_CORE_SERVICE_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/diff/gen-node.hpp"
#include "include/session-command-facade.h"
#include "stage/ctrl/state-recorder.hpp"
#include "stage/ctrl/bus-term.hpp"
#include "stage/ctrl/nexus.hpp"


namespace stage {
namespace ctrl{
  
  using steam::control::SessionCommand;
  using lib::diff::Rec;
  
  
  /**
   * Attachment point to »central services« within the UI-Bus.
   * This special implementation of the [BusTerm] interface receives and
   * handles those messages to be processed by centralised services:
   * - commands need to be sent down to Steam-Layer
   * - presentation state messages need to be recorded and acted upon.
   * As an object, CoreService encases the heart of the UI-Bus, the
   * \ref Nexus, and acts as "PImpl" for the stage::UiBus front-end.
   */
  class CoreService
    : public BusTerm
    , util::NonCopyable
    {
      
      Nexus uiBusBackbone_;
      StateRecorder stateRecorder_;
      
      
      virtual void
      act (GenNode const& command)  override
        {
          SessionCommand::facade().trigger (command.idi.getSym(), command.data.get<Rec>());
        }
      
      
      virtual void
      note (ID subject, GenNode const& stateMark)  override
        {
          stateRecorder_.recordState (subject, stateMark);
        }
      
      
    public:
      explicit
      CoreService (ID identity =lib::idi::EntryID<CoreService>())
        : BusTerm(identity, uiBusBackbone_)
        , uiBusBackbone_{*this}
        , stateRecorder_{*this}
        {
          INFO (stage, "UI-Backbone operative.");
        }
      
     ~CoreService();
      
      
      StateManager&
      getStateManager()
        {
          return stateRecorder_;
        }
    };
  
  
  
}} // namespace stage::ctrl
#endif /*STAGE_CTRL_CORE_SERVICE_H*/
