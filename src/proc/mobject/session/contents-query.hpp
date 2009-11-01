/*
  CONTENTS-QUERY.hpp  -  query to discover the contents of a container-like part of the model
 
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


#ifndef MOBJECT_SESSION_CONTENTS_QUERY_H
#define MOBJECT_SESSION_CONTENTS_QUERY_H

//#include "proc/mobject/mobject.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/session/query-resolver.hpp"
//#include "lib/iter-adapter.hpp"

//#include <vector>
//#include <string>

//using std::vector;
//using std::string;

namespace mobject {
namespace session {
  
  
  
  /**
   * TODO type comment
   */
  template<class MO>
  class ContentsQuery
    : public Query<Placement<MO> >
    {
      typedef Query<Placement<MO> > _Query;
      
      QueryResolver const&  index_;
      PlacementMO const& container_;
      
      
    public:
      ContentsQuery (QueryResolver const& resolver, PlacementMO const& scope)
        : _Query (_Query::defineQueryTypeID (Goal::DISCOVERY))
        , index_(resolver)
        , container_(scope)
        { }
      
      typedef typename _Query::iterator iterator;
      
      iterator
      operator() ()  const
        {
          return _Query::resolveBy (index_);
        }
      
      PlacementMO const&
      searchScope ()
        {
          return container_;
        }
      
      /////////////////////TODO maybe exposing a content filter predicate from here?
      
    };
///////////////////////////TODO currently just fleshing the API
  
  
}} // namespace mobject::session
#endif
