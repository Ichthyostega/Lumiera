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
#include "lib/iter-adapter-stl.hpp"
//#include "lib/util-foreach.hpp"
#include "lib/util.hpp"

#include <boost/operators.hpp>
#include <tr1/unordered_map>
//#include <iostream>
//#include <string>
//#include <set>

namespace lib    {
namespace advice {
  
  using lib::iter_stl::eachVal;
//  using util::for_each;
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
        , boost::equality_comparable<Entry, POA,
          boost::equality_comparable<Entry> >
        {
          explicit
          Entry (POA& elm)
            : pair<Binding::Matcher, POA*> (getMatcher(elm), &elm)
            { }
          
          friend bool
          operator== (Entry const& a, Entry const& b)
          {
            return a.second == b.second;
          }
          
          friend bool
          operator== (Entry const& a, POA const& p)
          {
            return a.second != &p;
          }
        };
      
      typedef vector<Entry> EntryList;
      typedef typename EntryList::iterator EIter;
      
      
      class Cluster
        {
           EntryList elms_;
           
        public:
           size_t
           size()  const
             {
               return elms_.size();
             }
           
           void
           append (POA& elm)
             {
               REQUIRE (!contains (elm), "Duplicate entry");
               elms_.push_back (Entry(elm));
             }
           
           void
           overwrite (POA const& oldRef, POA& newEntry)
             {
               EIter pos = std::find (elms_.begin(),elms_.end(), oldRef);
               REQUIRE (pos!=elms_.end(), "Attempt to overwrite an entry which isn't there.");
               REQUIRE (!contains (newEntry), "Duplicate entry");
               
               *pos = Entry(newEntry);
               
               ENSURE (!contains (oldRef), "Duplicate entry");
             }
           
           void
           remove (POA const& refEntry)
             {
               EIter pos = std::find (elms_.begin(),elms_.end(), refEntry);
               if (pos!=elms_.end())
                 elms_.erase(pos);
               
               ENSURE (!contains (refEntry), "Duplicate entry");
             }
           
           bool
           contains (POA const& refElm)
             {
               for (EIter i=elms_.begin(); i!=elms_.end(); ++i)
                 if (i->second == &refElm)
                   return true;
               return false;
             }
        private:
        };
      
      
      struct RequestCluster
        : Cluster
        {
          void
          publish_all_solutions (POA& provisionElm)
            {
              UNIMPLEMENTED ("traverse all, check match, publish solution");
            }
          
          void
          rewrite_all_solutions (POA const& oldProv, POA& newProv)
            {
              UNIMPLEMENTED ("traverse all, check match, publish solution, treat solutions with old Provision as if newly added request");
            }
          
          void
          retract_all_solutions (POA const& oldProv)
            {
              UNIMPLEMENTED ("traverse all, check match, treat matching as if newly added request");
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
      
      
        
      /* ==== Index Tables ===== */  
        
      typedef unordered_map<HashVal, RequestCluster> RTable; 
      typedef unordered_map<HashVal, ProvisionCluster> PTable;
      
      mutable RTable requestEntries_;
      mutable PTable provisionEntries_;
      
      
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
          HashVal oKey (hash_value(oldRef));
          HashVal nKey (hash_value(newEntry));
          if (oKey != nKey)
            {
              requestEntries_[oKey].remove (oldRef);
              requestEntries_[nKey].append (newEntry);
            }
          else
            requestEntries_[nKey].overwrite (oldRef, newEntry);
          provisionEntries_[nKey].publish_latest_solution (newEntry);
        }
      
      void
      removeRequest (POA const& refEntry)
        {
          HashVal oKey (hash_value(refEntry));
          requestEntries_[oKey].remove (refEntry);
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
          HashVal oKey (hash_value(oldRef));
          HashVal nKey (hash_value(newEntry));
          if (oKey != nKey)
            {
              provisionEntries_[oKey].remove (oldRef);
              provisionEntries_[nKey].append (newEntry);
              requestEntries_[nKey].publish_all_solutions (newEntry);
              requestEntries_[oKey].retract_all_solutions (oldRef);
            }
          else
            {
              provisionEntries_[nKey].overwrite (oldRef, newEntry);
              requestEntries_[nKey].rewrite_all_solutions (oldRef,newEntry);
            }
        }
      
      void
      removeProvision (POA const& refEntry)
        {
          HashVal oKey (hash_value(refEntry));
          provisionEntries_[oKey].remove (refEntry);
          requestEntries_[oKey].retract_all_solutions (refEntry);
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
          return sumClusters (eachVal (requestEntries_));
        }
      
      size_t
      provision_count()  const
        {
          return sumClusters (eachVal (provisionEntries_));
        }
      
      bool
      hasRequest (POA const& refEntry)  const
        {
          return requestEntries_[hash_value(refEntry)].contains (refEntry);
        }
      
      bool
      hasProvision (POA const& refEntry)  const
        {
          return provisionEntries_[hash_value(refEntry)].contains (refEntry);
        }
      
      
    private:
      /** internal: sum element count over all 
       *  clusters in the given hashtable */
      template<class IT>
      static size_t
      sumClusters (IT ii)
        {
          size_t sum=0;
          for ( ; ii; ++ii )
              sum += ii->size();
          return sum;
        }
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
