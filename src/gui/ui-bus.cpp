/*
  UiBus  -  UI model and control backbone

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
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

* *****************************************************/


/** @file ui-bus.cpp
 ** Implementation of the UI backbone service for messaging.
 ** Any globally relevant widget or controller within the Lumiera UI
 ** is connected to the [UI-Bus](ui-bus.hpp), which is essentially implemented
 ** within this compilation unit. [Clients](\ref gui::model::Tangible) typically
 ** use the [BusTerm-Interface](bus-term.hpp) to route generic actions and
 ** receive notifications, state changes and mutations.
 ** 
 ** @see core-service.hpp
 ** @see BusTerm_test
 ** 
 */


#include "gui/ui-bus.hpp"
#include "gui/ctrl/core-service.hpp"


namespace gui {
  ///////////////////////////////////////////////////////////////////////////////////TICKET #959 : scheduled for termination....
namespace controller {
    
  Controller::Controller (model::Project& modelProject)
    : project_(modelProject)
    , playback_()
    { }
  
  
  
  ctrl::PlaybackController& Controller::get_playback_controller()
  {
    return playback_;
  }
  
}// namespace gui::controller
  ///////////////////////////////////////////////////////////////////////////////////TICKET #959 : scheduled for termination....
  
  
  UiBus::UiBus()
    : coreService_{new ctrl::CoreService}
    { }
  
  
  // emit administrative code here...
  UiBus::~UiBus() { }
  
  
  ctrl::BusTerm&
  UiBus::getAccessPoint()
  {
    return *coreService_;
  }

  
namespace ctrl {
    
    CoreService::~CoreService() { }
    
    
    /** @note detaches the corresponding node automatically */
    BusTerm::~BusTerm()
    {
      theBus_.routeDetach (this->endpointID_);
    }
    
    
    diff_msg::Holder::~Holder() { }
    
    
    
    
    /** Builder function: establish and wire a new BusTerm.
     * @param newNode the UI-Element corresponding to and using the new BusTerm
     * @remarks automatically establishes a down-link connection to the given
     *        [Tangible]; the UI-Bus will use this node as target to dispatch
     *        `mark` messages addressed to the new node's ID. It is expected
     *        that the Tangible in turn will use the returned BusTerm for
     *        any up-link communication. Thus, on destruction, the new
     *        BusTerm will detach this UI-Bus connection altogether.
     */
    BusTerm
    BusTerm::attach (ID identity, Tangible& newNode)
    {
      return BusTerm(identity, theBus_.routeAdd (identity,newNode));
    }
    
    
    
    /* ==== standard implementation of the BusTerm interface ==== */
    
    /** prepare or trigger invocation of a command.
     * @param command a GenNode either holding command parameters
     *        or an int to signal actual command invocation
     * @remarks some commands can simply be invoked right away, but
     *          in the general case, command preparation and invocation
     *          is a multi-step process. The gui::interact::InvocationTrail
     *          is used to conduct this argument binding process from within
     *          the UI. Here, at the UI-Bus interface, we're just interested
     *          in the fact _that_ some command is to be bound or invoked.
     *          This information is forwarded to the command receiver service,
     *          which in turn talks to the proc dispatcher.
     * @note no information regarding the _origin_ of this command invocation
     *          is captured. If a command needs a _subject_, this has to be
     *          bound as an command argument beforehand.
     * @see gui::interact::InvocationTrail
     * @see gui::model::Tangible::prepareCommand()
     * @see gui::model::Tangible::issueCommand()
     */
    void
    BusTerm::act (GenNode const& command)
    {
      theBus_.act (command);
    }
    
    
    /** capture and record a "state mark" for later replay for restoring UI state.
     * @param subject the [endpoint-ID](BusTerm::endpointID_) of the emitting element
     * @param mark the actual state update or notification message to be remembered
     * @remarks relevant changes to presentation state, which are to be recalled and
     *          restored later, are emitted from the place they occur, packaged as
     *          "state mark" messages. This assumes the presence of some dedicated
     *          presentation state manager, attached and listening somewhere at
     *          a core service location. Such a core presentation state service
     *          has shares additional understanding of the state mark's meaning;
     *          it is assumed to track cumulated state, which will be replayed
     *          later when it comes to restoring some presentation state.
     */
    void
    BusTerm::note (ID subject, GenNode const& mark)
    {
      theBus_.note (subject,mark);
    }
    
    
    /** route a state update or notification to the given subject.
     * @param subject the [endpoint-ID](BusTerm::endpointID_) of the element to address
     * @param mark the actual state update or notification message to be delivered
     * @remarks each "subject" to be addressed is a gui::model::Tangible, and as such
     *          holds a BusTerm of its own, which in turn ensures a registration and
     *          connection from the [central routing hub](gui::ctrl::Nexus) down to
     *          the element. Thus, the default implementation is just to pass the
     *          given state mark "up", assuming that it will reach the hub
     *          eventually, which in turn knows hot to reach the element.
     * @return if the target was known and the mark operation dispatched.
     * @note messages to unreachable elements will be dropped.
     */
    bool
    BusTerm::mark (ID subject, GenNode const& mark)
    {
      return theBus_.mark (subject,mark);
    }
    
    
    /** broadcast a notification message to all currently connected bus terminals.
     * @param mark the actual state update or notification message to be delivered
     * @return number of notified terminals.
     * @remarks this call assumes that "somewhere" within the UI-Bus
     *          a distribution node or hub is installed, with the ability
     *          to find all currently connected terminals. In the standard
     *          configuration this is implemented by the ctrl::Nexus
     */
    size_t
    BusTerm::markAll (GenNode const& mark)
    {
      return theBus_.markAll (mark);
    }
    
    
    /** alter and reshape the designated subject by applying the given diff message.
     * @param diff encapsulated representation of a concrete diff sequence for the target.
     * @return if the target was known and the diff was applied without accident
     * @throws lumiera::error::State when diff application fails due to the target's shape
     *         or state being different than implicitly assumed by the given diff.
     * @remark each tangible offers to build a custom TreeMutator, which is appropriately
     *         wired to receive diff messages targeted towards this specific element. The
     *         standard implementation within the Nexus uses this ability to create a
     *         DiffApplicator<DiffMutable>, finally to feed the given diff to the target,
     *         which consequently will reshape and remould itself accordingly.
     */
    bool
    BusTerm::change (ID subject, MutationMessage& diff)
    {
      return theBus_.change(subject, diff);
    }
    
    
    /**
     * @internal establish new down-link connection form UI-Bus
     * @param node reference to the [Tangible] to be connected.
     * @return corresponding up-link for the initiating node to use
     */
    BusTerm&
    BusTerm::routeAdd (ID identity, Tangible& node)
    {
      return theBus_.routeAdd (identity, node);
    }
    
    
    /**
     * @internal disable down-link connection
     * @remarks corresponding node is about to go away.
     */
    void
    BusTerm::routeDetach(ID node)  noexcept
    {
      if (not isShortCircuit(node))
        theBus_.routeDetach (node);
    }
    
    
    BusTerm::operator string()  const
    {
      return "BusTerm-" + string(endpointID_);
    }
    
    
    
}}// namespace gui::ctrl
