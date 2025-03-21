/*
  Test-Helper  -  collection of functions supporting unit testing

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file test-helper.cpp
 ** definition of some widely used test helper functions.
 ** 
 ** @see TestHelper_test
 ** @see TestHelperDemangling_test
 ** 
 */


#include "lib/test/test-helper.hpp"
#include "lib/test/tracking-dummy.hpp"
#include "lib/unique-malloc-owner.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/random.hpp"

#include <string>

using util::_Fmt;
using std::string;

namespace lib {
namespace test{

  
  /** storage for test-dummy flags */
  long Dummy::_local_checksum = 0;
  bool Dummy::_throw_in_ctor = false;
  
  EventLog Tracker::log{"Instance-Tracker"};
  
  
  
  string
  showSizeof (size_t siz, string name)
  {
    static _Fmt fmt{"sizeof( %s ) %|40t|= %3d"};
    return fmt % name % siz;
  }
  
  
  
  
  /** @todo probably this can be done in a more clever way. Anyone...?
   */
  string
  randStr (size_t len)
  {
    static const string alpha{"aaaabbccddeeeeffgghiiiijjkkllmmnnooooppqqrrssttuuuuvvwwxxyyyyzz0123456789"};
    static const size_t MAXAL{alpha.size()};
    
    string garbage(len,'\0');
    size_t p = len;
    while (p)
      garbage[--p] = alpha[rani (MAXAL)];
    return garbage;
  }
  
  
  
  /**
   * @internal check equality and print difference
   * @remark defined here to avoid inclusion of `<iostream>` in header
   */
  bool
  ExpectString::verify (std::string const& actual)  const
  {
    std::string const& expected{*this}; // to avoid endless recursion
    bool expectationMatch {actual == expected};
    if (not expectationMatch)
      {
        cerr << "FAIL___expectation___________"
             << "\nexpect:"<<expected
             << "\nactual:"<<actual
             << endl;
      }
    return expectationMatch;
  }
  
  
}} // namespace lib::test
