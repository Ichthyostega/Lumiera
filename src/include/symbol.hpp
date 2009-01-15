/*
  SYMBOL.hpp  -  symbolic constant datatype 
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/

/** @file symbol.hpp
 ** WIP placeholder definition for a planned Symbol datatype.
 ** 
 ** @todo for the (currently just planned as of 11/08) rules based configuration
 ** in the Proc-Layer a explicit Symbol datatype will probably very helpful.
 ** For now we just a typedef is sufficient. A real Symbol datatype should 
 ** - be definable by string constant
 ** - integrate smoothly with std::string
 ** - provide a unique numeric index for each distinct Symbol
 ** - automatically maintain a symbol table at runtime to support this
 ** - provide some magic (macros) allowing to build distinct types based on symbols.
 ** 
 ** @see configrules.hpp
 ** @see query.hpp
 */


#ifndef LUMIERA_SYMBOL_H
#define LUMIERA_SYMBOL_H


#include <string>


namespace lumiera {
  
  typedef const char * const Symbol;  ///< Token or Atom with distinct identity @todo define a real Symbol class, i.e. same literal string==same pointer,

  typedef const std::string Literal; ///< inline string literal @todo improve interaction with Symbol

  
  
} // namespace lumiera

#endif
