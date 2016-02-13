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
 ** @see StateRecorder
 ** @see BusTerm_test::captureStateMark()
 ** @see BusTerm_test::replayStateMark()
 ** 
 */


#ifndef GUI_INTERACT_STATE_MAP_GROUPING_STORAGE_H
#define GUI_INTERACT_STATE_MAP_GROUPING_STORAGE_H


#include "lib/error.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/diff/gen-node.hpp"

#include <boost/noncopyable.hpp>
#include <unordered_map>
#include <algorithm>
#include <set>


namespace gui {
namespace interact {
  
  using lib::idi::BareEntryID;
  using lib::diff::GenNode;
  using lib::diff::Ref;
  using std::string;
  
  
  
  /**
   * Map storage for captured presentation state information.
   * The master table is an association of model::Tangible element IDs
   * to StateData records, which are a set of property data elements.
   * The key of the stored elements acts as propertyKey and was chosen
   * by the originating UI element. It is assumed that the entry last
   * seen represents the current state of this property, so previous
   * records are overwritten. Access to unknown data is marked by
   * returning diff::Ref::NO rsp. `Storage::end()` (when searching)
   */
  class StateMapGroupingStorage
    : boost::noncopyable
    {
      using StateData = std::set<GenNode,  GenNode::IDComparator>;
      using Storage = std::unordered_map<BareEntryID, StateData,  BareEntryID::UseEmbeddedHash>;
      
      
      Storage elmTable_;
      
    public:
      using Record = Storage::value_type;
      
      bool
      empty()  const
        {
          return elmTable_.empty();
        }
      
      size_t
      size()  const
        {
          size_t siz{0};
          for (Record const& entry : elmTable_)
            siz += entry.second.size();
          return siz;
        }
      
      void
      clear()
        {
          elmTable_.clear();
        }
      
      
      /** retrieve captured state
       * @return reference to the state mark last seen for the denoted property
       *         or reference to a generic "no" marker (Ref::NO)
       */
      GenNode const&
      retrieve (BareEntryID const& elementID, string propertyKey)  const
        {
          iterator entry = find (elementID);
          if (entry == elmTable_.end())
            return Ref::NO;
          else
            return getState (*entry, propertyKey);
        }
      
      void
      record (BareEntryID const& elementID, GenNode const& stateMark)
        {
          elmTable_[elementID].emplace (stateMark);
        }
      
      
      using iterator = Storage::const_iterator;
      
      iterator begin() const { return elmTable_.begin(); }
      iterator end()   const { return elmTable_.end(); }
      
      iterator
      find (BareEntryID const& elementID)  const
        {
          return elmTable_.find (elementID);
        }
      
      static BareEntryID const&
      getID (Record const& entry)
        {
          return entry.first;
        }
      
      static StateData const&
      getState (Record const& entry)
        {
          return entry.second;
        }
      
      
      /**
       * Access the recorded state mark, if any.
       * @warning we can't search logarithmically,
       *    since this would require to construct another GenNode
       *    to represent the key.
       * @todo seemingly GCC-4.9 doesn't yet support the "transparent search" feature
       *    of C++14, where std::set would support an additional overload of the `find()` function,
       *    templated to accept a value _convertible_ to some key equivalent, which needs to be
       *    understood by the comparator of the set. To trigger using this overload, the
       *    comparator has to define a suitable `operator()` and in addition a marker type
       *    `is_transparent`. See [transparent-cmp] and the [reference][cind-cpp14] for explanation.
       * 
       * [find-cpp14]: http://en.cppreference.com/w/cpp/container/set/find
       * [transparent-cmp]: http://stackoverflow.com/questions/20317413/what-are-transparent-comparators
       */
      static GenNode const&
      getState (Record const& entry, string propertyKey)
        {
          StateData const& stateSet = entry.second;
          StateData::const_iterator propertyRecord
               = std::find_if (stateSet.begin()
                              ,stateSet.end()
                              ,[=](GenNode const& stateMark)
                                   {
                                     return propertyKey == stateMark.idi.getSym();
                                   });
          
          if (propertyRecord == stateSet.end())
            return Ref::NO;
          else
            return *propertyRecord;
        }
    };
  
  
  
}} // namespace gui::interact
#endif /*GUI_INTERACT_STATE_MAP_GROUPING_STORAGE_H*/
