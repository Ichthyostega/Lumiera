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
  using lib::diff::Ref;
  using std::string;
  
  
  
  
  /**
   * Simple map based implementation of the
   * PresentationStateManager interface.
   * 
   * @todo write type comment...
   */
  class StateRecorder
    : public PresentationStateManager
    {
      using Storage = StateMapGroupingStorage;
      using Iter    = Storage::iterator;
      
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
          for (Iter entry = storage_.begin(); entry!=storage_.end(); ++entry)
            replayPropertiesOf (entry);
        }
      
      
      virtual void
      replayAllState (string propertyKey)  override
        {
          for (Iter entry = storage_.begin(); entry!=storage_.end(); ++entry)
            {
              StateMark state = Storage::getState (entry, propertyKey);
              if (state != Ref::NO)
                uiBus_.mark (Storage::getID (entry), state);
            }
        }
      
      
      virtual void
      replayAllProperties (ID uiElm)  override
        {
          Iter entry = storage_.find (uiElm);
          if (entry != storage_.end())
            replayPropertiesOf (entry);
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
      
      
      void
      record (ID uiElm, StateMark stateMark)
        {
          storage_.record (uiElm, stateMark);
        }
      
    private:
      void
      replayPropertiesOf (Iter entry)
        {
          ID uiElm = Storage::getID (entry);
          for (auto& stateMark : Storage::getState (entry))
            uiBus_.mark (uiElm, stateMark);
        }
    };
  
  
  
}} // namespace gui::interact
#endif /*GUI_INTERACT_STATE_RECORDER_H*/
