/*
  STATE-RECORDER.hpp  -  map based recording of presentation state

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file state-recorder.hpp
 ** Implementation of the PresentationStateManager interface
 ** through associative (key-value) store. This implementation
 ** groups recorded message by elementID and simply overwrites
 ** previous state info for a given attribute with later one.
 ** 
 ** @todo as of 2/2016 this is complete WIP-WIP-WIP
 ** 
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef GUI_INTERACT_STATE_RECORDER_H
#define GUI_INTERACT_STATE_RECORDER_H


#include "lib/error.hpp"
//#include "gui/ctrl/bus-term.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/diff/gen-node.hpp"
#include "gui/interact/presentation-state-manager.hpp"
#include "gui/interact/state-map-grouping-storage.hpp"
#include "gui/ctrl/bus-term.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

#include <boost/noncopyable.hpp>
#include <string>


namespace gui {
namespace interact {
  
//  using lib::HashVal;
//  using util::isnil;
  using gui::ctrl::BusTerm;
  using lib::diff::GenNode;
  using std::string;
  
  
  using ID = lib::idi::BareEntryID;
  
  
  /**
   * Simple map based implementation of the
   * PresentationStateManager interface.
   * 
   * @todo write type comment...
   */
  class StateRecorder
    : public PresentationStateManager
    {
      
      StateMapGroupingStorage storage_;
      BusTerm&        uiBusConnection_;
      
      
      /* === PresentationStateManager interface === */
      
      virtual lib::diff::GenNode const&
      currentState (string elementSymbol, string propertyID)  const override
        {
          UNIMPLEMENTED ("retrieve captured state");
        }
      
      
      virtual void
      replayState (string elementSymbol, string propertyID)  override
        {
          UNIMPLEMENTED ("retrieve captured state");
        }
      
      
      virtual void
      replayAllState()  override
        {
          UNIMPLEMENTED ("retrieve captured state");
        }
      
      
      virtual void
      replayAllState (string propertyID)  override
        {
          UNIMPLEMENTED ("retrieve captured state");
        }
      
      
      virtual void
      replayAllProperties (string elementSymbol)  override
        {
          UNIMPLEMENTED ("retrieve captured state");
        }
      
      virtual void
      clearState()  override
        {
          UNIMPLEMENTED ("discard all stored state information");
        }
      
    public:
      StateRecorder (BusTerm& busConnection)
        : uiBusConnection_(busConnection)
        , storage_()
        { }
      
      
      void
      record (ID const& elementID, lib::diff::GenNode const& stateMark)
        {
          UNIMPLEMENTED ("handle and record a state mark message");
        }
    };
  
  
  
}} // namespace gui::interact
#endif /*GUI_INTERACT_STATE_RECORDER_H*/
