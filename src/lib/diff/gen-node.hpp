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
 ** Representation built from GenNode elements is intended to support
 ** introspection of data structures and exchange of mutations in the
 ** form of \link diff-language.hpp diff messages. \endlink
 ** 
 ** Despite of the name, GenNode is \em not meant to be an universal
 ** data representation; rather it is limited to embody a fixed hard
 ** wired set of data elements relevant to stand-in for attributes
 ** and scope contents of the lumiera high-level data model.
 ** 
 ** \par Anatomy of a GenNode
 ** 
 ** GenNode is a polymorphic value with well defined identity and type-ID.
 ** Each element is conceived to be »unique within context« -- as defined
 ** by the immediately visible scope within a tree like structure.
 ** Beyond this identity metadata, each GenNode carries a DataCap, which
 ** is an inline container and attachment point for payload data. Simple
 ** attribute values can be carried alongside, while more complex types
 ** or entities bound to a reference and registration system (e.g. Placement)
 ** will be referred by a suitable reference representation (PlacementID).
 ** The DataCap is what creates the polymorphic nature, where the common
 ** interface is mostly limited to managemental tasks (copying of values,
 ** external representation). Besides, there is are special flavours of
 ** the DataCap to represent \em sub-collections of GenNode elements.
 ** Especially, the \ref Record type is a kind of collection suitable
 ** to represent object-like structures, since it both holds several
 ** \am attributes referable by-name, and a (ordered) collection
 ** of elements treated as children within the scope of the
 ** given record.
 ** 
 ** @see diff-index-table-test.cpp
 ** @see diff-list-generation-test.cpp
 ** @see DiffDetector
 ** 
 */


#ifndef LIB_DIFF_GEN_NODE_H
#define LIB_DIFF_GEN_NODE_H


#include "lib/error.hpp"
//#include "lib/util.hpp"
//#include "lib/format-string.hpp"

//#include <vector>
//#include <map>


namespace lib {
namespace diff{
  
  namespace error = lumiera::error;
  
//using util::_Fmt;
  
  class DataCap;
  
  
  /** generic data element node within a tree */
  class GenNode
    {
      
    public:
    };
  
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_GEN_NODE_H*/
