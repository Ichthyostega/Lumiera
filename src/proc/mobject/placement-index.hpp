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
#include "proc/mobject/placement.hpp"
#include "proc/mobject/placement-ref.hpp"
#include "proc/mobject/session/query-resolver.hpp"

#include <tr1/memory>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <vector>


namespace mobject { ///////////////////////////////////////////TODO: shouldn't this go into namespace session ?
  
  using lib::factory::RefcountFac;
  using std::tr1::shared_ptr;
  using boost::scoped_ptr;
  using std::vector;
  
  class MObject;
  
  
  /**
   */
  class PlacementIndex
    : public session::QueryResolver        ////////TODO: really inherit here?
//  , boost::noncopyable                  ////////TODO : where to put the "noncopyable" base
    {
      class Table;
      
      scoped_ptr<Table> pTab_;
      
    public:
      typedef Placement<MObject> PlacementMO;
      typedef PlacementRef<MObject> PRef;
      typedef PlacementMO::ID const& ID;
      
      typedef session::Goal::QueryID const& QID;
      
      
      PlacementMO& find (ID)  const;
      
      template<class MO>
      Placement<MO>&  find (PlacementMO::Id<MO>)  const;
      template<class MO>
      Placement<MO>&  find (PlacementRef<MO> const&) const;
      
      PlacementMO& getScope (PlacementMO const&)  const;
      PlacementMO& getScope (ID)                  const;
      
      vector<PRef> getReferrers (ID)              const;
      
      
      /** retrieve the logical root scope */
      PlacementMO& getRoot()                      const;
      
      size_t size()                               const;
      bool contains (PlacementMO const&)          const;
      bool contains (ID)                          const;
      
////////////////////////////////////////////////////////////////TODO: refactor into explicit query resolving wrapper      
      template<class MO>
      typename session::Query<Placement<MO> >::iterator
      query (PlacementMO& scope)                  const;
      
      operator string()  const { return "PlacementIndex"; }
////////////////////////////////////////////////////////////////TODO: refactor into explicit query resolving wrapper      
      
      
      bool canHandleQuery(QID)                    const;
      
      
      /* == mutating operations == */
      
      ID   insert (PlacementMO& newObj, PlacementMO& targetScope);
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
  ////////////////TODO currently just fleshing  out the API; probably have to split off an impl.class; but for now a PImpl is sufficient...
  
    
  typedef shared_ptr<PlacementIndex> PPIdx;

  
  
  /** @internal there is an implicit PlacementIndex available on a global scale,
   *            by default implemented within the current session. This function allows
   *            to re-define this implicit index temporarily, e.g. for unit tests. */
  void
  reset_PlacementIndex(PPIdx const&) ;
  
  /** @internal restore the implicit PlacementIndex to its default implementation (=the session) */
  void
  reset_PlacementIndex() ;
  
  /** @internal access point for PlacementRef to the implicit global PlacementIndex */
  Placement<MObject> &
  fetch_PlacementIndex(Placement<MObject>::ID const&) ;
  
  
  
  
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
  
  
  /** @todo use query-resolver-test as an example.....
   *        return a result set object derived from Resolution
   *        For the additional type filtering: build a filter iterator,
   *        using a type-filtering predicate, based on Placement#isCompatible
   */
  template<class MO>
  inline typename session::Query<Placement<MO> >::iterator
  PlacementIndex::query (PlacementMO& scope)  const
  {
    UNIMPLEMENTED ("actually run the containment query");
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
  
  
  
} // namespace mobject
#endif
