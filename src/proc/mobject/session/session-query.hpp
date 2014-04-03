/*
  SESSION-QUERY.hpp  -  querying for session contents in various ways

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


/** @file session-query.hpp
 ** Supporting queries for session contents.
 ** 
 ** WIP-WIP as of 6/10
 ** 
 ** Idea is to collect here various bits required to issue queries
 ** and retrieve specific session contents, allowing to keep the
 ** main session.hpp more lightweight.
 ** 
 ** @see Session public API
 ** @see query-resolver.hpp
 ** @see scope-query.hpp 
 **
 */


#ifndef MOBJECT_SESSION_SESSION_QUERY_H
#define MOBJECT_SESSION_SESSION_QUERY_H

#include "proc/mobject/session.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/session/track.hpp"

#include <functional>



namespace proc {
namespace mobject {
namespace session {
  
  using std::bind;
  using std::function;
  using std::placeholders::_1;
  
  ////////////TODO: shall we use a sub-namespace "query" ??
  
  namespace { // query function implementations....
    
    inline bool
    _compareTrackID (Placement<Track> const& trackMO, string refTrackID)
    {
      return trackMO->isSameID (refTrackID);
    }
    
  }//(End) query functions
  
  
  
  
  /* ====== Predicates for picking objects ====== */
  
  /** yield a query predicate to pick a specific track */
  inline function<bool(Placement<Track> const&)>
  match_specificTrack (string const& trackID)
  {
    return bind (_compareTrackID, _1, trackID);
  }
  
}}} // namespace proc::mobject::session
#endif
