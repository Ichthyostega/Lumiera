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
#include "lib/iter-tree-explorer.hpp"
#include "lib/iter-source.hpp"
#include "lib/util.hpp"

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
  using util::unConst;
//  using util::isnil;
//  using util::min;
  

  
  
  /**
   * Interface to locate and move within a tree shaped structure.
   * The actual nature of this structure is to be kept abstracted through this interface.
   * The purpose of this construct is to build evaluations and matching operations on top.
   */
  class TreeStructureNavigator
    : public lib::IterSource<Literal>
    {
    public:
      virtual ~TreeStructureNavigator();     ///< this is an interface
      
      /** expand into exploration of child elements at "current position".
       * At any point, a TreeStructureNavicator instance indicates and represents a position
       * within a tree-like structure. At the same time, it is part of a sequence of siblings,
       * which is accessible through iteration. This operation now allows to extend visitation
       * of siblings by consuming the current element and replacing it with the sequence of its
       * immediate child elements, exposing the first one as the _"current position"_.
       * @return pointer to a new heap allocated TreeStructureNavigator implementation, which
       *         represents the sequence of children. The object `this` will not be affected.
       * @note it is the caller's responsibility to own and manage the generated navigator.
       *         The typical (and recommended) way to achieve this is to rely on the embedded
       *         type #iterator, which exposes an appropriately wired iterator::expandChildren()
       */
      virtual TreeStructureNavigator* expandChildren()  const   =0;
      
      
      /** build a Lumiera Forward Iterator as front-end for `*this`.
       * @return copyable iterator front-end handle, which allows to retrieve once all values
       *         yielded by this IterSource. The front-end does _not take ownership_ of this object.
       * @note the generated iterator is preconfigured to allow for _"child expansion"_, thereby
       *         calling through the virtual API function expandChildren()
       */
      auto
      buildIterator ()
        {
          return lib::treeExplore (*this)
                        .expand([](TreeStructureNavigator& parent){ return parent.expandChildren(); });
        }
      
      /** build a Lumiera Forward Iterator as front-end and managing Handle for a heap allocated
       * TreeStructureNavigator or subclass. The provided pointer is assumed to point to heap allocated
       * storage, which will be owned by the generated iterator.
       */
      static auto
      buildIterator (TreeStructureNavigator* source)
        {
          return lib::treeExplore (source)
                        .expand([](TreeStructureNavigator& parent){ return parent.expandChildren(); });
        }
    };
  
  
  
  
  
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
      virtual ~LocationQuery();    ///< this is an interface
      
      using iteratorZ = decltype (TreeStructureNavigator::buildIterator(std::declval<TreeStructureNavigator*>()));
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
      virtual size_t determineCoverage (UICoord const& path)          =0;

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
  
  
  
  
  
  
  /**
   * Query and mutate UICoord specifications in relation to actual UI topology.
   * 
   * @todo initial draft as of 10/2017
   */
  class UICoordResolver
    : public UICoord::Builder
    {

      struct Resolution
        {
          const char* anchor = nullptr;
          size_t depth       = 0;
          std::unique_ptr<UICoord> covfefe;
          bool isResolved    = false;
        };
      
      LocationQuery& query_;
      Resolution res_;
      
      struct ResolutionState;
      
    public:
      UICoordResolver (UICoord const& uic, LocationQuery& queryAPI)
        : Builder{uic}
        , query_{queryAPI}
        , res_{}
        {
          attempt_trivialResolution();
        }
      
      
      /* === query functions === */
      
      /** is this path explicitly anchored at an existing window?
       * @remarks this also implies the path is complete and explicit (no wildcards).
       */
      bool
      isAnchored()  const
        {
          return res_.isResolved
             and res_.anchor and res_.anchor != Symbol::BOTTOM;
        }

      /** determine if a mutation is possible to anchor the path explicitly
       * @remarks basically this either means the path isAnchored(), or we're able
       *          to calculate a path resolution, interpolating any wildcards.
       *          And while the path resolution as such might fail, it was at least
       *          successful to determine an anchor point. The existence of such an
       *          anchor point implies the path is not totally in contradiction to the
       *          existing UI */
      bool
      canAnchor()  const
        {
          return isAnchored()
              or (res_.isResolved and res_.covfefe)
              or unConst(this)->pathResolution()
              or isAnchored(); // resolution failed, but computed at least an anchor
        }

      /** is this path at least _partially_ covered?
       *  A covered path describes an access path through widgets actually existing in the UI.
       * @remarks this also implies the path is anchored, complete and explicit.
       * @note this predicate tests for _partial_ coverage, which means, there might
       *       be some extraneous suffix in this path descending beyond existing UI
       */
      bool
      isCovered()  const
        {
          return res_.isResolved
             and res_.depth > 0;
        }
      
      /** there is no extraneous uncovered suffix in this path spec */
      bool
      isTotallyCovered()  const
        {
          return res_.isResolved
             and res_.depth == this->uic_.size();
        }

      /** determine if a mutation is possible to get the path (partially) covered.
       * @remarks in order to be successful, a path resolution must interpolate any gaps in the
       *       path spec _and_ reach a point behind / below the gap (wildcards), where an existing
       *       explicitly stated component in the path can be confirmed (covered) by the existing UI.
       *       The idea behind this definition is that we do not want just some interpolated wildcards,
       *       rather we really want to _confirm_ the essence of the path specification. Yet we accept
       *       an extraneous suffix _in the explicitly given part_ of the path spec to extend beyond or
       *       below what exists currently within the UI.
       */
      bool
      canCover()  const
        {
          return isCovered()
              or (res_.isResolved and res_.covfefe)
              or unConst(this)->pathResolution();
        }
      
      
      
      /* === mutation functions === */
      
      /** mutate the path to get it totally covered
       */
      UICoordResolver
      cover()
        {
          if (isCovered())
            truncateTo (res_.depth);
          else if (canCover())
            {
              ASSERT (res_.isResolved);
              REQUIRE (res_.covfefe);
              res_.depth = res_.covfefe->size();
              std::swap (this->uic_, *res_.covfefe);
              res_.covfefe.reset();
              ENSURE (isTotallyCovered());
            }
          else
            {
              ASSERT (res_.isResolved);
              REQUIRE (res_.depth == 0);
              REQUIRE (not res_.covfefe);
              truncateTo (0);
            }
          return std::move (*this);
        }
      
      
      /** mutate the path to extend it while keeping it partially covered
       */
      UICoordResolver
      extend (Literal pathExtension)
        {
          if (not isTotallyCovered())
            cover();
          ENSURE (isTotallyCovered());
          append (pathExtension);
          res_.depth = query_.determineCoverage (this->uic_); // coverage may grow
          ENSURE (isCovered());
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

      /** @internal algorithm to resolve this UICoord path against the actual UI topology. */
      bool pathResolution();
    };
  
  
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_UI_COORD_RESOLVER_H*/
