/*
  OUTPUT-MAPPING.hpp  -  generic interface for translation of output designations
 
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


#ifndef PROC_MOBJECT_OUTPUT_MAPPING_H
#define PROC_MOBJECT_OUTPUT_MAPPING_H

#include "proc/mobject/output-designation.hpp"
#include "lib/meta/function.hpp"
#include "lib/bool-checkable.hpp"
#include "lib/query.hpp"
#include "lib/util.hpp"

#include <boost/noncopyable.hpp>
#include <boost/operators.hpp>
#include <map>



namespace mobject {
  
  namespace { // Helper to extract and rebind definition types
    
    using std::tr1::function;
    using util::contains;
    
    template<class DEF>
    class _def
      {
        typedef asset::ID<asset::Pipe> PId;
        
        template<typename FUN>
        struct Rebind;
        
        template<typename RET>
        struct Rebind<RET(DEF::*)(PId)>
          {
            typedef RET Res;
          };
        
        typedef typeof(&DEF::output) OutputMappingMemberFunc;              // GCC extension: "typeof"
        typedef Rebind<OutputMappingMemberFunc> Rebinder;
        
      public:
        typedef typename Rebinder::Res Target;
        typedef function<Target(PId)>  OutputMappingFunc;
        
      };
    
  }
  
  using lumiera::Query;
  using asset::HashVal;
  
  
  /**
   * OutputMapping is a facility to resolve output designations.
   * For a given specification, resolution to the desired
   * target specification may be derived. Here, the 
   * type of the target specification is defined 
   * through the type parameter.
   * 
   * \par definition of specific mapping behaviour
   * 
   * This is an generic map-like container, acting as Interface to be used
   * in the signature of API functions either providing or requiring a Mapping.
   * For each distinct usage situation, an instantiation of this template should
   * be created, providing a <i>definition context</i> as template parameter.
   * Instances of this concrete mapping type may then be default constructed
   * and copied freely. The definition context is supposed to provide
   * - a functor \c DEF::output usable as function pipe-ID --> Target
   * - the concrete output-functor also defines the concrete Target type,
   *   which will be returned when accessing the OutputMapping
   */
  template<class DEF>
  class OutputMapping
    : public DEF
    {
      typedef _def<DEF> Setup;
      
      typedef asset::ID<asset::Pipe> PId;
      typedef asset::PPipe         PPipe;
      
      /* == mapping table storage == */
      std::map<HashVal,HashVal> table_;
      
    public:
      typedef typename Setup::Target   Target;
      
      // using default ctor and copy operations
      
      size_t size() const { return table_.size(); }
      bool empty()  const { return 0 == size(); }
      void clear()        { table_.clear(); }
      
      
      /**
       * @internal transient resolution wrapper to be exposed by map-style access.
       * A Resolver instance represents an output mapping result, yet to be fully resolved.
       * It is created on the stack by the OutputMapping container and internally wired
       * back to the container and the actually stored value (pipe-ID-hash) in the table.
       * Actually retrieving the result value by the client code triggers invocation
       * of the specific resolution functor, embedded in the definition context DEF,
       * which was given when instantiating the OutputMapping template.
       */
      class Resolver
        : public lib::BoolCheckable<Resolver
        , boost::equality_comparable<Resolver, Target,
          boost::equality_comparable<Resolver> 
                                  > >
        {
          OutputMapping& thisMapping_;
          HashVal& pID_;
          
          Resolver (OutputMapping& container, HashVal& resultVal)
            : thisMapping_(container)
            , pID_(resultVal)
            { }
          
          friend class OutputMapping;
          
          /* copy by clients prohibited */
          Resolver& operator= (Resolver const&);
          
          
          Target
          resolve()  const
            {
              REQUIRE (pID_);
              PId targetPipeID (pID_);
              return thisMapping_.resolve (targetPipeID);
            }
          
        public:
          void
          operator= (PId newId2map)
            {
              pID_ = newId2map;
            }
          
          void
          operator= (PPipe newPipe2map)
            {
              REQUIRE (newPipe2map);
              pID_ = newPipe2map->getID();
            }
          
          operator Target()
            {
              return resolve();
            }
          
          bool
          isValid()  const
            {
              return bool(pID_);
            }
          
          
          /* === equality comparisons (boost::operators) === */
          
          friend bool
          operator== (Resolver const& a, Resolver const& b)
          {
            return a.pID_ == b.pID_; 
          }                 // note: Resolver depends on tempate parameter DEF
                           //        All instances of DEF are considered equivalent!
          friend bool
          operator== (Resolver const& rr, Target const& tval)
          {
            return rr.resolve() == tval; 
          }
        };
      
      Resolver
      operator[] (PId sourcePipeID)
        {
          if (!contains (table_, sourcePipeID))
            {
              UNIMPLEMENTED ("how to resolve");
            }
          return buildResolutionWrapper (sourcePipeID);
        }
      
      Resolver
      operator[] (PPipe const& pipe)
        {
          REQUIRE (pipe);
          return (*this) [pipe->getID()];
        }
      
      Resolver
      operator[] (Query<asset::Pipe> const& query4pipe)
        {
          UNIMPLEMENTED ("lookup by extended query");
          HashVal hash4query;
          HashVal resulting_targetPipeID;
          table_[hash4query] = resulting_targetPipeID;
          return buildResolutionWrapper (hash4query);
        }
      
      
    private:
      Target
      resolve (PId resultingPipeID)
        {
          return DEF::output(resultingPipeID);
        }
      
      Resolver
      buildResolutionWrapper (HashVal tableSlot)
        {
          ASSERT (contains (table_, tableSlot));
          return Resolver (*this, table_[tableSlot]);
        }
    };
  
  
  
  
} // namespace mobject
#endif
