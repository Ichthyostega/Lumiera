/*
  GEN-NODE.hpp  -  generic node element for tree like data representation

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file gen-node.hpp
 ** Generic building block for tree shaped (meta)data structures.
 ** A representation built from GenNode elements is intended to support
 ** (limited) introspection of data structures and exchange of mutations
 ** in the form of \link diff-language.hpp diff messages. \endlink
 ** 
 ** Despite of the name, GenNode is \em not meant to be an universal
 ** data representation; rather it is limited to embody a fixed hard
 ** wired set of data types, able to stand-in for attributes
 ** and sub scope contents of the lumiera high-level data model.
 ** 
 ** \par Anatomy of a GenNode
 ** 
 ** GenNode is a polymorphic value with well defined identity and type.
 ** Each element is conceived to be »unique within context« -- as defined
 ** by the immediately visible scope within a tree like structure.
 ** Beyond this identity metadata, each GenNode carries a DataCap, which
 ** is an inline container and attachment point for payload data. Simple
 ** attribute values can be carried alongside, while more complex types
 ** or entities bound to a reference and registration system (e.g. Placement)
 ** will be referred by a suitable reference representation (PlacementID).
 ** The DataCap is what creates the polymorphic nature, where the common
 ** interface is mostly limited to managemental tasks (copying of values,
 ** external representation).
 ** 
 ** To represent object-like structures and for building trees, a special
 ** kind of data type is placed into the DataCap. This type, Record<GenNode>
 ** is recursive and has the ability to hold both a a set of attributes
 ** addressable by-name and an (ordered) collection of elements treated
 ** as children within the scope of the given record.
 ** 
 ** \par Requirements
 ** 
 ** GenNode elements are to be used in the diff detection and implementation.
 ** This implies some requirements for the (opaque) elements used in diff:
 ** - they need to support the notion of equality
 ** - we need to derive a key type for usage in index tables
 **   - this implies the necessity to support std::less comparisons for tree-maps
 **   - and the necessity to support hash code generation for unordered (hash)maps
 ** - moreover, the elements need to be values, able to be copied and handled at will
 ** - it will be beneficial for these values to support move semantics explicitly
 ** - in addition, the tree diffing suggests a mechanism to re-gain the fully
 **   typed context, either based on some kind of embedded type tag, or
 **   alternatively by visitation and matching
 ** - finally, the handling of changes prompts us to support installation
 **   of a specifically typed <i>change handling closure</i>.
 ** 
 ** \par monadic nature
 ** 
 ** As suggested by the usage for representation of tree shaped data, we acknowledge
 ** that GenNode is a <b>Monad</b>. We support the basic operations \em construction
 ** and \em flatMap. To fit in with this generic processing pattern, the one element
 ** flavours of GenNode are considered the special case, while the collective flavours
 ** form the base case -- every GenNode can be iterated. The \em construction requirement
 ** suggests that GenNode may be created readily, just by wrapping any given and suitable
 ** element, thereby picking up the element's type. For sake of code organisation and
 ** dependency management, we solve this requirement with the help of a trait type,
 ** expecting the actual usage to supply the necessary specialisations on site.
 ** 
 ** @todo the purpose and goal of the monadic approach is not clear yet (5/2015).
 **       To begin with, for the task of diff detection and application, it is sufficient
 **       to get the children as traversable collection
 ** 
 ** @see diff-index-table-test.cpp
 ** @see diff-list-generation-test.cpp
 ** @see DiffDetector
 ** 
 */


#ifndef LIB_DIFF_GEN_NODE_H
#define LIB_DIFF_GEN_NODE_H


#include "lib/hash-indexed.hpp"                 ///< @warning needs to be first, see explanation in lib/hash-standard.hpp

#include "lib/error.hpp"
#include "lib/variant.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/diff/record.hpp"
#include "lib/idi/entry-id.hpp"
//#include "lib/util.hpp"
//#include "lib/format-string.hpp"

#include "lib/format-util.hpp"
#include "lib/variant.hpp"
#include "lib/util.hpp"

//#include <vector>
#include <string>
//#include <map>


namespace lib {
namespace diff{
  
  namespace error = lumiera::error;
  
  using std::string;
  
  namespace {//////TODO this is a prototype, to be factored out
    
    template<typename X, class SELF>
    struct ShaddowCopyCtor
      {
        operator bool()  const { return true; }
      };
    
    template<class SELF>
    struct ShaddowCopyCtor<SELF, SELF>
      {
        // no bool conversion -> substitution fails.
      };
    template<class SELF>
    struct ShaddowCopyCtor<SELF&, SELF>
      {
        // no bool conversion -> substitution fails.
      };
    template<class SELF>
    struct ShaddowCopyCtor<const SELF, SELF>
      {
        // no bool conversion -> substitution fails.
      };
    
  }//(End) copy shaddowing solution
  
  
  class GenNode;
  
  using Rec = Record<GenNode>;
  using DataValues = meta::Types<int
                                ,int64_t
                                ,short
                                ,char
                                ,bool
                                ,double
                                ,string
                                ,time::Time
                                ,time::Offset
                                ,time::Duration
                                ,time::TimeSpan
                                ,hash::LuidH
                                ,Rec
                                >;
  
  
  
  class DataCap
    : public Variant<DataValues>
    {
    public:
      template<typename X>
      DataCap(X&& x,   bool = ShaddowCopyCtor<X, DataCap>())
        : Variant<DataValues>(std::forward<X>(x))
        { }
      
                                       ////////////////////////TICKET #963  Forwarding shadows copy operations -- generic solution??
    };
  
  
  /** generic data element node within a tree */
  struct GenNode
    {
      class ID
        : public idi::BareEntryID
        {
          friend class GenNode;
          
          template<typename X>
          ID (X*, string const& symbolicID)
            : idi::BareEntryID (symbolicID,
                                idi::getTypeHash<X>())
            { }
          
        public:
          ID (GenNode const& node)
            : ID(node.idi)
            { }
          
          // standard copy operations acceptable
        };
      
      ID      idi;
      DataCap data;
      
      
      template<typename X>
      GenNode(X&& val,   bool = ShaddowCopyCtor<X, GenNode>())
        : idi(&val, buildChildID<X>())
        , data(std::forward<X>(val))
        { }
      
      template<typename X>
      GenNode(string const& symbolicID, X&& val)
        : idi(&val, symbolicID)
        , data(std::forward<X>(val))
        { }
      
      GenNode(string const& symbolicID, const char* text)
        : GenNode(symbolicID, string(text))
        { }
      
      GenNode(const char* text)
        : GenNode(string(text))
        { }
      
      // default copy assignable
      
      
      bool
      isNamed()  const
        {
          return util::startsWith (idi.getSym(), "_CHILD_");
        }
      
      friend string
      name (GenNode const& node)
      {
        return node.idi.getSym();
      }
      
    private:
      template<typename X>
      static string
      buildChildID()
        {
          return "_CHILD_" + idi::generateSymbolicID<X>();
        }
    };
  
  
  
  /* === Specialisation for handling of attributes in Record<GenNode> === */
  
  template<>
  inline bool
  Rec::isAttribute (GenNode const& v)
  {
    return false; ////TODO
  }
  
  template<>
  inline bool
  Rec::isTypeID (GenNode const& v)
  {
    return false; ////TODO
  }
  
  template<>
  inline string
  Rec::extractTypeID (GenNode const& v)
  {
    return "todo"; ////TODO
  }
  
  template<>
  inline GenNode
  Rec::buildTypeAttribute (string const& typeID)
  {
    return GenNode("type", typeID);
  }
  
  template<>
  inline string
  Rec::extractKey (GenNode const& v)
  {
    return v.idi.getSym();
  }
  
  template<>
  inline GenNode
  Rec::extractVal (GenNode const& v)
  {
    return GenNode(v); ///TODO
  }
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_GEN_NODE_H*/
