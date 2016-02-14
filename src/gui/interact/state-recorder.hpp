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
 ** Referral to unknown elements and properties is ignored.
 ** 
 ** \par performance
 ** The storage layout was chosen under the assumption that we'll get
 ** many elements with only few properties per element. The implementing
 ** hash table relies on the hash function for BareEntryID, which uses
 ** the embedded hash, which in turn is based on hashing the symbolicID
 ** plus a hashed type string.
 ** 
 ** @todo as of 2/2016 noting but a simple data retrieval happens here.
 **       Actually, on the long run, we want "intelligent" handling of
 **       presentation state, we want to capture and restore state
 **       with sensitivity to perspective and work site. Probably
 **       this means to layer a second level of aggregation on top.
 ** @warning state mark entries are added, never discarded.
 **       Once we start actually persisting this state,
 **       this might get us into trouble.
 ** 
 ** @see BusTerm_test::captureStateMark usage example
 ** 
 */


#ifndef GUI_INTERACT_STATE_RECORDER_H
#define GUI_INTERACT_STATE_RECORDER_H


#include "lib/error.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/diff/gen-node.hpp"
#include "gui/ctrl/bus-term.hpp"
#include "gui/interact/presentation-state-manager.hpp"
#include "gui/interact/state-map-grouping-storage.hpp"

#include <string>


namespace gui {
namespace interact {
  
  using gui::ctrl::BusTerm;
  using lib::diff::GenNode;
  using lib::diff::Ref;
  using std::string;
  
  
  
  
  /**
   * Simple map based implementation of the
   * PresentationStateManager interface.
   * Requires a permanent connection to the UI-Bus,
   * which is given as reference at construction.
   * The intention is to use such an implementation
   * embedded within the gui::ctrl::CoreService,
   * which in turn then manages the lifecycle of
   * this UI-Bus connection. Besides, a derived
   * mock implementation is available through
   * the test::Nexus
   * 
   * @see StateMapGroupingStorage storage implementation
   */
  class StateRecorder
    : public PresentationStateManager
    {
      using Storage = StateMapGroupingStorage;
      using Record  = Storage::Record const&;
      
      BusTerm& uiBus_;
      Storage  storage_;
      
      
      
      /* === PresentationStateManager interface === */
      
      virtual StateMark
      currentState (ID uiElm, string propertyKey)  const override
        {
          return storage_.retrieve (uiElm, propertyKey);
        }
      
      
      virtual void
      replayState (ID uiElm, string propertyKey)  override
        {
          StateMark state = storage_.retrieve (uiElm, propertyKey);
          if (state != Ref::NO)
            uiBus_.mark (uiElm, state);
        }
      
      
      virtual void
      replayAllState()  override
        {
          for (Record entry : storage_)
            replayPropertiesOf (entry);
        }
      
      
      /** replay all captured state from any element,
       *  but captured especially for the given property
       */
      virtual void
      replayAllState (string propertyKey)  override
        {
          for (Record entry : storage_)
            {
              StateMark state = Storage::getState (entry, propertyKey);
              if (state != Ref::NO)
                uiBus_.mark (Storage::getID (entry), state);
            }
        }
      
      
      /** replay all captured state from the given element. */
      virtual void
      replayAllProperties (ID uiElm)  override
        {
          auto entry = storage_.find (uiElm);
          if (entry != storage_.end())
            replayPropertiesOf (*entry);
        }
      
      
      virtual void
      clearState()  override
        {
          storage_.clear();
        }
      
      
    public:
      StateRecorder (BusTerm& busConnection)
        : uiBus_(busConnection)
        , storage_()
        { }
      
      /** Interface for the operating facility (CoreService)
       *  to feed state mark messages to be remembered.
       * @param uiElm originator of the state notification
       * @param stateMark state information to record
       * @note special handling of a "reset" state mark:
       *       in that case, discard any captured state for
       *       this element, since it goes back to default.
       */
      void
      recordState (ID uiElm, StateMark stateMark)
        {
          if ("reset" == stateMark.idi.getSym())
            storage_.clearState (uiElm);
          else
          if ("clearErr" == stateMark.idi.getSym())
            storage_.clearProperty (uiElm, "Error");
          else
          if ("clearMsg" == stateMark.idi.getSym())
            storage_.clearProperty (uiElm, "Message");
          else
            storage_.record (uiElm, stateMark);
        }
      
    private:
      void
      replayPropertiesOf (Record entry)
        {
          ID uiElm = Storage::getID (entry);
          for (auto& stateMark : Storage::getState (entry))
            uiBus_.mark (uiElm, stateMark);
        }
    };
  
  
  
}} // namespace gui::interact
#endif /*GUI_INTERACT_STATE_RECORDER_H*/
