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
 ** \par design decisions
 ** The Record type is shaped from its intended use: It serves to symbolically represent
 ** \em objects in the "external tree description". Here, \em objects means objects for
 ** real, i.e. with types, fields and an enclosed scope. But \em external means that we
 ** do not work on these objects right here, we only represent them, for later referral,
 ** \em symbolically.
 ** 
 ** This leads to the following decisions
 ** - the Record entity is itself an object and thus has an inner side, privately.
 **   The entrails of the Record can be reworked and tuned for performance
 ** - yet the Record has an external appearance, which makes it look flat and passive.
 **   This is to say, a Record has no visible functionality.
 ** - the parts or \em realms within this symbolic representation are distinguished
 **   by convention solely
 **   
 **   * metadata is very limited and boils down to magic attributes known by name
 **   * children (scope contents) can be recognised by \em not bearing a name
 ** 
 ** Record entities are meant to be immutable. The proper way to alter a Record is
 ** to apply a \link tree-diff.hpp diff \endlink
 ** 
 ** \par rationale
 ** The underlying theme of this design is negative, dialectical: we do not want to
 ** build yet another object system. The object model of C++ is deemed adequate.  
 ** 
 ** @see GenericRecordRepresentation_test
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
