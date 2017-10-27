/*
  UI-COORD-RESOLVER.hpp  -  resolve UI coordinate spec against actual window topology

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


/** @file ui-coord-resolver.hpp
 ** Evaluation of UI coordinates against a concrete window topology.
 ** [UI-Coordinates](\ref UICoord) allow to describe and locate an interface component within the
 ** Lumiera GUI through a topological access path. As such these coordinate specifications are abstract,
 ** and need to be related, attached or resolved against the actual configuration of widgets in the UI.
 ** Through this relation it becomes possible to pose meaningful queries over these coordinates, or to
 ** build, rebuild and remould a coordinate specification.
 ** 
 ** We need to avoid tainting with the intrinsics of the actual UI toolkit though -- which indicates
 ** the UICoordResolver should be designed as an abstract intermediary, built on top of a command and
 ** query interface, provided by the \ref Navigator and backed by the actual UI configuration.
 ** 
 ** @todo WIP 10/2017 first draft ////////////////////////////////////////////////////////////////////////////TICKET #1106 generic UI coordinate system
 ** 
 ** @see UICoordResolver_test
 ** @see UICoord_test
 ** @see navigator.hpp
 ** @see view-locator.hpp
 */


#ifndef GUI_INTERACT_UI_COORD_RESOLVER_H
#define GUI_INTERACT_UI_COORD_RESOLVER_H

#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "gui/interact/ui-coord.hpp"
#include "lib/iter-source.hpp"
#include "lib/iter-stack.hpp"
//#include "lib/util.hpp"

//#include <boost/noncopyable.hpp>
//#include <string>
//#include <vector>
#include <utility>
#include <memory>


namespace gui {
namespace interact {
  
  namespace error = lumiera::error;
  
//  using std::unique_ptr;
//  using std::string;
  using lib::Literal;
//  using lib::Symbol;
//  using util::unConst;
//  using util::isnil;
//  using util::min;
  

  
  
  /**
   * Interface to discover a backing structure for the purpose of path navigation and resolution.
   * UICoord are meant to designate a position within the logical structure of an UI -- yet in fact
   * they may be resolved against any tree-like topological structure, which can be queried through
   * this interface.
   * @see Navigator the implementation used in the Lumiera UI, as backed by actual GUI components
   */
  class LocationQuery
    {
    public:
      virtual ~LocationQuery() { }  ///< this is an interface
      
      using ChildIter = lib::IterSource<Literal>::iterator;


      /** make the real anchor point explicit.
       * @param path an explicit UICoord spec to be anchored in the actual UI
       * @return _explicit_ literal window name, where the path can be anchored
       *         Symbol::BOTTOM in case the given path can not be anchored currently.
       * @remark here "to anchor" means to match and thus "attach" the starting point
       *         of the UIcoord path, i.e. the window spec, with an actual top-level
       *         window existing in the current UI configuration and state. This operation
       *         either confirms existence of a window given by explicit ID, or it supplies
       *         the current meaning of the meta specs `currentWindow` and `firstWindow`,
       *         again in the form of an explicit window name
       */
      virtual Literal determineAnchor (UICoord const& path)           =0;


      /** evaluate to what extent a UIcoord spec matches the actual UI
       * @return the depth to which the given spec is _"covered"_ by the actual UI.
       *         Can be zero, in which case the given coordinates can not be resolved
       *         and addressed within the currently existing windows, panes and views.
       * @note this operation does not perform any _resolution_ or interpolation of wildcards,
       *         it just matches explicit UI component names.
       * @see UICoordResolver for a facility to perform such a resolution and to navigate paths.
       */
      virtual size_t  determineCoverage (UICoord const& path)         =0;

      /** get the sequence of child components at a designated position in the actual UI
       * @param path an explicit UIcoord spec, expected to be anchored and at least partially
       *         covered within the current configuration and state of the UI
       * @param pos depth where the given path shall be evaluated, starting with 0 at window level
       * @return an iterator to enumerate all child components actually existing in the current
       *         UI below the location designated by path and pos.
       * @remark the path is only evaluated up to (not including) the given depth. Especially
       *         when `pos == 0`, then the path is not evaluated and matched at all, rather
       *         just the current list of top-level windows is returned.
       * @throw  error::State when navigating the given path touches a non-existing element
       */
      virtual ChildIter getChildren (UICoord const& path, size_t pos) =0;
    };
  
  
  
  
  namespace path { ///< @internal implementation details of UICoord resolution against actual UI

    struct Resolution
      {
        const char* anchor = nullptr;
        size_t depth       = 0;
        std::unique_ptr<UICoord> covfefe;
        bool isResolved    = false;
      };

    struct ResolutionState
      {
        using ChildIter = LocationQuery::ChildIter;

        lib::IterStack<ChildIter> backlog;
        lib::IterQueue<Resolution> solutions;
      };

  }//(End) implementation details



  /**
   * Query and mutate UICoord specifications in relation to actual UI topology.
   * 
   * @todo initial draft as of 10/2017
   */
  class UICoordResolver
    : public UICoord::Builder
    {
      LocationQuery& query_;
      path::Resolution res_;
      
    public:
      UICoordResolver (UICoord const& uic, LocationQuery& queryAPI)
        : Builder{uic}
        , query_{queryAPI}
        , res_{}
        {
          attempt_trivialResolution();
        }
      
      
      /* === query functions === */
      
      /** */
      bool
      isAnchored()  const
        {
          UNIMPLEMENTED ("path anchorage check");
        }

      /** */
      bool
      canAnchor()  const
        {
          UNIMPLEMENTED ("determine if a mutation is possible to anchor the path explicitly");
        }

      /** */
      bool
      isCovered()  const
        {
          UNIMPLEMENTED ("path coverage check");
        }
      
      /** */
      bool
      canCover()  const
        {
          UNIMPLEMENTED ("determine if a mutation is possible to get the path covered");
        }
      
      
      
      /* === mutation functions === */
      
      /**
       */
      UICoordResolver
      cover()
        {
          UNIMPLEMENTED ("mutate the path to get it covered");
          return std::move (*this);
        }
      
      
      /**
       */
      UICoordResolver
      extend (Literal pathExtension)
        {
          UNIMPLEMENTED ("mutate the path to extend it while keeping it partially covered");
          return std::move (*this);
        }
      
      
    private:
      /** establish a trivial anchorage and coverage, if possible.
       * @note when the UICoord contains wildcards or is incomplete,
       *       a full resolution with backtracking is necessary to
       *       determine anchorage and coverage
       */
      void
      attempt_trivialResolution()
        {
          if (not uic_.isExplicit()) return;
          res_.anchor = query_.determineAnchor  (this->uic_);
          res_.depth  = query_.determineCoverage(this->uic_);
          res_.isResolved = true;
        }

      /** @internal algorithm to resolve this UICoord path against the actual UI topology */
      bool pathResolution();
    };
  
  
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_UI_COORD_RESOLVER_H*/
