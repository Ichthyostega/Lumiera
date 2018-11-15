/*
  GEN-NODE-LOCATION-QUERY.hpp  -  pose UI-coordinate location queries against a GenNode structure

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


/** @file gen-node-location-query.hpp
 ** Implementation of the gui::interact::LocationQuery interface to work on a GenNode tree.
 ** The interface allows to pose queries against a concrete structure to verify and reshape some
 ** [UI Coordinate specification](\ref UICoord); basically it offers methods to navigate within a
 ** tree-like structure. While in the actual implementation, such a query interface would be backed
 ** by navigating real UI structures, the implementation given here instead uses a generic tree structure
 ** given as `Record<GenNode>`.
 ** 
 ** # Representing UI structure as GenNode tree
 **
 ** While basically the interface LocationQuery abstracts and reduces the structure of an UI into
 ** just some hierarchically arranged and nested IDs, we should note some specific twists how a GenNode
 ** tree is used here to represent the structure elements as defined through [UI coordinates](\ref UICoord):
 ** - we use the special _type_ attribute to represent the _perspective_ within each window;
 **   deliberately, we'll use this twisted structure here to highlight the fact that the
 **   backing structure need not be homogeneous; rather, it may require explicit branching
 ** - we use the _attributes_ within the GenNode "object" representation, since these are _named_
 **   nested elements, and the whole notion of an UI coordinate path is based on named child components
 ** - relying upon the [object builder notation](\ref Record::Mutator), it is possible to define a whole
 **   structure as nested inline tree; named nested elements can be added with the `set(key, val)`
 **   builder function, and for each nested scope, we start a new nested builder with `MakeRec()`.
 ** - since GenNodeLocationQuery is conceived for writing test and verification code, there is a
 **   special convention to set the `currentWindow` to be the last one in list -- in a real UI
 **   this would not of course not be a configurable property of the LocationQuery, and rather
 **   just reflect the transient window state and return the currently activated window
 **
 ** @todo WIP 10/2017 started in the effort of shaping the LoactionQuery interface, and used
 **       to support writing unit tests, to verify the UiCoordResolver. It remains to be seen
 **       if this implementation can be used beyond this limited purpose
 ** 
 ** @see UICoordResolver_test
 ** @see IterTreeExplorer_test
 ** @see ui-coord-resolver.hpp
 ** @see navigator.hpp
 */


#ifndef STAGE_INTERACT_GEN_NODE_LOCATION_QUERY_H
#define STAGE_INTERACT_GEN_NODE_LOCATION_QUERY_H

#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "stage/interact/ui-coord-resolver.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/format-string.hpp"
#include "lib/iter-tree-explorer.hpp"
#include "lib/iter-source.hpp"
#include "lib/itertools.hpp"
#include "lib/util.hpp"

#include <utility>
#include <string>


namespace stage {
namespace interact {
  
  namespace error = lumiera::error;
  
  using lib::Symbol;
  using lib::Literal;
  using lib::diff::Rec;
  using util::isnil;
  using util::_Fmt;
  using std::forward;
  using std::string;
  

  
  
  /**
   * Test/Diagnostics: implementation of the LocationQuery-API
   * based on a abstract topological structure given as Record<GenNode> ("GenNode tree").
   * @remark intended for verifying path resolution and navigation through unit tests
   */
  class GenNodeLocationQuery
    : public LocationQuery
    {
      Rec tree_;
      
    public:
      template<class REC>
      GenNodeLocationQuery(REC&& backingStructure)
        : tree_(std::forward<REC>(backingStructure))
        { }
      
      
      /* === LocationQuery interface === */
      
      /** resolve Anchor against GenNode tree */
      virtual Literal
      determineAnchor (UICoord const& path)  override
        {
          if (isnil(tree_) or not path.isPresent(UIC_WINDOW))
            return Symbol::BOTTOM;
          if (UIC_FIRST_WINDOW == path.getWindow())
            return getFirstWindow();
          if (UIC_CURRENT_WINDOW == path.getWindow())
            return getCurrentWindow();
          if (not tree_.hasAttribute(string{path.getWindow()}))         /////////////////////////////////////TICKET #1113 : unnecessary repackaging of a Literal into string when GenNode ID is based on Literal
            return Symbol::BOTTOM;
          return path.getWindow();
        }


      /** evaluate to what extent a UIcoord spec matches the structure given as GenNode tree */
      virtual size_t
      determineCoverage (UICoord const& path)  override
        {
          size_t depth = 0;
          drillDown (tree_, path, path.size(), depth);
          return depth;
        }

      /** get the sequence of child IDs at a designated position in the backing GenNode tree */
      virtual ChildIter
      getChildren (UICoord const& path, size_t pos)  override
        {
          size_t depth = 0;
          Rec const& node = drillDown (tree_, path, pos, depth);
          if (depth != pos)
            throw error::State(_Fmt{"unable to drill down to depth %d: "
                                    "element %s at pos %d in path %s is in "
                                    "contradiction to actual UI structure"}
                                   % pos
                                   % (depth<path.size()? path[depth] : Symbol::BOTTOM)
                                   % depth
                                   % path
                              );
          return TreeStructureNavigator::buildIterator(
                   childNavigator (node, depth));
        }
      
    private:
      Literal
      getFirstWindow()
        {
          return Symbol{*tree_.keys()};                        //////////////////////////////////////////////TICKET #1113 : warning use of Symbol table becomes obsolete when EntryID relies on Literal
        }
      Literal
      getCurrentWindow()
        {
          return Symbol{lib::pull_last (tree_.keys())};        //////////////////////////////////////////////TICKET #1113 : warning use of Symbol table becomes obsolete when EntryID relies on Literal
        }            // special convention for unit-tests
      
      Literal
      resolveElm (UICoord const& path, size_t depth)
        {
          REQUIRE (path.isPresent(depth));
          return depth==UIC_WINDOW? GenNodeLocationQuery::determineAnchor(path)
                                  : path[depth];
        }
      
      
      Rec const&
      drillDown (Rec const& tree, UICoord const& path, size_t maxDepth, size_t& depth)
        {
          if (depth<maxDepth and path.isPresent(depth))
            {
              const char* pathElm = resolveElm (path, depth);
              if (hasNode (tree, pathElm, depth))
                {
                  ++depth;
                  return drillDown (descendInto(tree,depth-1,pathElm), path, maxDepth, depth);
                }
            }
          return tree;
        }
      
      /** does the guiding tree contain the element as requested by the UICoord path?
       * @remark this function abstracts a special asymmetry of the tree representation:
       *  at `level==UIC_PERSP` (the second level), the perspective info is packed into
       *  the type meta attribute. This was done on purpose, to verify our design is able
       *  to handle such implementation intricacies, which we expect to encounter
       *  when navigating the widgets of a real-world UI toolkit set
       */
      static bool
      hasNode (Rec const& tree, const char* pathElm, size_t depth)
        {
          return depth==UIC_PERSP? pathElm == tree.getType()
                                 : tree.hasAttribute(pathElm);
        }
      
      /** within `tree` _at level_ `depth` descend into the child element designated by `pathElm` */
      static Rec const&
      descendInto (Rec const& tree, size_t depth, const char* pathElm)
        {
          return depth==UIC_PERSP? tree // perspective info is attached as type at the parent node
                                 : tree.get(pathElm).data.get<Rec>();
        }
      
      
      
      
      /* ==== iterate over siblings with the ability to expand one node's children ==== */
      
      /** @return a heap allocated object attached at "current tree position" while
       *          exposing the names of all child nodes [through iteration](\ref lib::IterSource)
       */
      static TreeStructureNavigator*
      childNavigator (Rec const& node, size_t depth)
        {
                                   //////////////////////////////////////////////////////////////////////////TICKET #1113 : capturing the string into the global Symbol table becomes obsolete, once GenNode exposes Literal as ID
          auto internedString = [](string const& id) -> Literal
                                  {
                                    return Symbol{id};
                                  };
          return depth==UIC_PERSP? buildNavigator (node, depth, singleValIterator (internedString (node.getType())))
                                 : buildNavigator (node, depth, transformIterator (node.keys(), internedString));
        }
      
      
      /**
       * Helper to navigate a tree topology represented as GenNode tree.
       * Basically this is a lib::IterSource<Literal> to encapsulate a sequence of sibling nodes.
       * A "current element" representation is layered on top to allow to expand one level deeper
       * on demand. This "child expansion" is triggered by invoking the `expandChildren()` function
       * on the iterator front-end provided as LocationQuery::ChildIter
       */
      template<class PAR>
      class GenNodeNavigator
        : public PAR
        {
          Rec const& pos_;
          size_t depth_;
          
          virtual TreeStructureNavigator*
          expandChildren()  const override
            {
              return childNavigator (descendInto (pos_,depth_, currentChild_), depth_+1);
            }
          
          ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1125 : work around the misaligned IterSource design
          // The design of IterSource attempts to be too clever, and we have to pay for it now...
          // If IterSource would just work like a StateCore and expose the "current element" via API call,
          // then we'd be able to retrieve the name of the current child node. Unfortunately it doesn't
          // and thus we rig a "wire tap" here and capture the node names whenever an iteration happens.
          // 
          Literal currentChild_ = Symbol::BOTTOM;
          
          using Pos = typename PAR::Pos;
          
          virtual Pos
          firstResult ()  override
            {
              Pos pos = PAR::firstResult();
              if (pos) currentChild_ = *pos;
              return pos;
            }
          
          virtual void
          nextResult(Pos& pos)  override
            {
              PAR::nextResult (pos);
              if (pos) currentChild_ = *pos;
            }
          ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1125 : work around the misaligned IterSource design

          
        public:
          template<class IT>
          GenNodeNavigator(Rec const& node, size_t depth, IT&& rawChildIter)
            : PAR{forward<IT> (rawChildIter)}
            , pos_{node}
            , depth_{depth}
            { }
        };
      
      
      /** type rebinding helper to pick up the concrete child iterator type `IT` */
      template<class IT>
      static TreeStructureNavigator*
      buildNavigator (Rec const& node, size_t depth, IT && rawIterator)
        {
          return new GenNodeNavigator<
                    lib::WrappedLumieraIter<IT,
                      TreeStructureNavigator>> {node, depth, forward<IT> (rawIterator)};
        }
    };
  
  
  
}}// namespace stage::interact
#endif /*STAGE_INTERACT_GEN_NODE_LOCATION_QUERY_H*/
