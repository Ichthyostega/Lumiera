/*
  SYMBOL-TABLE.hpp  -  registry for automatically interned symbol string tokens 

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file symbol-table.hpp
 ** Registry table for automatically _interned strings_.
 ** The implementation of the lib::Symbol token relies on unique string pointers,
 ** such as to create one distinct identity for each distinct "symbol string". The idea
 ** is to generate unique and distinct numeric token IDs, while still holding onto a human readable
 ** string. Which in turn requires us to manage a registry of already known symbol strings; when a
 ** Symbol object with such an already known string is created, it will thus connect internally
 ** to the known token ID.
 ** 
 ** @todo as of this writing (4/2017), it is neither clear if we really need such a facility, nor
 **       do we know enough to judge the implementation approach taken here. It might well turn out
 **       that a mere hash over the symbol string would be sufficient. Just the general vision for
 **       Lumiera indicates that we're going to rely heavily on some kind of symbolic or meta processing,
 **       involving a rules based query system, and in the light of such future developments, introducing
 **       a distinct lib::Symbol datatype seemed like a good idea. When we started to generate command IDs
 **       systematically, just literal c-string constants weren't sufficient anymore, leading to this
 **       very preliminary table based implementation.
 ** 
 ** @see symbol-impl.cpp 
 ** @see Symbol_test
 ** @see Symbol_HashtableTest
 ** @see SessionCommandFunction_test multithreaded access
 */


#ifndef LIB_SYMBOL_TABLE_H
#define LIB_SYMBOL_TABLE_H

#include "lib/sync.hpp"
#include "lib/symbol.hpp"
#include "lib/nocopy.hpp"

#include <unordered_set>
#include <utility>
#include <string>


namespace lib {
  
  using std::string;
  using std::move;
  
  
  /** 
   * Table for automatically _interned strings_.
   * This table is used to back the lib::Symbol token type,
   * which is implemented by a pointer into this registration table
   * for each new distinct "symbol string" created.
   * @warning grows eternally, never shrinks
   */
  class SymbolTable
    : public Sync<>
    , util::NonCopyable
    {
      std::unordered_set<string> table_;
      
    public:
      Literal
      internedString (string && symbolString)
        {
          Lock sync{this};
          auto res = table_.insert (move (symbolString));
          return res.first->c_str();
        }
    };
  
  
  
} // namespace lib
#endif /*LIB_SYMBOL_TABLE_H*/
