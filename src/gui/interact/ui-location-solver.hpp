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


/** @file ui-location-solver.hpp
 ** A solver to match incomplete coordinate specifications against the actual UI topology.
 ** Within the Lumiera UI, a _component view_ is typically _created or retrieved_ to live at some position
 ** within the tree-like topology of the interface. Such happens as a consequence of interaction or other
 ** events, and the logic as to where and how to place a new UI element shall not be intermingled with the
 ** actual event handling code. Rather, the ViewLocator, as a service related to the InteractionDirector,
 ** can be invoked to draw on some default configuration plus the actual UI topology present at this time.
 ** 
 ** ## Pattern matching against the actual UI-tree
 ** 
 ** This Location solver is based on the pattern matching algorithm [embedded](\ref ui-coord-resolver.cpp)
 ** within the UICoordResolver component. This mechanism allows to treat (typically) incomplete coordinate
 ** specifications as rules for locating an element. Several such coordinate _clauses_ can be combined into
 ** a _disjunctive_ LocationRule, which is evaluated by matching the clauses one by one, in given order,
 ** against the currently existing UI tree (topology). Each clause is evaluated individually from scratch
 ** (there is no common variable binding); the first clause to produce a successful match is used as
 ** solution -- with any placeholders replaced by the actually matching UI elements.
 ** 
 ** ## Default View location configuration DSL
 ** 
 ** Whenever a new UI element of a given kind is to be created, we query a standard location configuration
 ** to determine it's actual location within the interface. This standard configuration is known as
 ** ["ViewSpec DSL"](\ref id-scheme.hpp) and hard-wired into the UI code. Based on the aforementioned
 ** pattern matching rules, it allows to express placement rules dependent on the already existing UI.
 ** There are two kinds of location clauses
 ** - the standard rules describe an element required to exist. Typically this is the _parent element_
 **   of the UI widget in question. But it is also possible to write clauses directly mentioning this
 **   element, in which case such an element must already exist in the UI and will be retrieved as result.
 ** - the more relaxed _create clauses_ describe a new location / path within the UI-tree, meaning that
 **   any (parent) elements not yet present are to be created. A _create clause_ is defined within the
 **   DSL by ending a UI coordinate specification with the term `.create()`. It may still be incomplete
 **   (i.e. contain wildcards), which means, that the first explicitly given element after (below) the
 **   wildcards must exist in the tree, to allow for an unambiguous pattern match. Otherwise, for
 **   creating a new path completely from scratch, all elements have to be given explicitly.
 ** As a minimum requirement, each LocationRule should be concluded with such a "catch-all" explicit
 ** create clause, which describes the standard location of the element in question.
 ** 
 ** @see UILocationSolver_test
 ** @see ViewSpecDSL_test
 ** @see UICoordResolver
 ** @see view-locator.hpp
 */


#ifndef GUI_INTERACT_UI_LOCATION_SOLVER_H
#define GUI_INTERACT_UI_LOCATION_SOLVER_H

#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/format-util.hpp"
#include "gui/interact/ui-coord.hpp"
#include "gui/interact/ui-coord-resolver.hpp"
#include "lib/nocopy.hpp"

#include <utility>
#include <vector>
#include <string>


namespace gui {
namespace interact {
  
  using lib::Literal;
  using std::string;
  using std::move;
  
  
  class LocationQuery;
  using LocationQueryAccess = std::function<LocationQuery&()>;
  
  
  
  
  /**
   * A single location specification to be matched and fulfilled.
   * It is created from a -- typically incomplete -- UICoord spec,
   * which in turn can be built through a DSL notation.
   * @todo maybe add a flag to require the current query goal to exist in tree //////////////////////////////TICKET #1130
   */
  struct LocationClause
    : util::NonCopyable
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
      
      operator string() const;
    };
  
  
  /**
   * A rule to determine some location by matching against the UI-tree.
   * It is comprised of one or several disjunctive [clauses](\ref LocationClause),
   * each of which is a coordinate pattern to be matched. The clauses are tried
   * in order and the first successfully matched clause wins.
   */
  class LocationRule
    : util::NonCopyable
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
      
      operator string() const;
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
  
  
  
  /* === diagnostics === */
  
  inline
  LocationClause::operator string()  const
  {
    return string{pattern}
         + (createParents? " create!":"");
  }
  inline
  LocationRule::operator string()  const
  {
    return "=~\t.. "
         + util::join(clauses_, "\n\tOR ");
  }
  
  
  
  
  /**
   * Service to determine the location of an UI component view.
   * @see LocationRule
   * @see UILocationSolver_test::simple_usage_example()
   */
  class UILocationSolver
    : util::NonCopyable
    {
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
      solve (LocationRule const& rule, size_t depth, Literal elementTypeID)
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
              preprocess (resolver, clause);
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
      /** perform adjustments on the current pattern to support some very specific situations
       * - when we want to create a new panel (or re-use an existing of the same name), on top
       *   of an existing (but irrelevant) perspective, we mark this perspective as "just there".
       *   This allows to get a new path as solution, which is just covered up to and including
       *   that perspective; without special treatment, such a match would otherwise be rejected.
       */
      void
      preprocess (UICoordResolver& builder, LocationClause const& clause)
        {
          if (clause.createParents
              and clause.pattern.isComplete())
            {
              builder.existentiallyQuantify (UIC_PERSP);
            }
        }
    };
  
  
  
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_UI_LOCATION_SOLVER_H*/
