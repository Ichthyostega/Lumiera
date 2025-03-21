/*
  STATE-MANAGER.hpp  -  maintaining persistent interface state

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file state-manager.hpp
 ** Interface: a component to maintain persistent interface state.
 ** Here, "presentation state" is understood as state not rooted within the
 ** model and without effect on the final rendered result. Most of this state
 ** is transitory and arises from the normal working from the UI (and toolkit set).
 ** Yet part of this state is relevant to the _way to work with the tooling_, so we
 ** typically expect these choices and traces of usage to remain sticky, persistent
 ** across editing sessions.
 ** 
 ** # Implementation technique
 ** In Lumiera, handling of persistent presentation state relies on the UI backbone
 ** structure known as [UI-Bus](\ref ui-bus.hpp). Any element of more than local relevance,
 ** as attached to this backbone, will emit *state mark notification* messages, whenever
 ** some transition of presentation state is deemed relevant. The PresentationStateManager
 ** operates as one of the [core services](\ref core-service.hpp) and receives, groups and
 ** remembers those messages, always retaining the latest state information observed for any
 ** property of any [tangible interface element](\ref tangible.hpp) encountered thus far.
 ** 
 ** @todo as of 2/2016 this is still preliminary.
 **       In the end, we want to capture and restore presentation state
 **       in dependency to the current perspective and work site
 ** 
 ** @see BusTerm_test::captureStateMark() usage example
 ** @see BusTerm_test::replayStateMark()  usage example
 ** 
 */


#ifndef STAGE_CTRL_STATE_MANAGER_H
#define STAGE_CTRL_STATE_MANAGER_H


#include "lib/idi/entry-id.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/nocopy.hpp"

#include <string>


namespace stage {
namespace ctrl {
  
  using std::string;
  
  
  /**
   * Interface: handling of persistent interface state.
   * Operations to retrieve previously captured state and
   * to re-play this state towards the originating UI-elements.
   * It is assumed that the actual implementation is connected
   * to the UI-Bus and captures *state mark notifications*.
   * State is first grouped by ID of the originating interface
   * element, and recorded per distinct property within each
   * element.
   */
  class StateManager
    : util::NonCopyable
    {
    protected:
      virtual ~StateManager();    ///< this is an interface
      
      using ID = lib::idi::BareEntryID const&;
      using StateMark = lib::diff::GenNode const&;
    public:
      
      virtual StateMark
      currentState (ID uiElm, string propertyKey) const =0;
      
      virtual void
      replayState (ID uiElm, string propertyKey)        =0;
      
      virtual void
      replayAllState()                                  =0;
      
      virtual void
      replayAllState (string propertyKey)               =0;
      
      virtual void
      replayAllProperties (ID uiElm)                    =0;
      
      virtual void
      clearState()                                      =0;
      
    private:
    };
  
  
  
}} // namespace stage::ctrl
#endif /*STAGE_CTRL_STATE_MANAGER_H*/
