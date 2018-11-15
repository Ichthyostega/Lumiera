/*
  ELEMENT-QUERY.hpp  -  session sub-interface to query and retrieve elements

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


/** @file element-query.hpp
 ** Search and query services to discover contents of the session
 ** 
 ** @todo WIP implementation of session core from 2010
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#ifndef PROC_MOBJECT_SESSION_ELEMENT_QUERY_H
#define PROC_MOBJECT_SESSION_ELEMENT_QUERY_H


#include "steam/mobject/placement.hpp"
#include "steam/mobject/mobject-ref.hpp"
#include "steam/mobject/session/specific-contents-query.hpp"
#include "steam/mobject/session/session-service-explore-scope.hpp"
#include "lib/nocopy.hpp"

#include <functional>



namespace proc {
namespace mobject {
namespace session {
  
  
  using std::function;
  
  
  namespace { // type matching helper
                                                             //////////////////////////////TICKET #644  combine/clean up! see also specific-contents-query.hpp
    template<class PRED>
    struct _PickRes;
    
    template<class MO>
    struct _PickRes<function<bool(Placement<MO> const&)> >
      {
        typedef MO Type;
        typedef MORef<MO> Result;
        typedef typename ScopeQuery<MO>::iterator Iterator;
      };
    
    template<class MO>
    struct _PickRes<bool(&)(Placement<MO> const&)>
      {
        typedef MO Type;
        typedef MORef<MO> Result;
        typedef typename ScopeQuery<MO>::iterator Iterator;
      };
    
    template<class MO>
    struct _PickRes<bool(*)(Placement<MO> const&)>
      {
        typedef MO Type;
        typedef MORef<MO> Result;
        typedef typename ScopeQuery<MO>::iterator Iterator;
      };
  }
  
  
  
  
  /**
   * Access point to session global search and query functions.
   * This sub-component of the public session interface allows to
   * search and retrieve objects from the high-level-model, by type
   * and using additional filter predicates.
   * 
   * WIP-WIP-WIP as of 6/2010  -- this might evolve into an extended 
   * query facility, using specialised sub-indices and dedicated queries.
   * For now the motivation to package this as a separate interface module
   * was just to reduce the includes on the top level session API and to allow
   * for templated search functions, based on function objects.
   * 
   * @see session-element-query-test.cpp demo test
   * @see struct-factory-impl.hpp usage example
   */
  class ElementQuery
    : util::NonCopyable
    {
      
    public:
      
      /** pick the first element from session satisfying a predicate. 
       *  @param searchPredicate applied to \c Placement<MO> for filtering
       *  @return MObject ref to the fist suitable element. Might be an empty MObjectRef.
       *  @note  the embedded MObject subtype (MO) causes an additional filtering
       *         on that specific kind of MObject (e.g. considering just Clips)
       *  @warning be sure the passed predicate actually takes a `Placement<XX> const&`
       *         with XX being the correct type. Note the \c const& -- Failing to do so shows up as
       *         compiler error "no suitable function pick(.....)"
       */
      template<typename PRED>
      typename _PickRes<PRED>::Result
      pick (PRED const& searchPredicate)
        {
          typedef typename _PickRes<PRED>::Result ResultRef;
          typedef typename _PickRes<PRED>::Iterator Iterator;
          
          Iterator iter (pickAllSuitable ( SessionServiceExploreScope::getScopeRoot()
                                         , searchPredicate
                                         , SessionServiceExploreScope::getResolver()
                                         ));
          ResultRef found;
          if (iter)                 // in case there is an result
            found.activate(*iter); //  pick the first element found...
          return found;           //   or return an empty MObjectRef else
        }
      
    };
  
  
  
}}} // namespace proc::mobject::session
#endif
