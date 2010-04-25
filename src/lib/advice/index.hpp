/*
  INDEX.hpp  -  data structure for organising advice solutions and matching
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/


/** @file index.hpp
 ** Implementation datastructure for use by the Advice system.
 ** To support the \em Advice collaboration, it is necessary to match advice requests with
 ** existing advice provisions. Each successful match creates an advice solution, resulting in
 ** the bound piece of advice (data) to become visible to all the advised entities having placed
 ** a matching advice request into the advice system.
 ** 
 ** This header is intended to be incorporated as part of the advice system implementation (advice.cpp).
 ** It is \em not usable as an external interface. But it is written in a rather self-contained manner,
 ** in order to be testable in isolation. To this end, the actual PointOfAdvice entities being organised
 ** by this index datastructure remain abstract (defined as template parameter). As link for dealing 
 ** with those entities, we employ the free functions \c getSolution(POA) and \c setSolution(POA)
 ** -- expected to be picked up by ADL.
 ** 
 ** \par implementation notes
 ** The advice binding index is implemented by two hashtables holding Binding::Matcher entries.
 ** Each entry associates a back-link to the corresponding POA (PointOfAdvice), which is assumed
 ** to be maintained outside the index. PointOfAdvice is an type-erased interface baseclass.
 ** Actually the advice system will have to deal with concrete advice::Request and advice::Provision
 ** objects, which are templated to a specific advice type; but this specifically typed context
 ** is kept on the interface level (advice.hpp) and the type information is stripped before
 ** calling into the actual implementation, so the index can be realised in a generic fashion.
 ** 
 ** @note as of 4/2010 this is an experimental setup and implemented just enough to work out
 **       the interfaces. Ichthyo expects this collaboration service to play a central role
 **       at various places within proc-layer.
 ** @note for now, \em only the case of a completely constant (ground) pattern is implemented.
 **       Later we may consider to extend the binding patterns to allow variables. The mechanics
 **       of the index are designed right from start to support this case (and indeed the index
 **       could be much simpler if it wasn't to deal with this foreseeable additional complexity.
 **       When a pattern contains variables, then even within one bucket of the hashtable there
 **       might be non-matching entries. Each individual pair of (provision, request) has to be
 **       checked explicitly to determine a match.     /////////////////////////TICKET #615
 ** 
 ** @see advice.hpp
 ** @see binding.hpp
 ** @see advice-index-test.cpp
 ** @see advice-binding-pattern-test.cpp
 ** @see advice-basics-test.cpp
 ** 
 */


#ifndef LIB_ADVICE_INDEX_H
#define LIB_ADVICE_INDEX_H


#include "lib/error.hpp"
#include "lib/advice/binding.hpp"
//#include "lib/symbol.hpp"
//#include "lib/query.hpp"
#include "include/logging.h"
#include "lib/util.hpp"

#include <tr1/unordered_map>
//#include <iostream>
//#include <string>
//#include <set>

namespace lib    {
namespace advice {
  
  using util::contains;
//  using std::string;
  using std::pair;
  using std::vector;
  using std::tr1::unordered_map;
  
//  LUMIERA_ERROR_DECLARE (BINDING_PATTERN_SYNTAX); ///< Unable to parse the given binding pattern definition
  
  
  
  /**
   * Index datastructure for organising advice solutions.
   * Based on two hashtables for advice provisions and requests,
   * the index allows to add, modify and remove entities of these
   * two kinds. Each of these mutating operations immediately
   * re-computes the advice solutions and publishes the results
   * by invoking the free function \c setSolution(POA) for the
   * corresponding PointOfAdvice entity.
   * 
   * @note the diagnostic API is mainly intended for unit testing
   *       and \em not implemented with focus on performance. 
   */
  template<class POA>
  class Index
    {
      
      
      struct Entry
        : pair<Binding::Matcher, POA*> 
        {
          Entry (POA& elm)
            : pair<Binding::Matcher, POA*> (getMatcher(elm), &elm)
            { }
          
          friend bool
          operator== (Entry const& a, Entry const& b)
          {
            return a.second == b.second;
          }
          
          friend bool
          operator!= (Entry const& a, Entry const& b)
          {
            return a.second != b.second;
          }
        };
      
      typedef vector<Entry> EntryList;
      
      
      class Cluster
        {
           EntryList elms_;
           
        public:
           void
           append (POA& elm)
             {
               Entry entry (elm);
               REQUIRE (!contains (elms_, entry));
               elms_.push_back(entry);
             }
        };
      
      
      struct RequestCluster
        : Cluster
        {
          void
          publish_all_solutions (POA& provisionElm)
            {
              UNIMPLEMENTED ("traverse all, check match, publish solution");
            }
        };
      
      struct ProvisionCluster
        : Cluster
        {
          void
          publish_latest_solution (POA& requestElm)
            {
              UNIMPLEMENTED ("visit from back, find first match, publish solution");
            }
        };
      
        
      typedef unordered_map<HashVal, RequestCluster> RTable; 
      typedef unordered_map<HashVal, ProvisionCluster> PTable;
      
      RTable requestEntries_;
      PTable provisionEntries_;
      
      
    public:
      
      void
      addRequest (POA& entry)
        {
          HashVal key (hash_value(entry));
          requestEntries_[key].append (entry);
          provisionEntries_[key].publish_latest_solution (entry);
        }
      
      void
      modifyRequest (POA const& oldRef, POA& newEntry)
        {
          UNIMPLEMENTED ("replace denoted entry with new advice request");
        }
      
      void
      removeRequest (POA const& refEntry)
        {
          UNIMPLEMENTED ("drop advice request");
        }
      
      
      void
      addProvision (POA& entry)
        {
          HashVal key (hash_value(entry));
          provisionEntries_[key].append (entry);
          requestEntries_[key].publish_all_solutions (entry);
        }
      
      void
      modifyProvision (POA const& oldRef, POA& newEntry)
        {
          UNIMPLEMENTED ("replace denoted entry with new advice provision");
        }
      
      void
      removeProvision (POA const& refEntry)
        {
          UNIMPLEMENTED ("retract advice provision entry");
        }
      
      
      /** @warning calling this effectively detaches any existing advice information,
       *           but doesn't clean up storage of advice provisions incorporated
       *           within the advice system in general.
       */
      void
      clear ()
        {
          WARN (library, "Purging Advice Binding Index...");
          requestEntries_.clear();
          provisionEntries_.clear();
        }
      
      
      
      /* == diagnostics == */
      
      /** validity self-check */
      bool isValid()  const;
      
      size_t
      size()  const
        {
          return request_count() + provision_count();
        }
      
      size_t
      request_count()  const
        {
          UNIMPLEMENTED ("number of request entries");
        }
      
      size_t
      provision_count()  const
        {
          UNIMPLEMENTED ("number of provision entries");
        }
      
      bool
      hasRequest (POA const& refEntry)  const
        {
          UNIMPLEMENTED ("do we hold this entry?");
        }
      
      bool
      hasProvision (POA const& refEntry)  const
        {
          UNIMPLEMENTED ("do we hold this entry?");
        }
      
      
    private:
      /** internal: parse into atoms, and insert them */
    };
  
  
  
  
  
  
  /* === equality comparison and matching === */
  
  
  
  
  
  /* == diagnostics == */
  template<class POA>
  bool
  Index<POA>::isValid()  const
  {
    UNIMPLEMENTED ("verify the index invariant");
  }
  
  
  
  
  
  
}} // namespace lib::advice
#endif
