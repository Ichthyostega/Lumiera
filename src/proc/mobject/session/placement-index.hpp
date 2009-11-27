/*
  PLACEMENT-INDEX.hpp  -  tracking individual Placements and their relations
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file placement-index.hpp
 ** Key interface of the session implementation datastructure.
 ** The PlacementIndex is attached to and controlled by the SessionImpl.
 ** Client code is not intended to interface directly to this API. Even
 ** Proc-Layer internal facilities use the session datastructure through
 ** SessionServices. Embedded within the implementation of PlacementIndex
 ** is a flat table structure holding all the Placement instances \em contained
 ** in the session. Any further structuring exists on the logical level only.
 ** 
 ** \par PlacementIndex, PlacementRef and MObjectRef
 ** TODO
 ** 
 ** \par Querying and contents discovery
 ** TODO 
 **
 ** @see PlacementRef
 ** @see PlacementIndex_test
 **
 */



#ifndef MOBJECT_PLACEMENT_INDEX_H
#define MOBJECT_PLACEMENT_INDEX_H

//#include "pre.hpp"
//#include "proc/mobject/session/locatingpin.hpp"
//#include "proc/asset/pipe.hpp"
#include "lib/util.hpp"
#include "lib/factory.hpp"
#include "lib/itertools.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/placement-ref.hpp"

#include <tr1/memory>
#include <tr1/unordered_map>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <vector>


namespace mobject {

  class MObject;
  
namespace session {
  
  LUMIERA_ERROR_DECLARE (NOT_IN_SESSION);  ///< referring to a Placement not known to the current session
  
  
  using lib::factory::RefcountFac;
  using std::tr1::shared_ptr;
  using boost::scoped_ptr;
  
  
  class PlacementIndex;
  typedef shared_ptr<PlacementIndex> PPIdx;
  
  /**
   * Structured compound of Placement instances
   * with lookup capabilities. Core of the session datastructure.
   * Adding a Placement creates a separate instance within this network,
   * owned and managed by the backing implementation. All placements are
   * related in a tree-like hierarchy of scopes, where each Placement is
   * within the scope of a parent Placement. There is an additional
   * reverse index, allowing to find the immediate children of any
   * given Placement efficiently. All lookup is based on the
   * Placement's hash-IDs.
   */
  class PlacementIndex
    : boost::noncopyable
    {
      class Table;
      
      scoped_ptr<Table> pTab_;
      
      
      typedef PlacementMO::ID _PID;
      typedef std::tr1::unordered_multimap<_PID,_PID>::iterator ScopeIter;
      
      
      
    public:
      typedef Placement<MObject> PlacementMO;
      typedef PlacementRef<MObject> PRef;
      typedef PlacementMO::ID const& ID;
      
      typedef lib::TransformIter<lib::RangeIter<ScopeIter>, PlacementMO> iterator;
      
      
      PlacementMO& find (ID)  const;
      
      template<class MO>
      Placement<MO>&  find (PlacementMO::Id<MO>)  const;
      template<class MO>
      Placement<MO>&  find (PlacementRef<MO> const&) const;
      
      PlacementMO& getScope (PlacementMO const&)  const;
      PlacementMO& getScope (ID)                  const;
      
      iterator getReferrers (ID)                  const;
      
      
      /** retrieve the logical root scope */
      PlacementMO& getRoot()                      const;
      
      size_t size()                               const;
      bool contains (PlacementMO const&)          const;
      bool contains (ID)                          const;
      
      
      
      
      /* == mutating operations == */
      
      ID   insert (PlacementMO const& newObj, PlacementMO const& targetScope);
      bool remove (PlacementMO&);
      bool remove (ID);
      
      
      typedef RefcountFac<PlacementIndex> Factory;
      
      static Factory create;
      
      ~PlacementIndex();
      
      void clear();
      
    protected:
      PlacementIndex() ;
      
      friend class lib::factory::Factory<PlacementIndex, lib::factory::Wrapper<PlacementIndex, shared_ptr<PlacementIndex> > >;
    };
  
  
  
  
  
  
  
  
  /* === forwarding implementations of the templated API === */
  
  template<class MO>
  inline Placement<MO>&
  PlacementIndex::find (PlacementMO::Id<MO> id)  const
  {
    PlacementMO& result (find (id));
    REQUIRE (INSTANCEOF (MO, &result) );
    return static_cast<Placement<MO>&> (result);
  }
  
    
  template<class MO>
  inline Placement<MO>&
  PlacementIndex::find (PlacementRef<MO> const& pRef)  const
  {
    PlacementMO::Id<MO> id (pRef);
    return find (id);
  }
  
  
  inline Placement<MObject>&
  PlacementIndex::getScope (PlacementMO const& p)  const
  {
    return getScope(p.getID()); 
  }
  
  inline bool
  PlacementIndex::contains (PlacementMO const& p)  const
  {
    return contains (p.getID());
  }
  
  inline bool
  PlacementIndex::remove (PlacementMO& p)
  {
    return remove (p.getID());
  }
  
  
  
}} // namespace mobject::session
#endif
