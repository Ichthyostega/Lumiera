/*
  UI-LOCATION-SOLVER.hpp  -  decide upon a possible location for some UI component

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file ui-location-resolver.hpp
 ** A solver to match incomplete coordinate specifications against the actual UI topology.
 ** Within the Lumiera UI, a _component view_ is typically _created or retrieved_ to live at some position
 ** within the tree-like topology of the interface. Such happens as a consequence of interaction or other
 ** events, and the logic as to where and how to place a new UI element shall not be intermingled with the
 ** actual event handling code. Rather, the ViewLocator, as a service related to the InteractionDirector,
 ** can be invoked to draw on some default configuration plus the actual UI topology present at this time.
 ** 
 ** @todo WIP 2/2018 early draft       ////////////////////////////////////////////////////////////TICKET #1127
 ** 
 ** @see UILocationResolver_test
 ** @see ViewSpecDSL_test
 ** @see UICoordResolver
 ** @see view-locator.hpp
 */


#ifndef GUI_INTERACT_UI_LOCATION_SOLVER_H
#define GUI_INTERACT_UI_LOCATION_SOLVER_H

#include "lib/error.hpp"
#include "lib/symbol.hpp"
//#include "lib/meta/function.hpp"
//#include "lib/meta/tuple-helper.hpp"
//#include "lib/meta/function-closure.hpp"
#include "gui/interact/ui-coord.hpp"
#include "gui/interact/ui-coord-resolver.hpp"

//#include <functional>
#include <boost/noncopyable.hpp>
#include <utility>
#include <vector>


namespace gui {
namespace interact {
  
//  using std::forward;
  using std::move;
  using lib::Symbol;
  
  
  class LocationQuery;
  using LocationQueryAccess = std::function<LocationQuery&()>;
  
  /** @internal access UI service to query and discover locations within UI topology */
  extern LocationQueryAccess loactionQuery;
  
  
  
  class LocationClause
    : boost::noncopyable
    {
      UICoord pattern_;
      bool createParents_;
      
    public:
      LocationClause (UICoord && locationPattern, bool createParents =false)
        : pattern_{move (locationPattern)}
        , createParents_{createParents}
        { }
      LocationClause (LocationClause && rr)
        : pattern_{move (rr.pattern_)}
        , createParents_{rr.createParents_}
        { }
      
      operator UICoord const&()  const
        {
          return pattern_;
        }
      
      size_t
      size()  const
        {
          return pattern_.size();
        }
    };
  
  
  class LocationRule
    : boost::noncopyable
    {
      using Clauses = std::vector<LocationClause>;
      
      Clauses clauses_;
      
    public:
      LocationRule (UICoord && firstRule)
        : clauses_{}
        {
          this->append (move (firstRule));
        }
      LocationRule (LocationRule && rr)
        : clauses_{move (rr.clauses_)}
        { }
      
      
      LocationRule&&
      append (UICoord && furtherRule)
        {
          clauses_.emplace_back (move (furtherRule));
          return move (*this);
        }
      
      
      using iterator = lib::RangeIter<Clauses::const_iterator>;
      iterator begin() const { return iterator{clauses_.begin(), clauses_.end()}; }
      iterator end()   const { return iterator(); }
    };
  
  
  /** DSL operator to assemble a sequence of clauses.
   * Introduced solely for the purpose of writing location specifications within the
   * [ViewSpec-DSL](\ref id-scheme.hpp), this operator acts on several UI-Coordinate specs
   * to create a sequence of clauses, to be checked against the currently existing UI topology,
   * in the given order, ranging from more specific to more general patterns.
   */
  inline LocationRule
  operator or (UICoord::Builder && firstRule, UICoord secondRule)
  {
    return LocationRule{move (firstRule)}
            .append (move (secondRule));
  }
  
  inline LocationRule&&
  operator or (LocationRule && ruleSet, UICoord furtherRule)
  {
    ruleSet.append (move (furtherRule));
    return move(ruleSet);
  }
  
  
  
  
  /**
   * Access or allocate a UI component view
   * 
   * @todo initial draft as of 2/2018 -- actual implementation need to be filled in
   */
  class UILocationSolver
    : boost::noncopyable
    {
//      ctrl::GlobalCtx& globals_;
      LocationQueryAccess getLocationQuery;
      
    public:
      explicit
      UILocationSolver (LocationQueryAccess accessor)
        : getLocationQuery{accessor}
        { }
      
      explicit
      UILocationSolver (LocationQuery& locationQueryService)
        : getLocationQuery{[&]() -> LocationQuery& { return locationQueryService; }}
        { }
      
      
      /**
       * Solve for a location according to the given location rule.
       * @param depth desired kind of UI element (and thus the depth in the UI topology tree)
       * @param elementType designator of the specific element to be created at that level
       * @return a explicit location, resolved against the current UI topology. May be empty
       * @remarks the returned path is either empty (no solution exists), or it is "partially covered"
       *        by the existing UI; here, the "covered" part are the already existing UI elements,
       *        while the remaining, uncovered extension describes additional elements to be created.
       *        When the resolution process found an already existing UI element, the returned path
       *        is completely covered. The degree of coverage of a path can be found out with the
       *        help of a UICoordResolver, which also needs a LocationQuery (service) to find out
       *        about the currently existing UI topology.
       */
      UICoord
      solve (LocationRule& rule, size_t depth, Symbol elementType)
        {
          for (auto& clause : rule)
            {
              if (clause.size() > depth+1) continue;
              UICoordResolver resolver{clause, getLocationQuery()};
              resolver.coverPartially();
              if (not isnil (resolver))
                return move (resolver);
              //////////////////////////////TODO this is only the more relaxed "create" case 
            }
        }
      
    private:
      
    };
  
  
  
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_UI_LOCATION_SOLVER_H*/
