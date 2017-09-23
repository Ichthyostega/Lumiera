/*
  VIEW-SPEC-DSL.hpp  -  configuration of view allocation patterns

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file view-spec-dsl.hpp
 ** A framework for configuration of view access and allocation patterns.
 ** Component views are building blocks of the Lumiera UI, and may, depending on their,
 ** be instantiated or allocated according to specific rules and patterns. And these might
 ** vary in accordance to the desired working style. To give a typical example, at times it
 ** might be mandatory to use a single, external output for all kind of media playback, while
 ** other users prefer the classical editing application layout with two media viewers side by
 ** side. And yet another working style would be to use a stack of media viewers allocated on
 ** demand in MRU-fashion.
 ** 
 ** To specify those standard behaviour patterns, we provide a small internal DSL to spell out
 ** the default configuration in a (hopefully) self explanatory way.
 ** @todo as of 9/2017, the intention to open this configuration DSL for some kind of
 **       user provided flexible configuration of screen layouts eventually; right now
 **       only the foundation for this flexible configuration is provided while the defaults
 **       are compiled in hard wired.
 ** 
 ** # Allocation of UI component views
 ** 
 ** Here, Allocation means
 ** - to constitute the desired element's identity
 ** - to consider multiplicity and possibly retrieve an existing instance
 ** - to determine the hosting location
 ** - possibly to instantiate and register a new instance
 ** - and finally to configure that instance for the desired role
 ** 
 ** ## View access
 ** 
 ** The global access point to component views is the ViewLocator within InteractionDirector,
 ** which exposes a generic access- and management API to
 ** - get (possibly create) some view of given type
 ** - get (possibly create) a view with specific identity
 ** - destroy a specific view
 ** Together these operations expose a strictly typed direct reference to the respective view component
 ** and thus allow to manage them like child objects. Operating on top of these primitive operations,
 ** the _configuration of view access patterns_ creates a flexible binding layer, which isolates the
 ** users of component views (typically other parts of the UI) from the actual mechanics of locating.
 ** While the client just retrieves a view instance, a dedicate _allocation logic_ ensures this view
 ** instance is placed at the desired place within the UI and manages the active view instances.
 ** 
 ** ## Configuration DSL
 ** 
 ** Access to component views typically happens through the ViewLocator provided by the InteractionDirector.
 ** The implementation of view access in turn redirects access through the configured patterns.
 ** - at [definition site](\ref id-scheme.hpp), explicit specialisations are given for the relevant types
 **   of component view
 ** - each of those general view configurations
 **     * defines the multiplicity allowed for this kind of view
 **     * defines how to locate this view
 ** ... and that location definition is given as a list of alternatives in order of precedence.
 ** That is, the system tries each pattern of location and uses the first one applicable.
 ** 
 ** Actually, the definitions and tokens used within the configuration DSL are implemented as _functors._
 ** At implementation level, the basic specification tokens are bound to the corresponding operations
 ** within ViewLocator and PanelLocator. Thus, the "value" retrieved from the configuration is actually
 ** a functor, which provides the desired behaviour pattern
 ** - the *locate* spec is evaluated to yield [user interface coordinates](\ref UICoord) representing the
 **   desired location of the view
 ** - the *count* spec is then evaluated as a second step, while providing the coordinates from first step.
 **   This results in calling a dedicated _view allocator_ to create a new view or re-use an existing one
 **   as appropriate.
 ** 
 ** @see ViewSpecDSL_test
 ** @see id-scheme.hpp
 ** @see view-locator.hpp
 ** @see interaction-director.hpp
 */


#ifndef GUI_INTERACT_VIEW_SPEC_DSL_H
#define GUI_INTERACT_VIEW_SPEC_DSL_H

#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "gui/interact/ui-coord.hpp"

//#include <boost/noncopyable.hpp>
#include <functional>
//#include <string>
//#include <memory>


namespace gui {
namespace interact {
  
//  using std::unique_ptr;
//  using std::string;
  
//  class GlobalCtx;
  
  
  
  /**
   * A specification to describe the desired location of a component view within the Lumiera UI.
   * ViewSpec is basically a set of [UI coordinates](\ref UICoord), with the additional possibility
   * of specifying several alternatives, with the intention to pick the first applicable one.
   * 
   * @todo initial draft as of 9/2017
   */
  class ViewSpec
    {
      
    public:
      ViewSpec(UICoord coordinates)
        {
          UNIMPLEMENTED ("build a view spec from explicitly given UI coordinates");
        }
      
      operator UICoord()
        {
          UNIMPLEMENTED ("resolve a view spec to yield explicit UI coordinates");
        }
    private:
      
    };
  
  
  /**
   * Allocator is a functor to resolve a given, desired location of a view within the UI, resulting
   * in creation or allocation of the view. The result of this invocation are the UI coordinates
   * of an existing or newly created view.
   */
  class Allocator
    : public std::function<UICoord(UICoord)>
    {
      
    };
  
  
  template<typename PAR>
  class AllocSpec
    : public std::function<Allocator(PAR)>
    {
    public:
      template<class FUN>
      AllocSpec(FUN fun)
        {
          UNIMPLEMENTED ("generate an allocator builder functor by partial function application");
        }
    };
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_VIEW_SPEC_DSL_H*/
