/*
  REF-ARRAY.hpp  -  abstraction providing array-like access to a list of references

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file ref-array.hpp
 ** Abstraction interface: array-like access by subscript
 ** @todo as of 2016, this concept seems very questionable: do we _really_ want
 **       to abstract over random access, or do we _actually_ want for-iteration??
 ** @deprecated 5/2025 to be obsoleted by a similar design
 ** @see several.hpp
 */


#ifndef LIB_REF_ARRAY_H
#define LIB_REF_ARRAY_H


#include "lib/nocopy.hpp"


namespace lib {
  
  /**
   * Abstraction: Array of const references.
   * Typically the return type is an interface, 
   * and the Implementation wraps some datastructure
   * holding subclasses.
   * @todo ouch -- a collection that isn't iterable...  ///////////////////////TICKET #1040
   * @warning in rework 5/2025
   */
  template<class T>
  class RefArray
    : util::NonCopyable
    {
    public:
      virtual ~RefArray() {}  ///< this is an interface
      
      virtual T const& operator[] (size_t i)  const =0;
      virtual size_t size()                   const =0;
    };
  
  
} // namespace lib
#endif
