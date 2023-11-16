/*
  DOT-GEN.hpp  -  DSL to generate Graphviz-DOT code

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file dot-gen.hpp
 ** Support for generation of Graphviz-DOT code for structure visualisation.
 ** The [dot language] offers a simple notation to represent structural information
 ** as diagrams of abstract graphs and networks. The *Graphviz* layout programs
 ** translate these into automatically arranged diagrams graphs, relying on basic
 ** layout schemes like directed and undirected graphs, force-directed placement,
 ** radial arrangements, clustered graphs or squarified treemap layout. These
 ** visualisations can be rendered as images, vector graphic SVG or PDF and
 ** a lot of further formats.
 ** 
 ** The namespace lib::dot_gen contains a set of integrated builder DSL functions
 ** to simplify the task of syntax generation; notable it is possible to set up
 ** several [sections](\ref lib::dot_gen::Section), which can then be gradually
 ** populated with definition clauses while traversing a data structure.
 ** 
 ** @todo 11/2023 this is an initial draft, shaped by the immediate need to visualise
 **       [random generated](\ref vault::gear::test::TestChainLoad) computation
 **       patterns for Scheduler [load testing](\ref SchedulerStress_test).
 **       The abstraction level of this DSL is low and structures closely match
 **       some clauses of the DOT language; this approach may not yet be adequate
 **       to generate more complex graph structures and was extracted as a starting
 **       point for further refinements.
 ** 
 ** ## Usage
 ** The top-level entrance point is lib::dot_get::digraph(), allowing to combine a
 ** series of lib::dot_gen::Section definitions into a DOT script, which can then
 ** be retrieved by string conversion (or sent to standard output)
 ** - Section is an accumulator of lines with DOT language specs
 ** - Code is a string with syntax, used as base for some pre-configured terms
 ** - Node defines a variable name, but can be augmented with attributes to
 **   build a _node-statement_
 ** - Scope is meant as a device to group several nodes together, typically to
 **   form a cluster or stratum in the generated layout
 ** 
 ** @see TestChainLoad_test
 ** @see SchedulerStress_test
 ** [dot language]: https://graphviz.org/doc/info/lang.html
 */


#ifndef LIB_DOT_GEN_H
#define LIB_DOT_GEN_H


#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include <utility>
#include <sstream>
#include <string>
#include <vector>


namespace lib {
namespace dot_gen { ///< Collection of builder DSL functions to generate Graphviz-DOT code
  
  using util::toString;
  using util::isnil;
  using std::string;
  using std::move;
  
  
  /** markup to generate a piece of code */
  struct Code : string
    {
      using string::string;
      Code(string const& c) : string{c}      { }
      Code(string     && c) : string{move(c)}{ }
    };
  
  
  /** Accumulator to collect lines of DOT code */
  struct Section
    {
      std::vector<string> lines;
      
      Section (string name)
        : lines{"// "+name}
        { }
      
      Section&&
      operator+= (Code const& code)
        {
          lines.emplace_back(code);
          return move(*this);
        }
    };
  
  
  /** Helper to collect DOT-Graphviz code for output */
  class DotOut
    {
      std::ostringstream buff_;
      
      static uint const IDENT_STEP = 2;
    public:
      void
      putLine (string line, uint indent=0)
        {
          if (indent)
            buff_ << string(indent,' ');
          buff_   << line
                  << '\n';
        }
      
      void
      put (Code const& code)
        {
          buff_ << code;
        }
      
      void
      put (Section const& sect)
        {
          for (string const& line : sect.lines)
            putLine (line, IDENT_STEP);
        }
      
      template<class P, class...PS>
      void
      put (P const& part, PS const& ...parts)
        {
          put (part);
          putLine ("");
          put (parts...);
        }
      
      /** retrieve complete code generated thus far */
      operator string()  const
        {
          return buff_.str();
        }
    };
  
  
  /** generate a Node name or a node_statement
   *  defining attributes of that node. All variables
   *  use the format `N<number>`. */
  struct Node : Code
    {
      Node (size_t id)
        : Code{"N"+toString(id)}
        { }
      
      Node&&
      addAttrib (string def)
        {
          if (back() != ']')
            append ("[");
          else
            {
              resize (length()-2);
              append (", ");
            }
          append (def+" ]");
          return move(*this);
        }
      
      Node&&
      label (size_t i)
        {
          return addAttrib ("label="+toString(i));
        }
      
      Node&&
      style (Code const& code)
        {
          if (not isnil(code))
            addAttrib (code);
          return move(*this);
        }
    };
  
  
  /** accumulator to collect nodes grouped into a scope */
  struct Scope : Code
    {
      Scope (size_t id)
        : Code{"{ /*"+toString(id)+"*/ }"}
        { }
      
      Scope&&
      add (Code const& code)
        {
          resize(length()-1);
          append (code+" }");
          return move(*this);
        }
      
      Scope&&
      rank (string rankSetting)
        {
          return add(Code{"rank="+rankSetting});
        }
    };
  
  /** generate a directed node connectivity clause */
  inline Code
  connect (size_t src, size_t dest)
  {
    return Code{Node(src) +" -> "+ Node(dest)};
  }
  
  
  /**
   * Entrance-point: generate a graph spec in DOT-Language.
   * @param parts a sequence of Section or Code objects to be combined and rendered
   * @return DotOut object holding the script rendered into a stringstream-buffer
   */
  template<class...COD>
  inline DotOut
  digraph (COD ...parts)
  {
    DotOut script;
    script.putLine (Code{"digraph {"});
    script.put (parts...);
    script.putLine (Code{"}"});
    return script;
  }
  
  
}} // namespace lib::dot_gen
#endif /*LIB_DOT_GEN_H*/
