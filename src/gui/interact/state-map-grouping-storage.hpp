/*
  STATE-MAP-GROUPING-STORAGE.hpp  -  grouping storage to track presentation state

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


/** @file state-map-grouping-storage.hpp
 ** Implementation of storage for captured presentation state.
 ** This is a associative storage, grouped by element ID.
 ** 
 ** @todo as of 2/2016 this is complete WIP-WIP-WIP
 ** 
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef GUI_INTERACT_STATE_MAP_GROUPING_STORAGE_H
#define GUI_INTERACT_STATE_MAP_GROUPING_STORAGE_H


#include "lib/error.hpp"
//#include "gui/ctrl/bus-term.hpp"
//#include "lib/idi/entry-id.hpp"
#include "lib/diff/gen-node.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

#include <boost/noncopyable.hpp>
#include <unordered_map>
#include <string>


namespace gui {
namespace interact {
  
//  using lib::HashVal;
//  using util::isnil;
  using lib::idi::BareEntryID;
  using lib::diff::GenNode;
  using std::string;
  
  struct GenNodeComparator
    {
      bool
      operator() (GenNode const& left, GenNode const& right)  const
        {
          return left.idi.getSym() < right.idi.getSym();
        }
    };
  
  
  /**
   * Map storage for captured presentation state information.
   * @todo write type comment...
   */
  struct StateMapGroupingStorage
    : boost::noncopyable
    {
      using StateData = std::set<GenNode, GenNodeComparator>;
      
      using Storage = std::unordered_map<BareEntryID, StateData, BareEntryID::UseEmbeddedHash>;
      
      using Record = Storage::value_type;
      
      
      Storage elmTable_;
      
      
      /** retrieve captured state
       * @return reference to the state mark last seen for the denoted property
       *         or reference to a generic "no" marker (Ref::NO)
       */
      GenNode const&
      retrieve (BareEntryID const& elementID, string propertyKey)  const
        {
          UNIMPLEMENTED ("retrieve captured state");
        }
      
      void
      record (BareEntryID const& elementID, GenNode const& stateMark)
        {
          UNIMPLEMENTED ("store state record");
        }

      void
      clear()
        {
          UNIMPLEMENTED ("discard all stored state information");
        }
      
      using iterator = Storage::const_iterator;
      
      iterator begin() const { return elmTable_.begin(); }
      iterator end()   const { return elmTable_.end(); }
      
      iterator
      find (BareEntryID const& elementID)
        {
          UNIMPLEMENTED ("search for element record");
        }
      
      static BareEntryID const&
      getID (Record entry)
        {
          return entry.first;
        }
      
      static StateData const&
      getState (Record entry)
        {
          return entry.second;
        }
      
      static GenNode const&
      getState (Record entry, string propertyKey)
        {
          UNIMPLEMENTED ("fetch property data from given element record");
        }
      
    private:
    };
  
  
  
}} // namespace gui::interact
#endif /*GUI_INTERACT_STATE_MAP_GROUPING_STORAGE_H*/
