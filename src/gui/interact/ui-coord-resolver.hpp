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
 ** # Abstraction
 ** 
 ** The abstraction used to found this interface is twofold. For one, we rely on the notion of logical,
 ** topological [Coordinates in User Interface space](\ref UICoord). And secondly, we rely on a very
 ** limited form of navigation: we navigate a tree-shaped (abstracted) structure just by
 ** - iteration over siblings, which are children of our previous starting point
 ** - the ability, _on this iterator,_ to expand the "current child" and inject
 **   the next level of child iteration at its place, similar to the `flatMap`
 **   operation known from functional programming.
 ** Together, these two capabilities allow us to build exploring and backtracking evaluations,
 ** which is enough to build a secondary helper component on top, the gui::interact::UICoordResolver
 ** 
 ** 
 ** # UI coordinate path evaluation
 ** 
 ** Such a _resolver_ can be used to relate and match a given, incomplete UI coordinate specification
 ** (a "pattern") against the actual UI topology. Evaluation is accomplished by first constituting an anchoring,
 ** followed by traversal of the coordinate spec and matching against a navigation path within the actual UI window
 ** configuration. This process might involve interpretation of some meta-symbols and interpolation of wildcards.
 ** 
 ** As indicated above, the coordinate resolver internally relies on a [context query interface](\ref LocationQuery),
 ** to find out about existing windows, panels, views and tabs and to navigate the real UI structure. The actual
 ** implementation of this context query interface is backed by the \ref Navigator component exposed through the
 ** \ref InteractionDirector.
 ** 
 ** ## Query operations
 ** In addition to the _locally decidable properties_ of a [coordinate spec](\ref UICoord), which are the explicitness
 ** and the presence of some component, several contextual predications may be queried:
 ** 
 ** - *anchorage*
 **   ** the way the given coordinate spec is or can be anchored
 **      *** it is already _explicitly anchored_ by referring either to a specific window or by generic specification
 **      *** it _can be a anchored_ by interpolation of some wildcards
 **      *** it is _incomplete_ and need to be extended to allow anchoring
 **      *** it is _impossible to anchor_ in the current UI configuration
 ** 
 ** - *coverage*
 **   ** the extent to which a given coordinate spec is backed by the actual UI configuration
 **   ** _please note_: to determine the coverage, the spec needs to be anchored, either explicitly,
 **      or by interpolation, or by extension of an incomplete spec
 **      *** it is _completely covered_
 **      *** it is _partially covered_ with an remaining, uncovered extension part
 **      *** it is _possible to cover completely_
 **      *** it is _impossible to cover_ related to the current UI topology
 ** 
 ** \par Some fine points to note
 ** Anchorage and coverage are not the same thing, but coverage implies anchorage. Only when a path is complete
 ** (i.e. it starts with the window spec) and explicit (has no wildcards), then anchorage implies also partial coverage
 ** (namely at least to depth 1). To determine the possibility of coverage means to perform a resolution with backtracking
 ** to pick the maximal solution. Moreover, since "covered" means that the path specification _is at least partially supported
 ** by the real UI,_ we establish an additional constraint to ensure this resolution did not just match some arbitrary wildcards.
 ** Rather we demand that behind rsp. below the last wildcard there is at least one further explicit component in the path spec,
 ** which is supported by the real UI. As a consequence, the coverage resolution may fail altogether, while still providing at
 ** least a possible anchor point.
 ** 
 ** ## Mutations
 ** In addition to querying the interpretation of a given coordinate spec with respect to the current UI environment,
 ** it is also possible to rewrite or extend the spec based on this environment
 ** 
 ** - *anchoring*
 **   ** in correspondence to the possible states of anchorage, we may derive an explicitly anchored spec
 **      *** by interpolating the given spec
 **      *** by interpretation and extension of the given spec
 ** 
 ** - *covering*
 **   ** we may construct the covered part of a given spec, which includes automatic anchoring
 ** 
 ** - *extending*
 **   ** a given UI coordinate pattern is covered...
 **   ** and _truncated_ to the covered part
 **   ** the given _extension suffix_ is then attached behind
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
#include "lib/format-string.hpp"
#include "gui/interact/ui-coord.hpp"
#include "lib/iter-tree-explorer.hpp"
#include "lib/iter-source.hpp"
#include "lib/util.hpp"

#include <utility>
#include <memory>


namespace gui {
namespace interact {
  
  namespace error = lumiera::error;
  
  using std::unique_ptr;
  using util::unConst;
  using lib::Literal;
  using lib::Symbol;
  
  
  
  
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
      
      
      /** build a Lumiera Forward Iterator as front-end and managing Handle for a TreeStructureNavigator
       *  or subclass. The provided pointer is assumed to point to heap allocated storage.
       * @return copyable iterator front-end handle, which allows to retrieve once all values
       *         yielded by this IterSource. The front-end _takes ownership_ of the given object.
       * @note the generated iterator is preconfigured to allow for _"child expansion"_, thereby
       *         calling through the virtual API function expandChildren()
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
   * @see GenNodeLocationQuery a dummy/test implementation, where the "UI topology" is hard wired
   *      as a tree of GenNode elements. This serves the purpose of unit testing, without having
   *      to rely on an actual UI.
   */
  class LocationQuery
    {
    public:
      virtual ~LocationQuery();    ///< this is an interface
      
      using ChildIter = decltype (TreeStructureNavigator::buildIterator(0));


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
       * @remark a depth > 0 also implies that the path can be _anchored._
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
   * This specialised builder uses a _location query facility_ to retrieve information about
   * the current actual UI topology. The UI coordinate spec given as initialisation acts as a
   * _pattern_, to be *queried and resolved* against that actual UI topology in various ways
   *  - determine the _anchor point_, where this UI coordinate pattern will be rooted in the UI
   *  - determine to which extent this UI coordinate pattern can be _"covered"_ (= supported)
   *    by the exiting UI. To resolve this query, it is necessary to perform a matching search
   *    with backtracking, in order to find the best possible coverage
   *    ** the coverage can be _complete_, which means that all components mentioned explicitly
   *       within the pattern in fact exist in the actual UI
   *    ** a _partial coverage_ means that there is a prefix actually supported, while some
   *       extraneous tailing components do not (yet) exist in the UI.
   * Moreover, the pattern can be *mutated to conform* with the existing UI topology:
   *  - the anchor point can be made explicit, which means to replace the _meta specifications_
   *    ** `firstWindow`
   *    ** `currentWindow`
   *  - the calculated _coverage solution_ can be interpolated into the pattern, thereby binding
   *    and replacing any placeholders ("wildcards", i.e. components designated as `"*"`)
   *  - we may _extend_ the pattern by attaching further elements, _behind_ the covered part.
   * @note this is a _coordinate builder,_ which means that it works on a copy of the UI coordinate
   *       pattern provided at initialisation. The result, a possibly reworked UI coordinate spec
   *       can be _moved out_ into a new target UI coordinate (which is then immutable). The
   *       various query and binding operations work by side-effect on the internal state
   *       encapsulated within this builder.
   * @warning computing a coverage solution for a sparsely defined pattern against a large
   *       UI topology can be expensive, since in worst case we have to perform a depth-first
   *       scan of the whole tree structure
   */
  class UICoordResolver
    : public UICoord::Builder
    {

      struct Resolution
        {
          const char* anchor = nullptr;
          size_t depth       = 0;
          unique_ptr<UICoord> covfefe{};
          bool isResolved    = false;
        };
      
      LocationQuery& query_;
      Resolution res_;
      
    public:
      UICoordResolver (UICoord const& uic, LocationQuery& queryAPI)
        : Builder{uic}
        , query_{queryAPI}
        , res_{}
        {
          attempt_trivialResolution();
        }
      
      UICoordResolver (UICoord && uic, LocationQuery& queryAPI)
        : Builder{std::move(uic)}
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
          return res_.anchor
             and res_.anchor != Symbol::BOTTOM;
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
       * @remark this also implies the path is anchored, complete and explicit.
       * @note this predicate tests for _partial_ coverage, which means, there might
       *       be some extraneous suffix in this path descending beyond existing UI
       */
      bool
      isCoveredPartially()  const
        {
          return res_.isResolved
             and res_.depth > 0;
        }
      
      /** this path is completely covered by the currently existing UI structure;
       * @remark there is no extraneous uncovered suffix in this path spec;
       *         moreover, the path is anchored, complete and explicit
       */
      bool
      isCovered()  const
        {
          return res_.isResolved
             and res_.depth == this->uic_.size();
        }
      
      /** synonymous to #isCovered() */
      bool
      isCoveredTotally()  const
        {
          return isCovered();
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
          return isCovered()                          // either explicit coverage known
              or (res_.isResolved and res_.covfefe)   // or previous matching run found (partial) solution
              or unConst(this)->pathResolution()      // perform matching run now to find total coverage
              or (res_.covfefe);                      // or at least partial coverage was found
        }
      
      
      
      /* === mutation functions === */
      
      /** mutate the path to get it totally covered
       *  - make the anchorage explicit
       *  - possibly match and expand any wildcards
       *  - then truncate the UI-Coordinate spec to that part
       *    actually covered by the UI
       * @note if the coordinate spec can not be covered at all,
       *    it will be truncated to zero size
       */
      UICoordResolver&&
      cover()
        {
          if (isCoveredPartially() and not res_.covfefe)
            {
              ASSERT (res_.anchor);    // depth > 0 implies anchorage
              window (res_.anchor);   //  thus make this anchor explicit
              truncateTo (res_.depth);
            }
          else if (canCover())
            {
              ASSERT (res_.isResolved);
              REQUIRE (res_.covfefe);
              res_.depth = res_.covfefe->size();
              this->uic_ = std::move (*res_.covfefe);
              res_.covfefe.reset();
            }
          else
            {
              ASSERT (res_.isResolved);
              REQUIRE (res_.depth == 0);
              REQUIRE (not res_.covfefe);
              truncateTo (0);
            }
          ENSURE (isCovered());
          return std::move (*this);
        }
      
      
      /** mutate the path by resolving all wildcards to achieve partial coverage
       *  - anchorage and all wildcards will be resolved against current UI
       *  - but an extraneous, uncovered, explicit suffix is retained
       * @note if the coordinate spec can not be (partially) covered at all,
       *    it will be truncated to zero size
       */
      UICoordResolver&&
      coverPartially()
        {
          if (isCoveredPartially() and not res_.covfefe)
            {
              ASSERT (res_.anchor);
              window (res_.anchor); // just ensure the anchor info is explicit,
            }                      //  the rest is already in place and explicit
          else if (canCover())
            {
              ASSERT (res_.isResolved);
              REQUIRE (res_.covfefe);
              REQUIRE (uic_.size() >= res_.covfefe->size());
              res_.depth = res_.covfefe->size();
              // possibly overwrite placeholders by explicitly resolved info...
              for (size_t pos = 0; pos < res_.depth; ++pos )
                overwrite (pos, (*res_.covfefe)[pos]);
              res_.covfefe.reset();
            }
          else
            {
              ASSERT (res_.isResolved);
              REQUIRE (res_.depth == 0);
              REQUIRE (not res_.covfefe);
              truncateTo (0);
            }
          ENSURE (empty() or (isCoveredPartially() and uic_.isExplicit()));
          return std::move (*this);                 // no wildcards remain
        }
      
      
      /** mutate the window part of the path such as
       *  to make the anchorage explicit, if possible
       * @remark if the path starts with meta specs like
       *  `firstWindow` or `currentWindow`, they will be
       *  replaced by their current meaning. If the path
       *  is incomplete, but can somehow be resolved, we
       *  use the anchorage as indicated by that resolution,
       *  without interpolating the rest of the path.
       */
      UICoordResolver&&
      anchor()
        {
          if (canAnchor())
            {
              window (res_.anchor);
              normalise();
            }
          return std::move (*this);
        }
      
      
      
      /** mutate the path to extend it while keeping it partially covered
       * @param pathExtension a literal specification, which is extended immediately
       *        behind the actually covered part of the path, irrespective of the depth
       * @note the extension may contain '/', which are treated as component separators
       */
      UICoordResolver&&
      extend (Literal pathExtension)
        {
          if (not isCovered())
            cover();
          ENSURE (isCovered());
          append (pathExtension);
          res_.depth = query_.determineCoverage (this->uic_); // coverage may grow
          return std::move (*this);
        }
      
      /** mutate the path and extend it with components at fixed positions
       * @param partialExtensionSpec UI coordinates used as extension
       * @throw if the extension would overwrite the covered part.
       * @remark this is the typical use case, where we want to place some component
       *         explicitly at a given depth (e.g. a new view or tab)
       */
      UICoordResolver&&
      extend (UICoord const& partialExtensionSpec)
        {
          if (not canCover())
            uic_ = partialExtensionSpec;
          else
            {
              REQUIRE (res_.isResolved);
              size_t coverable = res_.covfefe? res_.covfefe->size() : res_.depth;
              auto newContent = partialExtensionSpec.begin();
              size_t extensionPos = newContent? partialExtensionSpec.indexOf(*newContent) : coverable;
              if (coverable > extensionPos)
                throw error::Invalid (util::_Fmt{"Attempt to extend covered path %s with %s "
                                                 "would overwrite positions %d to %d (incl)"}
                                                % (res_.covfefe? *res_.covfefe : UICoord{uic_.rebuild().truncateTo(res_.depth)})
                                                % partialExtensionSpec
                                                % extensionPos
                                                % (coverable-1));
              cover();
              for ( ; newContent; ++newContent, ++extensionPos )
                overwrite (extensionPos, *newContent);
              normalise();
            }
          res_ = Resolution{};      // start over with pristine resolution state
          attempt_trivialResolution();
          canCover();
          return std::move (*this);
        }
      
      
      
      /** diagnostics */
      operator string()   const { return string(this->uic_); }
      size_t coverDepth() const { return res_.depth; }
      
      
      
    private:
      /** establish a trivial anchorage and coverage, if possible.
       * @note when the UICoord contains wildcards or is incomplete,
       *       a full resolution with backtracking is necessary to
       *       determine anchorage and coverage
       */
      void
      attempt_trivialResolution()
        {
          res_.anchor = query_.determineAnchor  (this->uic_);
          if (not uic_.isExplicit()) return;
          res_.depth  = query_.determineCoverage(this->uic_);
          res_.isResolved = true;
        }

      /** @internal algorithm to resolve this UICoord path against the actual UI topology.
       *  @return true if total coverage is possible (by interpolating wildcards)
       *  @remark after invoking this function, res_.isResolved and possible coverage are set.
       */
      bool pathResolution();
    };
  
  
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_UI_COORD_RESOLVER_H*/
