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
 **   structure as nested inline tree; this leads to a somewhat confusing notation, where the names of
 **   the child nodes are spelled of at the closing bracket of each construct.
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
 ** @see ui-coord-resolver.hpp
 ** @see navigator.hpp
 */


#ifndef GUI_INTERACT_GEN_NODE_LOCATION_QUERY_H
#define GUI_INTERACT_GEN_NODE_LOCATION_QUERY_H

#include "lib/error.hpp"
//#include "lib/symbol.hpp"
#include "gui/interact/ui-coord-resolver.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/format-string.hpp"
#include "lib/itertools.hpp"
#include "lib/util.hpp"

//#include <boost/noncopyable.hpp>
//#include <string>
//#include <vector>
//#include <utility>
//#include <memory>


namespace gui {
namespace interact {
  
  namespace error = lumiera::error;
  
//  using std::unique_ptr;
//  using std::string;
//  using lib::Literal;
//  using lib::Symbol;
  using lib::diff::Rec;
  using util::_Fmt;
//  using util::unConst;
//  using util::isnil;
//  using util::min;
  

  
  
  /**
   * Test/Diagnostics: implementation of the LocationQuery-API
   * based on a abstract topological structure given as Record<GenNode> ("GenNode tree").
   * @remark intended for verifying path resolution and navigation through unit tests
   * @todo initial draft as of 10/2017
   */
  class GenNodeLocationQuery
    : public LocationQuery
    {
      Rec const& tree_;
      
    public:
      GenNodeLocationQuery(Rec const& backingStructure)
        : tree_(backingStructure)
        { }
      
      
      /* === LocationQuery interface === */
      
      /** resolve Anchor against GenNode tree */
      virtual Literal
      determineAnchor (UICoord const& path)
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
      determineCoverage (UICoord const& path)
        {
          size_t depth = 0;
          drillDown (tree_, path, path.size(), depth);
          return depth;
        }

      /** get the sequence of child IDs at a designated position in the backing GenNode tree */
      virtual ChildIter
      getChildren (UICoord const& path, size_t pos)
        {
          size_t depth = 0;
          Rec const& node = drillDown (tree_, path, pos, depth);
          if (pos != depth-1)
            throw error::State(_Fmt{"unable to drill down to depth %d: "
                                    "element %s at pos %d in path %s is in "
                                    "contradiction to actual UI structure"}
                                   % pos
                                   % (depth<path.size()? path[depth] : Symbol::BOTTOM)
                                   % depth
                                   % path
                              );
          return childSequence(node);
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
        }
      
      static ChildIter
      childSequence (Rec const& node)
        {
          auto internedString = [](string const& id) -> Literal
                                  {
                                    return Symbol{id};
                                  };
                                              ///////////////////////////////////////////////////////////////TICKET #1113 : could just use lib::wrapIter when GenNode exposes Literal as ID
          return lib::transform (node.keys(), internedString);
        }
      
      static Rec const&
      drillDown (Rec const& tree, UICoord const& path, size_t maxDepth, size_t& depth)
        {
          if (depth<maxDepth and path.isPresent(depth))
            {
              const char* pathElm = path[depth];
              if (tree.hasAttribute(pathElm))
                return drillDown (tree.get(pathElm).data.get<Rec>(), path, maxDepth, ++depth);
            }
          return tree;
        }
    };
  
  
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_GEN_NODE_LOCATION_QUERY_H*/
