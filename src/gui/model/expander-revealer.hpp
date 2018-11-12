/*
  EXPANDER-REVEALER.hpp  -  functor components for standard UI element actions

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


/** @file expander-revealer.hpp
 ** Helper components to implement some standard UI-element actions by installing a functor.
 ** The protocol for operating UI elements connected to the [UI-Bus](\ref ui-bus.hpp) includes
 ** some generic operations, which need to be concretised for each actual usage situation.
 ** Moreover, there are many situations where implementing those actions does not make
 ** much sense, and thus they need to be treated as optional.
 ** - some widgets or elements can be expanded or collapsed, to show more information
 **   or to save screen real estate. Such widgets also expose an _expansion_ state.
 ** - sometimes it is necessary to bring a given widget into sight, be it to show
 **   the effect of some processing, or be it to indicate an error situation.
 ** 
 ** ## Usage in the default implementation
 ** 
 ** The base class of all [tangible UI elements](\ref Tangible) provides a default implementation
 ** for these generic interaction mechanisms: It offers slots to connect UI signals against, and
 ** it understands the \em `mark` messages `"expand"` and `"reveal"`. These are implemented
 ** by delegating to the \ref Expander and \ref Revealer functors respectively. Moreover, this
 ** default implementation automatically detects a resulting state change and emits an appropriate
 ** \em `note` message on the UI-Bus, so to make those state changes persistent. However, in order
 ** to supply an concrete implementation, depending on the circumstances, either the widget itself
 ** or a parent container holding the element needs to install lambdas into those functor holders,
 ** to detail what actually needs to be done. E.g. it is quite common to implement the "expand"
 ** functionality by wrapping the widget into a `Gtk::Expander`. Which effectively means that
 ** the widget itself is not able to expand itself, since this `Gtk::Expander` container widget
 ** lives within the parent widget to hold the element. So this parent needs to install a
 ** lambda when establishing the child element, and bind it to the `Gtk::Expander::set_expanded`
 ** property, and the corresponding `Gtk::Expander::get_expanded` to retrieve the current
 ** _expansion state_. However, to the contrary, some widgets do implement their _expansion state_
 ** all by themselves, e.g. by switching the presentation layout. Those widgets will have to
 ** install an appropriate Expander functor within their constructor.
 ** 
 ** @see [UI-Element protocol](\ref tangible.hpp)
 ** @see [simple test case](\ref AbstractTangible_test::revealer())
 ** @see [usage example](\ref error-log-display.hpp)
 ** 
 */


#ifndef GUI_MODEL_EXPANDER_REVEALER_H
#define GUI_MODEL_EXPANDER_REVEALER_H


#include "lib/error.hpp"

#include <functional>
#include <utility>


namespace gui {
namespace model {

  using std::move;
  
  
  /**
   * Functor component to support the default implementation of expanding/collapsing.
   * It is built based on closures (lambdas) to explicate how to detect the expansion
   * state of the widget in question and how actively to expand or collapse it.
   * A default constructed Expander disables this feature -- which can be detected
   * through the #canExpand() function.
   */
  class Expander
    {
    public:
      using ProbeFun = std::function<bool(void)>;
      using ChangeFun = std::function<void(bool)>;
      
    private:
      ProbeFun probeState_;
      ChangeFun changeState_;
      
    public:
      Expander() { }
      Expander(ProbeFun detectCurrExpansionState, ChangeFun expand_collapse)
        : probeState_{move (detectCurrExpansionState)}
        , changeState_{move (expand_collapse)}
        {
          ENSURE (canExpand());
        }
      
      bool
      canExpand()  const
        {
          return bool{probeState_}
             and bool{changeState_};
        }
      
      explicit
      operator bool()  const
        {
          REQUIRE (canExpand());
          return probeState_();
        }
      
      bool
      operator() (bool shallExpand)
        {
          REQUIRE (canExpand());
          bool currState = probeState_();
          if (currState != shallExpand)
            changeState_(shallExpand);
          return currState;
        }
      
      /* === alternate "expressive" API === */
      bool
      expand (bool yes =true)
        {
          return this->operator() (yes);
        }
      
      bool
      collapse()
        {
          return expand (false);
        }
      
      bool
      isExpanded()  const
        {
          return bool{*this};
        }
    };
  
  
  
  /**
   * Functor component to support the default implementation of revealing an UI-Element.
   * It is built based on a closure (lambda) detailing what actually to do in order
   * to bring a given UI element into sight. A default constructed Revealer disables
   * this feature, which can be detected through the #canReveal() function.
   */
  class Revealer
    {
    public:
      using RevealeItFun = std::function<void()>;
      
    private:
      RevealeItFun revealIt_;
      
    public:
      Revealer() { }
      Revealer(RevealeItFun how_to_uncover_the_element)
        : revealIt_{move (how_to_uncover_the_element)}
        {
          ENSURE (canReveal());
        }
      
      bool
      canReveal()  const
        {
          return bool{revealIt_};
        }
      
      void
      operator()()
        {
          REQUIRE (canReveal());
          revealIt_();
        }
    };
  
  
  
}} // namespace gui::model
#endif /*GUI_MODEL_EXPANDER_REVEALER_H*/
