/*
  RECORD.hpp  -  collection to represent object-like data

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


/** @file record.hpp
 ** Special collection to represent object-like data.
 ** To be used in a context where introspection, open, extensible definitions
 ** and loose coupling of data representation matters. Typically, structures
 ** defined in terms of Record elements are linked to the actual \em core
 ** representation of the same entities relying on
 ** \link diff-language.hpp diff messages. \endlink
 ** Record is one of the supported flavours within the DataCap of GenNode elements,
 ** which in turn serve as the standard handle to refer to other elements, entities,
 ** attributes or references within the "backbone" of the Lumiera GUI.
 ** 
 ** @see diff-index-table-test.cpp
 ** @see diff-list-generation-test.cpp
 ** @see DiffDetector
 ** 
 */


#ifndef LIB_DIFF_RECORD_H
#define LIB_DIFF_RECORD_H


#include "lib/error.hpp"
//#include "lib/util.hpp"
//#include "lib/format-string.hpp"

//#include <vector>
//#include <map>


namespace lib {
namespace diff{
  
  namespace error = lumiera::error;
  
//using util::_Fmt;
  
  
  
  /** object-like record of data */
  template<typename VAL>
  class Record
    {
      
    public:
    };
  
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_GEN_NODE_H*/
