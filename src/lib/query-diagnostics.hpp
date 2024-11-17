/*
  QUERY-DIAGNOSTICS.hpp  -  helpers for writing tests covering config queries

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file query-diagnostics.hpp
 ** diagnostic helpers to support test related to predicate queries
 */


#ifndef LIB_QUERY_DIAGNOSTICS_H
#define LIB_QUERY_DIAGNOSTICS_H


#include "lib/format-string.hpp"
#include "lib/random.hpp"

using util::_Fmt;
using std::string;
using std::rand;



namespace lib  {
namespace query{
namespace test {
  
  namespace {// implementation constants
    
    _Fmt predicatePattern{"%s_%02i( %s )"};
    const string garbage {"asanisimasasmicksmaggtutti"};
    
    const uint MAX_DEGREE_RAND = 9;
    
  }
  
  
  
  inline string
  garbage_term ()         ///< yields a random string of 3 letters
  {
    return predicatePattern
         % char ('a'+ rani(26))
         % rani (100)
         % garbage.substr (rani(23) , 3);
  }
  
  inline string
  garbage_query (int degree=0)    ///< fabricating (random) query strings
  {
    string fake;
    if (!degree) 
      degree = 1 + rani(MAX_DEGREE_RAND);
    while (0 < --degree)
      fake += garbage_term() + ", ";
    fake += garbage_term() + ".";
    return fake;
  }
  
  
  
  
}}} // namespace lib::query::test
#endif
