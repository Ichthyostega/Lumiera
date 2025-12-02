/*
  UtilSanitizedIdentifier(Test)  -  remove non-standard-chars and punctuation

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file util-sanitised-identifier-test.cpp
 ** unit test \ref UtilSanitizedIdentifier_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"


#include <iostream>
using std::cout;



namespace util {
namespace test {
  
  
  class UtilSanitizedIdentifier_test : public Test
    {
      virtual void run (Arg)
        {
          CHECK (sanitise (                              "Word") == "Word"_expect);
          CHECK (sanitise (                        "a Sentence") == "a_Sentence"_expect);
          CHECK (sanitise (    "trailing Withespace\n       \t") == "trailing_Withespace"_expect);
          CHECK (sanitise ("with    a  \t lot\n  of Whitespace") == "with_a_lot_of_Whitespace"_expect);
          CHECK (sanitise (  "@with\".\'much ($punctuation)[]!") == "@with.much_($punctuation)"_expect);
          CHECK (sanitise (            "§&Ω%€  leading garbage") == "leading_garbage"_expect);
          CHECK (sanitise (              "mixed    Ω   garbage") == "mixed_garbage"_expect);
          CHECK (sanitise (                          "Bääääh!!") == "Bh"_expect);
          CHECK (sanitise (                             "§&Ω%€") == ""_expect);
        }
    };
  
  LAUNCHER (UtilSanitizedIdentifier_test, "unit common");
  
  
}} // namespace util::test

