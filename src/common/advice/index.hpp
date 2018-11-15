/*
  INDEX.hpp  -  data structure for organising advice solutions and matching

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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
 ** by this index datastructure remain abstract (defined as template parameter), and are only manipulated
 ** through the following functions:
 ** - \c hash_value(POA)
 ** - \c POA::getMatcher()
 ** - \c POA::getSolution()
 ** - \c POA::setSolution(solution*)
 ** 
 ** \par implementation notes
 ** The advice binding index is implemented by two hashtables holding Binding::Matcher entries.
 ** Each entry associates a back-link to the corresponding POA (PointOfAdvice), which is assumed
 ** to be maintained outside the index. PointOfAdvice is an type-erased interface baseclass.
 ** Actually the advice system will have to deal with concrete advice::Request and advice::Provision
 ** objects, which are templated to a specific advice type; but this specifically typed context
 ** is kept on the interface level (advice.hpp) and the type information is stripped before
 ** calling into the actual implementation, so the index can be implemented generic.
 ** 
 ** While both hashtables are organised by the binding pattern hash, the individual buckets are
 ** coded explicitly as ProvisionCluster and RequestCluster -- both based on a vector of entries.
 ** In case of the provisions, there is a stack-like order, inasmuch additions happen at the back
 ** and solutions are always searched starting from the end. Because of the basic structure of
 ** a binding match, solutions are possible _only_ between provision/request - clusters with the
 ** same hash value (which is based on the predicate symbols within the patterns to match). Thus,
 ** in case of changing an existing request or solution, the internal handling is different,
 ** depending on the new value to belong or don't belong to the same cluster (hash code).
 ** It's possible (for patterns including variables) that an entry leading to a solution with
 ** the old provision doesn't match a new provision (and vice versa); thus we'll have to traverse
 ** the contents of the whole cluster, find all old solutions, match against the new counterpart
 ** and treating those entries _not matching_ with the new value as if they where completely
 ** newly added entries. In case we don't find any solution, the entries are supposed to be
 ** implemented such as to fall back to an default solution automatically (when receiving
 ** a `NULL` solution)
 ** 
 ** @note as of 4/2010 this is an experimental setup and implemented just enough to work out
 **       the interfaces. Ichthyo expects this collaboration service to play a central role
 **       later at various places within steam-layer.
 ** @note for now, _only_ the case of a completely constant (ground) pattern is implemented.
 **       Later we may consider to extend the binding patterns to allow variables. The mechanics
 **       of the index are designed right from start to support this case (and indeed the index
 **       could be much simpler if it wasn't to deal with this foreseeable additional complexity:
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


#ifndef LUMIERA_ADVICE_INDEX_H
#define LUMIERA_ADVICE_INDEX_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "include/logging.h"
#include "lib/iter-adapter-stl.hpp"
#include "lib/format-obj.hpp"
#include "lib/util-foreach.hpp"
#include "lib/util.hpp"
#include "common/advice/binding.hpp"

#include <boost/operators.hpp>
#include <unordered_map>
#include <string>

namespace lumiera{
namespace advice {
  
  namespace error = lumiera::error;
  
  using std::placeholders::_1;
  using std::unordered_map;
  using lib::iter_stl::eachVal;
  using lib::iter_stl::eachElm;
  using util::toString;
  using util::for_each;
  using util::contains;
  using util::unConst;
  using lib::Literal;
  using std::string;
  using std::vector;
  using std::pair;
  
  
  
  
  
  /**
   * Index datastructure for organising advice solutions.
   * Based on two hashtables for advice provisions and requests,
   * the index allows to add, modify and remove entities of these
   * two kinds. Each of these mutating operations immediately
   * re-computes the advice solutions and publishes the results
   * by invoking the \c setSolution() function on the
   * corresponding PointOfAdvice entity.
   * 
   * @note element \em identity is defined in terms of pointing 
   *       to the same memory location of a POA (point of advice).
   *       Thus e.g. #hasProvision means this index holds an entry
   *       pointing to exactly this given data entity.
   * @note the implementation of modifying a Request entry
   *       explicitly relies on that definition of equality.
   * @note the diagnostic API is mainly intended for unit testing
   *       and \em not implemented with focus on performance. 
   * 
   * \par Exception safety
   * Adding new registrations might throw error::Fatal or bad_alloc.
   * The addition in this case has no effect and the index remains valid.
   * The other mutating operations are NO_THROW, given that Binding::Matcher
   * is a POD and std::vector fulfils the guarantee for POD content elements.
   */
  template<class POA>
  class Index
    {
      
      
      struct Entry
        : pair<Binding::Matcher, POA*>
        , boost::equality_comparable<Entry, POA,
          boost::equality_comparable<Entry>
        >
        {
          explicit
          Entry (POA& elm)
            : pair<Binding::Matcher, POA*> (elm.getMatcher(), &elm)
            { }
          
          // using default-copy, thus assuming copy is NO_THROW
          
          
          operator string()  const  ///< diagnostics
            {
              return "E-" +hash_value(this->first) +"--> "+ this->second ;
            }
          
          friend bool
          operator== (Entry const& a, Entry const& b)
          {
            return a.second == b.second;
          }
          
          friend bool
          operator== (Entry const& a, POA const& p)
          {
            return a.second == &p;
          }
        };
      
      
      typedef vector<Entry> EntryList;
      typedef typename EntryList::iterator EIter;
      
      
      struct Cluster
        {
           EntryList elms_;
           
           size_t
           size()  const
             {
               return elms_.size();
             }
           
           void
           append (POA& elm)
             {
               REQUIRE (!contains (elm), "Duplicate entry");
               try { elms_.push_back (Entry(elm)); }
               
               catch(std::bad_alloc&)
                 {
                   throw error::Fatal("AdviceSystem failure due to exhausted memory");
                 }
             }
           
           void
           overwrite (POA const& oldRef, POA& newElm)
             {
               EIter pos = std::find (elms_.begin(),elms_.end(), oldRef);
               REQUIRE    (pos!=elms_.end(), "Attempt to overwrite an entry which isn't there.");
               REQUIRE_IF (&oldRef != &newElm, !contains (newElm), "Duplicate entry");
               
               *pos = Entry(newElm);
               
               REQUIRE_IF (&oldRef != &newElm, !contains (oldRef), "Duplicate entry");
             }
           
           void
           remove (POA const& refElm)
             {
               EIter pos = std::find (elms_.begin(),elms_.end(), refElm);
               if (pos!=elms_.end())
                 elms_.erase(pos);
               
               ENSURE (!contains (refElm), "Duplicate entry");
             }
           
           bool
           contains (POA const& refElm)
             {
               for (EIter i=elms_.begin(); i!=elms_.end(); ++i)
                 if (*i == refElm) return true;
               return false;
             }
           
           operator string()  const  ///< debugging helper: show Cluster contents
             {
               string dump{"elmList("+toString(elms_.size())+")\n"};
               for (auto const& entry : elms_)
                 dump += "E...:"+entry+"\n";
               return dump;
             }
           
           lib::RangeIter<EIter>
           allElms ()
             {
               return eachElm (elms_);
             }
        };
      
      
      struct ProvisionCluster
        : Cluster
        {
          POA*
          find_latest_solution (POA& requestElm)
            {
              typedef typename EntryList::reverse_iterator RIter;
              Binding::Matcher pattern (requestElm.getMatcher());
              for (RIter ii=this->elms_.rbegin();
                   ii!=this->elms_.rend();
                   ++ii )
                if (ii->first.matches (pattern))
                  return ii->second;
              
              return NULL;
            }
          
          void
          publish_latest_solution (POA& requestElm)
            {
              POA* solution = find_latest_solution (requestElm);
              if (solution)
                 // found the most recent advice provision satisfying the (new) request
                //  thus publish this new advice solution into the request object
                requestElm.setSolution (solution);
              else
                requestElm.setSolution ( NULL );
                //  report "no solution" which causes a default solution to be used
            }
        };
      
      struct RequestCluster
        : Cluster
        {
          void
          publish_all_solutions (POA& provisionElm)
            {
              Binding::Matcher pattern (provisionElm.getMatcher());
              for (EIter ii=this->elms_.begin();
                   ii!=this->elms_.end();
                   ++ii )
                if (pattern.matches (ii->first))
                   // the given (new) advice provision satisfies this request
                  //  thus publish this new advice solution into the request object
                  ii->second->setSolution (&provisionElm);
            }
          
          void
          retract_all_solutions (POA const& oldProv, ProvisionCluster& possibleReplacementSolutions)
            {
              Binding::Matcher pattern (oldProv.getMatcher());
              for (EIter ii=this->elms_.begin();
                   ii!=this->elms_.end();
                   ++ii )
                if (pattern.matches (ii->first))
                   // the old advice provision (to be dropped) satisfied this request
                  //  which thus needs to be treated anew (could cause quadratic complexity)
                  possibleReplacementSolutions.publish_latest_solution (*(ii->second));
            }
          
          void
          rewrite_all_solutions (POA const& oldProv, POA& newProv, ProvisionCluster& possibleReplacementSolutions)
            {
              Binding::Matcher oldPattern (oldProv.getMatcher());
              Binding::Matcher newPattern (newProv.getMatcher ());
              for (EIter ii=this->elms_.begin();
                   ii!=this->elms_.end();
                   ++ii )
                if (newPattern.matches (ii->first))
                  ii->second->setSolution (&newProv);
                else
                if (oldPattern.matches (ii->first))
                  possibleReplacementSolutions.publish_latest_solution (*(ii->second));
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
          requestEntries_[key].append (entry);  // might throw
          provisionEntries_[key].publish_latest_solution (entry);
        }
      
      /** @note explicitly relying on the implementation of \c ==
       *        which checks only the memory location of the Request.
       *        Thus we can use the already modified Request to find
       *        the old entry within the index pointing to this Request.
       *  @param oKey the binding hash value prior to modification
       */      
      void
      modifyRequest (HashVal oKey, POA& entry)
        {
          HashVal nKey (hash_value(entry));
          if (oKey != nKey)
            {
              requestEntries_[nKey].append (entry);   // might throw
              requestEntries_[oKey].remove (entry);
            }
          else
            { // rewrite Entry to include the new binding
              requestEntries_[nKey].overwrite (entry, entry);
            }
          provisionEntries_[nKey].publish_latest_solution (entry);
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
          provisionEntries_[key].append (entry);  // might throw
          requestEntries_[key].publish_all_solutions (entry);
        }
      
      void
      modifyProvision (POA const& oldRef, POA& newEntry)
        {
          HashVal oKey (hash_value(oldRef));
          HashVal nKey (hash_value(newEntry));
          if (oKey != nKey)
            {
              provisionEntries_[nKey].append (newEntry);  // might throw, in which case it has no effect
              provisionEntries_[oKey].remove (oldRef);
              requestEntries_[nKey].publish_all_solutions (newEntry);
              requestEntries_[oKey].retract_all_solutions (oldRef, provisionEntries_[oKey]);
            }
          else
            {
              provisionEntries_[nKey].overwrite (oldRef, newEntry);
              requestEntries_[nKey].rewrite_all_solutions (oldRef,newEntry, provisionEntries_[nKey]);
            }
        }
      
      void
      removeProvision (POA const& refEntry)
        {
          HashVal key (hash_value(refEntry));
          provisionEntries_[key].remove (refEntry);  // NO_THROW
          requestEntries_[key].retract_all_solutions (refEntry, provisionEntries_[key]);
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
        }                       // note: even just lookup might create a new (empty) cluster;
                               //        thus the tables are defined as mutable 
      
      
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
      
      void verify_Entry   (Entry const&, HashVal)  const;
      void verify_Request (Entry const&, HashVal)  const;
    };
  
  
  
  
  
  
  
  
  
  
  
  /* == Self-Verification == */
  
  namespace { // self-check implementation helpers...
    
    LUMIERA_ERROR_DEFINE(INDEX_CORRUPTED, "Advice-Index corrupted");
    
    struct SelfCheckFailure
      : error::Fatal
      {
        SelfCheckFailure (Literal failure)
          : error::Fatal (string("Failed test: ")+failure
                         ,LERR_(INDEX_CORRUPTED))
          { }
      };
  }
  
  
  
  
  /** Advice index self-verification: traverses the tables to check
   *  each entry is valid. Moreover, when a advice request has a stored solution
   *  which points back into the current advice provisions, this solution will be
   *  re-computed with the current data to prove it's still valid.
   *  @note expensive operation
   */
  template<class POA>
  bool
  Index<POA>::isValid()  const
  {
    typedef typename RTable::const_iterator RTIter;
    typedef typename PTable::const_iterator PTIter;
    
    try {
        for (PTIter ii =provisionEntries_.begin();
             ii != provisionEntries_.end(); ++ii)
          {
            HashVal hash (ii->first);
            Cluster& clu = unConst(ii->second);
            for_each (clu.allElms(), &Index::verify_Entry, this, _1, hash);
          }
        for (RTIter ii=requestEntries_.begin();
             ii != requestEntries_.end(); ++ii)
          {
            HashVal hash (ii->first);
            Cluster& clu = unConst(ii->second);
            for_each (clu.allElms(), &Index::verify_Request, this, _1, hash);
          }
        return true;
      }
    
    catch(SelfCheckFailure& failure)
      {
        lumiera_error();
        ERROR (library, "%s", failure.what());
      }
    return false;
  }
  
  
  
#define VERIFY(_CHECK_, DESCRIPTION) \
      if (!(_CHECK_))                 \
        throw SelfCheckFailure ((DESCRIPTION));
  
  
  template<class POA>
  void
  Index<POA>::verify_Entry (Entry const& e, HashVal hash)  const
  {
    VERIFY (hash == hash_value(e.first), "Wrong bucket, hash doesn't match bucket");
    VERIFY (e.second,                    "Invalid Entry: back-link is NULL");
  }
  
  template<class POA>
  void
  Index<POA>::verify_Request (Entry const& e, HashVal hash)  const
  {
    verify_Entry (e,hash);
    POA& request = *(e.second);
    const POA* solution (request.getSolution());
    if (solution && hasProvision(*solution))
      {
        POA* currentSolution = provisionEntries_[hash].find_latest_solution (request); 
        VERIFY (e.first.matches (solution->getMatcher()),"stored advice solution not supported by binding match");
        VERIFY (bool(currentSolution),                   "unable to reproduce stored solution with the current provisions")
        VERIFY (solution == currentSolution,             "stored advice solution isn't the topmost solution for this request")
      }
  }
  
#undef VERIFY
  
  
}} // namespace lumiera::advice
#endif
