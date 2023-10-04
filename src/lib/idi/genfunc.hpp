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
 ** Generic functions to build identification schemes.
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
 ** @todo better unit test coverage
 ** @todo improve implementation of typeFullID
 ** @see GenericIdFunction_test
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
namespace meta{
  std::string demangleCxx (lib::Literal rawName);
  std::string humanReadableTypeID (lib::Literal);
  std::string primaryTypeComponent (lib::Literal);
  std::string sanitisedFullTypeName(lib::Literal);
}// implemented in format-obj.cpp

namespace idi {
  
  using lib::HashVal;
  using std::string;
  
  namespace format { // integration helpers...
    
    string instance_format (string const& prefix, size_t instanceNr);
    string instance_hex_format (string const& prefix, size_t instanceNr);
    
  } //(End)integration helpers...
  
  
  
  /** Short readable type identifier, not necessarily unique or complete.
   * @return the innermost component of the demangled C++ type name.
   *         Usually, this is the bare name without any namespaces.
   * @note this function is also defined in lib/meta/util.hpp,
   *       both delegating to the same implementation
   */
  template<typename TY>
  inline string
  typeSymbol()
  {
    return lib::meta::primaryTypeComponent (typeid(TY).name());
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
    return lib::meta::sanitisedFullTypeName (typeid(TY).name());
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
  
  /** designation of an distinct object instance
   * @par obj pointer to the memory location of the object
   * @return a notation "typename.hash", where the hash is given
   *         as 4 hex digits derived from the memory location
   */
  template<typename TY>
  inline string
  instanceTypeID(const TY *const obj)
  {
    return format::instance_hex_format (namePrefix<TY>(), (size_t(obj) / alignof(TY)) % (1<<16));
  }
  
  
  TypedCounter& sharedInstanceCounter();
  
  
  /** build a per-type identifier, with type prefix and running counter.
   * @return a type based prefix, followed by an instance number
   * @note we use the short prefix without namespace, not necessarily unique
   * @todo consequently the generated IDs might clash for two distinct types,
   *          which generate the same \c namePrefix(). Is this a problem?
   * @warning this operation is not exactly cheap; it acquires a lock
   *          for the counter and, after increasing and dropping the lock,
   *          it builds and uses a boost::format instance.
   */
  template<class TY>
  inline string
  generateSymbolicID()
  {
    return format::instance_format (namePrefix<TY>(), sharedInstanceCounter().inc<TY>());
  }
  
  /** build a long type based identifier, with running counter and custom prefix.
   * @param prefix optional prefix to prepend to the generated ID
   * @return a ID string based on the full type, followed by an instance number
   * @warning for one, like \ref generateSymbolicID(), this operation is not really
   *          cheap. And then, since the type ID is slightly abbreviated and then
   *          mangled, there is still the possibility of occasional clashes.
   */
  template<class TY>
  inline string
  generateExtendedID(string prefix ="")
  {
    return format::instance_format (prefix + typeFullID<TY>(), sharedInstanceCounter().inc<TY>());
  }
  
  /**
   * @return a standard hash value, based on the full (mangled) C++ type name
   */
  template<typename TY>
  inline HashVal
  getTypeHash()
  {
    return typeid(TY).hash_code();
  }
  
  
  
}} // namespace lib::idi
#endif /*LIB_IDI_GENFUNC_H*/
