/*
  DUMMY-FUNCTIONS  -  mock functions for checking functor utils

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file dummy-functions.hpp
 ** dummy functions to support unit testing of function metaprogramming helpers
 */


#ifndef LIB_META_DUMMY_FUNCTIONS_H
#define LIB_META_DUMMY_FUNCTIONS_H




namespace lib  {
namespace meta {
namespace test {
  
  
  namespace { // test helpers
  
    int _sum_ =0;  ///< used to verify the effect of testFunc
  
    void
    testFunc (int i, char c)
    {
      _sum_ += i + c;
    }
    
    int
    returnIt ()
    {
      return _sum_;
    }
  }
  
  
  
}}} // namespace lib::meta::test
#endif
