/*
  STATE-MAP-GROUPING-STORAGE.hpp  -  grouping storage to track presentation state

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file state-map-grouping-storage.hpp
 ** Implementation of storage for captured presentation state.
 ** This is a associative storage, grouped by element ID.
 ** 
 ** @see StateMapGroupingStorage_test
 ** @see StateRecorder
 ** @see BusTerm_test::captureStateMark()
 ** @see BusTerm_test::replayStateMark()
 ** 
 */


#ifndef STAGE_CTRL_STATE_MAP_GROUPING_STORAGE_H
#define STAGE_CTRL_STATE_MAP_GROUPING_STORAGE_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/util.hpp"

#include <unordered_map>
#include <algorithm>
#include <set>


namespace stage {
namespace ctrl {
  
  using lib::idi::BareEntryID;
  using lib::diff::GenNode;
  using lib::diff::Ref;
  using ::util::unConst;
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
   * @todo reinvestigate with GCC-5                                //////////////////////////////////TICKET #991
   * @see StateMapGroupingStorage_test
   */
  class StateMapGroupingStorage
    : util::NonCopyable
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
      
      /** remember the state mark for the denoted element
       * @note stateMark value is assigned to a previously existing
       *       entry with the same ID-symbol. If no such entry exists,
       *       a (heap allocated) copy of the state mark is stored.
       */
      void
      record (BareEntryID const& elementID, GenNode const& stateMark)
        {
          auto res = elmTable_[elementID].emplace (stateMark);
          if (not res.second)
            unConst(*res.first) = stateMark; // update existing contents
        }
      
      /** clear previously recorded state for a given element and specific property */
      void
      clearProperty (BareEntryID const& elementID, string propertyKey)
        {
          iterator entry = find (elementID);
          if (entry != elmTable_.end())
            {
              StateData const& stateSet = entry->second;
              auto propertyRecord = findProperty(stateSet, propertyKey);
              
              if (propertyRecord != stateSet.end())
                unConst(stateSet).erase (propertyRecord);
            }
        }
      
      /** clear any previously recorded state for a given element */
      void
      clearState (BareEntryID const& elementID)
        {
          iterator entry = find (elementID);
          if (entry != elmTable_.end())
            elmTable_.erase (entry);
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
       *    since this would require to construct another GenNode to represent the key.
       * @todo seemingly GCC-4.9 doesn't yet support the "transparent search" feature
       *    of C++14, where std::set would support an additional overload of the `find()` function,
       *    templated to accept a value _convertible_ to some key equivalent, which needs to be
       *    understood by the comparator of the set. To trigger using this overload, the
       *    comparator has to define a suitable `operator()` and in addition a marker type
       *    `is_transparent`. See [transparent-cmp] and the [reference][cind-cpp14] for explanation.
       *                                                                            //////////////////////////////////TICKET #991
       * [find-cpp14]: http://en.cppreference.com/w/cpp/container/set/find
       * [transparent-cmp]: http://stackoverflow.com/questions/20317413/what-are-transparent-comparators
       */
      static GenNode const&
      getState (Record const& entry, string propertyKey)
        {
          StateData const& stateSet = entry.second;
          StateData::const_iterator propertyRecord = findProperty (stateSet, propertyKey);
          
          if (propertyRecord == stateSet.end())
            return Ref::NO;
          else
            return *propertyRecord;
        }
      
    private:
      static StateData::const_iterator
      findProperty (StateData const& stateSet, string propertyKey)
        {
          return std::find_if (stateSet.begin()
                              ,stateSet.end()
                              ,[=](GenNode const& stateMark)
                                   {
                                     return propertyKey == stateMark.idi.getSym();
                                   });
        }
    };
  
  
  
}} // namespace stage::ctrl
#endif /*STAGE_CTRL_STATE_MAP_GROUPING_STORAGE_H*/
