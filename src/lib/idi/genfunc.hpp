/*
  GENFUNC.hpp  -  generic identification functions

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


/** @file genfunc.hpp
 ** Generic function to build identification schemes.
 ** These template functions are meant as common extension point.
 ** The purpose is to streamline and disentangle the various identification schemes
 ** in use at various places within Lumiera. We strive to cover all the common basic
 ** usage situations through these functions
 ** - build a symbolic ID
 ** - build a classification record
 ** - create a readable yet unique ID
 ** - render an ID in human readable form
 ** - derive a hash function
 ** 
 ** @see EntryID
 **
 */



#ifndef LIB_IDI_GENFUNC_H
#define LIB_IDI_GENFUNC_H

#include "lib/hash-value.h"
#include "lib/symbol.hpp"
#include "lib/typed-counter.hpp"
//#include "lib/hash-standard.hpp"

#include <typeinfo>
#include <string>


namespace lib {
namespace idi {
  
  using lib::HashVal;
  using std::string;
  
  namespace { // integration helpers...
    string demangled_innermost_component (const char* rawName);
    string demangled_sanitised_name      (const char* rawName);
    
    string instance_formatter (string const& prefix, long instanceNr);
    
  } //(End)integration helpers...
  
  
  
  /** Short readable type identifier, not necessarily unique or complete.
   * @return the innermost component of the demangled C++ type name.
   *         Usually, this is the bare name without any namespaces.
   */
  template<typename TY>
  inline string
  typeSymbol()
  {
    return demangled_innermost_component (typeid(TY).name());
  }
  
  /** Complete unique type identifier
   * @return complete demangled C++ type name, additionally
   *         passed through our ID sanitiser function, i.e.
   *         one word, no whitespace, only minimal punctuation
   */
  template<typename TY>
  inline string
  typeFullID()
  {
    return demangled_sanitised_name (typeid(TY).name());
  }
  
  template<typename TY>
  inline string
  categoryFolder()
  {
    return typeSymbol<TY>();
  }
  
  template<typename TY>
  inline string
  namePrefix()
  {
    return typeSymbol<TY>();
  }
  
  
  /** build a per-type unique identifier.
   * @return a type based prefix, followed by an instance number
   * @note we use the short prefix without namespace, not necessarily unique
   * @warning this operation is not exactly cheap; it acquires a lock
   *          for the counter and, after increasing and dropping the lock,
   *          it builds and uses a boost::format instance.
   */
  template<class TY>
  inline string
  generateSymbolicID()
  {
    static TypedCounter instanceCounter;
    return instance_formatter (namePrefix<TY>(), instanceCounter.inc<TY>());
  }
  
  /**
   * @return a boost hash value, based on the full (mangled) C++ type name
   */
  template<typename TY>
  inline HashVal
  getTypeHash()
  {
    Literal rawTypeName (typeid(TY).name());
    return hash_value (rawTypeName);
  }
  
  
  
}} // namespace lib::idi
#endif /*LIB_IDI_GENFUNC_H*/
