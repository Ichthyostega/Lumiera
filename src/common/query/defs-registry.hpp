/*
  DEFS-REGISTRY.hpp  -  implementation of the default object store

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file defs-registry.hpp
 ** A piece of implementation code factored out into a separate header (include).
 ** Only used through defs-manager-impl.hpp and for the unit tests. We can't place it
 ** into a separate compilation unit, because config-resolver.cpp defines some explicit
 ** template instantiation, which cause the different Slots of the DefsrRegistry#table_
 ** to be filled with data and defaults for the specific Types.
 ** 
 ** Basically, this piece of code defines a specialised index / storage table to hold
 ** Queries-for-default objects. This allows to remember what actually was used as
 ** "default" solution for some query and to oder possible default solutions.
 ** @remarks as of 2017, we're still using a fake implementation of the resolution,
 **          no real resolution engine. While the basic idea of this "defaults registry"
 **          is likely to stay, the actual order relation and maybe even the components
 **          to be stored in this registry might be subject to change. 
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
#include "lib/format-string.hpp"
#include "lib/query-util.hpp"
#include "common/query.hpp"
#include "lib/nocopy.hpp"

#include <set>
#include <vector>
#include <memory>


namespace lumiera{
namespace query  {
  
  using lib::P;
  using lib::ClassLock;
  using std::weak_ptr;
  using std::string;
  
  namespace impl {
    
    namespace {
      using util::_Fmt;
      
      uint maxSlots (0); ///< number of different registered Types
      _Fmt dumpRecord ("%2i| %64s --> %s\n");
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
        QueryKey queryKey;
        weak_ptr<TAR> objRef;
        uint degree;
        
        Record (Query<TAR> const& q, P<TAR> const& obj)
          : queryKey (q)
          , objRef (obj)
          , degree(queryKey.degree())
          { }
        
        
        struct Search  ///< Functor searching for a specific object
          {
            Search (P<TAR> const& obj)
              : obj_(obj) { }
            
            P<TAR> const& obj_;
            
            bool 
            operator() (Record const& rec)
            {
              P<TAR> storedObj (rec.objRef.lock());
              return storedObj && (storedObj == obj_);
            }
          };
        
        friend bool
        operator< (Record one, Record two) ///< @note doesn't touch the objRef
        {
          return one.queryKey < two.queryKey;
        }
        
        operator string ()  const { return dumpRecord % degree % queryKey.display() % dumpObj(); }
        string  dumpObj ()  const { P<TAR> o (objRef.lock()); return o? string(*o):"dead"; }
      };
      
    /** every new kind of object (Type) creates a new
     *  slot in the main Table holding all registered 
     *  default objects. Each slot actually holds a
     *  separate tree (set) of registry entries
     */
    template<class TAR>
    struct Slot
      : public TableEntry
      {
        typedef std::set<Record<TAR>> Registry;
        
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
            ClassLock<TableEntry> guard;
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
    class DefsRegistry
      : util::NonCopyable
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
            
            P<TAR>
            findNext ()  ///< EX_FREE
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
        Iter<TAR>
        candidates (Query<TAR> const& query)
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
         *  @param obj to be recorded as "default". Only a weak pointer will be stored
         *  @retval true if object has actually been stored
         *  @retval false if another object is registered for exactly the same query
         *                Nothing is changed in this case
         */
        template<class TAR>
        bool
        put (P<TAR> const& obj, Query<TAR> const& query)
          {
            Record<TAR> entry (query, obj);
            typedef typename Slot<TAR>::Registry Registry;
            typedef typename Registry::iterator RIter;
            
            Registry& registry = Slot<TAR>::access(table_);
            RIter pos = registry.lower_bound (entry);
            if (  pos!=registry.end()
               && pos->queryKey == query)
              {
                P<TAR> storedObj (pos->objRef.lock());
                if (storedObj)
                  return (storedObj == obj);
                else
                  // use the opportunity to purge the expired entry
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
        bool
        forget (P<TAR> const& obj)
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
        string
        dump ()
          {
            string res;
            util::for_each ( Slot<TAR>::access(table_)
                           , [&] (Record<TAR>& entry)
                                 {
                                   res += string(entry);
                                 }
                           );
            return res;
          }
      };
    
    
    
  } // (End) impl namespace
  
  using impl::DefsRegistry;
  
  
}} // namespace lumiera::query
#endif
