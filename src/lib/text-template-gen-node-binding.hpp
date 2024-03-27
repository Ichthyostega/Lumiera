/*
  TEXT-TEMPLATE-GEN-NODE-BINDING.hpp  -  data binding adapter for ETD

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file text-template-gen-node-binding.hpp
 ** A complement to allow instantiation of a TextTemplate with ETD data.
 ** Instead of requiring a specific data source type, the text template engine relies
 ** on an extension point through a data-binding template, which can be (partially) specialised
 ** to implement data access for any suitable kind of structured data. The Lumiera [ETD] is
 ** a recursive data type comprised of lib::diff::GenNode elements, which in turn can hold
 ** a small selection of binary data elements. This scheme for structured data is widely used
 ** for internal communication between the components of the Lumiera application; it offers
 ** all the structural elements to provide a full featured backing data structure for the
 ** text template engine.
 ** 
 ** # Intricacies
 ** 
 ** Since the ETD is based on _binary data,_ we need to invoke a string rendering during
 ** data access, in the end relying on util::toString(). This limits the precision of floating-pont
 ** and time data (assuming this is adequate for the purpose of instantiating placeholders in a
 ** text template). A further challenge arises from the openness of the ETD format, which is
 ** intended for loose coupling between subsystems, and deliberately imposes not much structural
 ** constraints, while also offering only very limited introspection capabilities (to prevent
 ** a »programming by inspection« style). Some further conventions are thus necessary
 ** - a _Scope_ is assumed to be a _Record-Node._ (»object structure«)
 ** - _Keys_ are translated into _Attribute access._
 ** - _Iteration_ is assumed to pick a _loop-control Node_ and descend into this node's child scope.
 ** - if such iterated children _happen to be simple values_, then a pseudo-scope is synthesised,
 **   containing a single virtual attribute with the KeyID "value" (which implies that TextTemplate
 **   can expand a `${value}` placeholder in that scope.
 ** - Attributes of enclosing scopes are also visible — unless shadowed.
 ** [ETD]: https://lumiera.org/documentation/design/architecture/ETD.html
 ** @todo WIP-WIP-WIP 3/2024
 ** @see TextTemplate_test
 */


#ifndef LIB_TEXT_TEMPLATE_GEN_NODE_BINDING_H
#define LIB_TEXT_TEMPLATE_GEN_NODE_BINDING_H


#include "lib/diff/gen-node.hpp"
#include "lib/text-template.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-util.hpp"
//#include "lib/regex.hpp"
//#include "lib/util.hpp"

//#include <memory>
#include <string>
//#include <vector>
//#include <stack>
//#include <map>


namespace lib {
  
  using std::string;
//  using StrView = std::string_view;
//  
//  using util::_Fmt;
//  using util::isnil;
//  using util::unConst;
  
  
  namespace {
    
  }
  
  /* ======= Data binding for GenNode (ETD) ======= */
  
  /**
   * Data-binding for a tree of GenNode data (ETD).
   * Attributes are accessible as keys, while iteration descends
   * into the child scope of the attribute indicated in the ${for <key>}` tag.
   * @see TextTemplate_test::verify_ETD_binding()
   */
  template<>
  struct TextTemplate::DataSource<diff::GenNode>
    {
      using Node = diff::GenNode;
      using Rec  = diff::Rec;
      
      Node const* data_;
      DataSource* parScope_;
      bool isSubScope() { return bool(parScope_); }
      
      DataSource(Node const& root)
        : data_{&root}
        , parScope_{nullptr}
        { }
      
      
      using Value = std::string;
      using Iter = Rec::scopeIter;
      
      Node const*
      findNode (string key)
        {
          if (data_->isNested())
            {// standard case: Attribute lookup
              Rec const& record = data_->data.get<Rec>();
              if (record.hasAttribute (key))
                return & record.get (key);
            }
          else
          if ("value" == key) // special treatment for a »pseudo context«
            return data_;    //  comprised only of a single value node
          else
          if (isSubScope())
            return parScope_->findNode (key);
          
          return nullptr;
        }
      
      bool
      contains (string key)
        {
          return bool(findNode (key));
        }
      
      Value
      retrieveContent (string key)
        {
          return Value(findNode(key)->data);  ///////////////////////////////OOO this is not correct -- need a way to render the bare content
        }
      
      Iter
      getSequence (string key)
        {
          if (not contains(key))
            return Iter{};
          Node const* node = findNode (key);
          if (not node->isNested())
            return Iter{};
          else
            return node->data.get<Rec>().scope();
        }
      
      DataSource
      openContext (Iter& iter)
        {
          REQUIRE (iter);
          DataSource nested{*this};
          nested.parScope_ = this;
          nested.data_ = & *iter;
          return nested;
        }
    };
  
  
  namespace {// help the compiler with picking the proper specialisation for the data binding
    
    inline auto
    bindDataSource(diff::GenNode const& etd)
      {
        return TextTemplate::DataSource<diff::GenNode>{etd};
      }
  }
  
  
  
}// namespace lib
#endif /*LIB_TEXT_TEMPLATE_GEN_NODE_BINDING_H*/
