/*
  DEFS-REGISTRY.hpp  -  implementation of the default object store

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file defs-registry.hpp
 ** A piece of implementation code factored out into a separate header (include).
 ** Only used in defs-manager.cpp and for the unit tests. We can't place it into
 ** a separate compilation unit, because defs-manager.cpp defines some explicit
 ** template instantiation, which cause the different Slots of the DefsrRegistry#table_
 ** to be filled with data and defaults for the specific Types.
 ** 
 ** @see mobject::session::DefsManager
 ** @see DefsRegistryImpl_test
 **
 */




#ifndef LUMIERA_QUERY_DEFS_REGISTRY_H
#define LUMIERA_QUERY_DEFS_REGISTRY_H


#include "lib/p.hpp"
#include "lib/util.hpp"
#include "lib/util-foreach.hpp"
#include "lib/sync-classlock.hpp"
#include "common/query.hpp"

#include <set>
#include <vector>
#include <tr1/memory>
#include <boost/format.hpp>
#include <boost/utility.hpp>
#include <boost/lambda/lambda.hpp>


namespace lumiera{
namespace query  {
  
  using lib::P;
  using lib::ClassLock;
  using std::tr1::weak_ptr;
  
  using std::string;
  using boost::format;
  using boost::lambda::_1;
  using boost::lambda::var;  
  
  namespace impl {
    
    namespace {
      uint maxSlots (0); ///< number of different registered Types
      format dumpRecord ("%2i| %64s --> %s\n");
    }
    
    
    struct TableEntry 
      {
        virtual ~TableEntry() {};
      };
    
    /** we maintain an independent defaults registry
     *  for every participating kind of object. */
    typedef std::vector< P<TableEntry> > Table;
    
    
    /**
     * holding a single "default object" entry 
     */
    template<class TAR>
    struct Record
      {
        uint degree;
        Query<TAR> query;
        weak_ptr<TAR> objRef;
        
        Record (const Query<TAR>& q, const P<TAR>& obj)
          : degree (lumiera::query::countPred ("TODO")),//q)),////////////////////////////////////////////////////////////////////////////////////////////TODO
            query (q),
            objRef (obj)
          { 
            UNIMPLEMENTED("Query remolding");////////////////////////////////////////////////////////////////////////////////////////////TODO
          }
        
        
        struct Search  ///< Functor searching for a specific object
          {
            Search (const P<TAR>& obj)
              : obj_(obj) { }
            
            const P<TAR>& obj_;
            
            bool 
            operator() (const Record& rec)
            {
              P<TAR> storedObj (rec.objRef.lock());
              return storedObj && (storedObj == obj_);
            }
          };
        
        struct OrderRelation
          {
            inline bool
            operator() (Record one, Record two) ///< @note doesn't touch the objRef
              {
                UNIMPLEMENTED ("arbitrary total ordering of queries");
                return (  one.degree < two.degree
                       ||(one.degree == two.degree && false)//one.query < two.query)////////////////////////////////////////////////////////////////////////////////////////////TODO
                       );
              }
          };
        
        operator string ()  const { return str (dumpRecord % degree % query % dumpObj()); }
        string  dumpObj ()  const { P<TAR> o (objRef.lock()); return o? string(*o):"dead"; }
      };
      
    /** every new kind of object (Type) creates a new
     *  slot in the main Table holding all registered 
     *  default objects. Each slot actually holds a
     *  separate tree (set) of registry entries
     */
    template<class TAR>
    struct Slot : public TableEntry
      {
        typedef typename Record<TAR>::OrderRelation Ordering;
        typedef std::set<Record<TAR>, Ordering> Registry;
        
        Registry registry;
        static size_t index; ///< where to find this Slot in every Table
        
        static Registry&
        access (Table& table)
          {
            if ( !index 
               || index > table.size() 
               ||!table[index-1])
              createSlot (table);
            
            ASSERT (0 < index && index<=table.size() && table[index-1]);
            Slot* item = static_cast<Slot*> (table[index-1].get());
            return item->registry;
          }
        
      private:
        static void
        createSlot (Table& table)
          {
            ClassLock<TableEntry> guard();
            if (!index)
              index = ++maxSlots;
            if (index > table.size())
              table.resize (index);
            table[index-1].reset(new Slot);   
          }
      };
      
      
    // static vars to organise one Table Slot per type....
    template<class TAR>
    size_t Slot<TAR>::index (0);
    
    
    
    
    /**
     * @internal Helper for organising preconfigured default objects.
     * Maintains a collection of objects known or encountered as "default"
     * for a given type. This collection is ordered by "degree of constriction",
     * which is implemented by counting the number of predicates in the query
     * used to define or identify each object.
     * Accessing an object identified by a query causes the query to be resolved
     * (executed in prolog), which may result in some additional properties of
     * the object being bound or specified.
     * @todo as of 3/2008 the real query implementation is missing, and the
     * exact behaviour has to be defined.
     */
    class DefsRegistry : private boost::noncopyable
      {
        Table table_;
        
      public:
        /** used for enumerating solutions */
        template<class TAR>
        class Iter          
          {
            friend class DefsRegistry;
            typedef typename Slot<TAR>::Registry::iterator II;
            
            II p,i,e;
            P<TAR> next, ptr;
            
            Iter (II from, II to) ///< just enumerates the given range 
              : p(from), i(from), e(to)
              {
                if (i!=e) ++i;  // p is next to be tested, i always one ahead
                operator++ ();
              }
            
            Iter (II match, II from, II to) ///< returns direct match first, then enumerates 
              : p(match), i(from), e(to)
              { 
                operator++ ();  // init to first element (or to null if empty)
              }
            
            P<TAR> findNext ()  throw()
              {
                while (!next)
                  {
                    if (p==e) break;
                    next = p->objRef.lock();
                    p = i++;
                  }
                return next;
              }
            
          
          public:
            P<TAR> operator* ()    { return ptr; }
            bool  hasNext ()       { return next || findNext(); }
            Iter& operator++ ()
              { 
                ptr=findNext();
                next.reset();
                return *this; 
              }
          };
        
        /** find a sequence of "default" objects possibly matching the query.
         *  If there was a registration for some object of the given kind with
         *  the \em same query, this one will be first in the sequence. Besides,
         *  the sequence will yield all still existing registered "default" objects
         *  of this kind, ordered ascending by "degree of constriction", i.e. starting
         *  with the object registered together with the shortest query.
         *  @return a forward input iterator yielding this sequence
         *  @note none of the queries will be evaluated (we're just counting predicates)
         */
        template<class TAR>
        Iter<TAR> candidates (const Query<TAR>& query)
          {
            P<TAR> dummy;
            Record<TAR> entry (query, dummy);
            typedef typename Slot<TAR>::Registry Registry;
            Registry& registry = Slot<TAR>::access(table_);
            
            // try to get a possible direct match (same query)
            typename Registry::iterator pos = registry.find (entry);
            typename Registry::iterator end = registry.end();
            
            if (pos==end)
              return Iter<TAR> (registry.begin(), end);        // just enumerate contents
            else
              return Iter<TAR> (pos, registry.begin(), end); // start with direct match
          }
        
        
        /** register the object as being "default" when searching something
         *  similar as designated by the given query. Only a weak ref is stored.
         *  @param obj Will be rebound, if another object is already stored.
         *  @return true if object has actually been stored, false if another
         *          object is registered for exactly the same query. In this
         *          case, also the param obj shared-ptr is rebound!
         */
        template<class TAR>
        bool put (P<TAR>& obj, const Query<TAR>& query)
          {
            Record<TAR> entry (query, obj);
            typedef typename Slot<TAR>::Registry Registry;
            typedef typename Registry::iterator RIter;
            
            Registry& registry = Slot<TAR>::access(table_);
            RIter pos = registry.lower_bound (entry);
            if (  pos!=registry.end()
               && pos->query == query)
              {
                P<TAR> storedObj (pos->objRef.lock());
                if (storedObj)
                  return (storedObj == obj);
                else
                  // use the opportunity and purge the expired entry
                  registry.erase(pos++);
              }
            // no existing entry....
            registry.insert(pos, entry);
            ENSURE (registry.find (entry) != registry.end());
            return true;
          }
        
        
        /** if this object is registered as "default" in some way, drop the registration.
         *  @return false if the object wasn't registered at all.
         */
        template<class TAR>
        bool forget (const P<TAR>& obj)
          {
            typedef typename Slot<TAR>::Registry Registry;
            typedef typename Record<TAR>::Search SearchFunc;
            
            Registry& registry = Slot<TAR>::access(table_);
            return util::remove_if(registry, SearchFunc (obj));
          }
        
        
        /** helper for diagnostics.
         *  @note to use it, your objects need an operator string()
         */
        template<class TAR>
        string dump()
          {
            string res;
            util::for_each ( Slot<TAR>::access(table_)
                           , var(res) += _1
                           );
            return res;
          }
      };
    
    
    
  } // (End) impl namespace
  
  using impl::DefsRegistry;
  
  
}} // namespace lumiera::query
#endif
