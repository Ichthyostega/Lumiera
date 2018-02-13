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
  
  
  
  struct LocationClause
    : boost::noncopyable
    {
      UICoord pattern;
      bool createParents;
      
      
      LocationClause (UICoord && locationPattern, bool allowCreate =false)
        : pattern{move (locationPattern)}
        , createParents{allowCreate}
        { }
      LocationClause (LocationClause && rr)
        : pattern{move (rr.pattern)}
        , createParents{rr.createParents}
        { }
    };
  
  
  class LocationRule
    : boost::noncopyable
    {
      using Clauses = std::vector<LocationClause>;
      
      Clauses clauses_;
      
    public:
      LocationRule (LocationClause && firstRule)
        : clauses_{}
        {
          this->append (move (firstRule));
        }
      LocationRule (LocationRule && rr)
        : clauses_{move (rr.clauses_)}
        { }
      
      
      LocationRule&&
      append (LocationClause && furtherRule)
        {
          clauses_.emplace_back (move (furtherRule));
          return move (*this);
        }
      
      
      using iterator = lib::RangeIter<Clauses::const_iterator>;
      iterator begin() const { return iterator{clauses_.begin(), clauses_.end()}; }
      iterator end()   const { return iterator(); }
    };
  
  
  /* ==== Support of UI-Coordinate notation within the ViewSpec-DSL ==== */
  
  /** interprets the current (inline) contents of an UICoord builder expression
   * as a standard LocationClause, which has the meaning of "when an element
   * exists at the location XYZ in the real UI"
   * @warning like all the UICoord::Builder functions, the contents are moved.
   *          Thus, after using this conversion path _once_, the Builder _is defunct_
   */
  inline
  UICoord::Builder::operator LocationClause()
  {
    return LocationClause{move(*this), false};
  }
  
  /** interprets the current (inline) builder contents as _create clause_,
   * which has the meaning "create a new element XYZ when possible" */
  inline LocationClause
  UICoord::Builder::create()
  {
    return LocationClause{move(*this), true};
  }
  
  
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
       * @return an explicit location, resolved against the current UI topology. May be empty
       * @remarks the returned path is either empty (no solution exists), or it is "partially covered"
       *        by the existing UI; here, the "covered" part are the already existing UI elements,
       *        while the remaining, uncovered extension describes additional elements to be created.
       *        When the resolution process found an already existing UI element, the returned path
       *        is completely covered. The degree of coverage of a path can be found out with the
       *        help of a UICoordResolver, which also needs a LocationQuery (service) to discover
       *        the currently existing UI topology.
       */
      UICoord
      solve (LocationRule const& rule, size_t depth, Symbol elementTypeID)
        {
          for (auto& clause : rule)
            {
              // Clauses which do not at least describe an element at parent level
              // will never lead to a solution and can thus be skipped
              if (depth+1 < clause.pattern.size()
                  or depth > clause.pattern.size())
                continue;
              
              // try to solve the current Clause by matching against real UI topology
              UICoordResolver resolver{clause.pattern, getLocationQuery()};
              resolver.coverPartially(); // now either holds a solution or is empty
              
              if (not isnil(resolver)                     // Solution found!
                  and (clause.createParents              //  The "create" case requires only some part to exist,
                       or resolver.isCoveredTotally())) //   while in the default case we demand complete coverage
                {
                  if (depth == clause.pattern.size())
                    // append ID of the new element to be created
                    // unless it's already there (and thus exists)
                    resolver.append (elementTypeID);
                  return move (resolver);
                      // use the first suitable solution and exit
                }
              else
                if (clause.createParents and clause.pattern.isExplicit())
                  // allow creation of a totally new path from scratch
                  // as long as it is complete and explicitly given
                  {
                    if (depth == clause.pattern.size())
                      return clause.pattern.append (elementTypeID);
                    else
                      return clause.pattern;
                  }
            }
           //all clauses tried without success...
          return UICoord();
        }
      
    private:
      
    };
  
  
  
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_UI_LOCATION_SOLVER_H*/
