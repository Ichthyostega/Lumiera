/*
  UtilParseBool(Test)  -  derive bool value from text form

   Copyright (C)
     2018,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file util-parse-bool-test.cpp
 ** unit test \ref UtilParseBool_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include <string>


using lumiera::error::LUMIERA_ERROR_INVALID;


namespace util {
namespace test {
  
  
  class UtilParseBool_test : public Test
    {
      virtual void
      run (Arg)
        {
          CHECK (boolVal ("true"));
          CHECK (boolVal ("True"));
          CHECK (boolVal ("TRUE"));
          CHECK (boolVal ("tRuE"));
          CHECK (boolVal ("yes"));
          CHECK (boolVal ("Yes"));
          CHECK (boolVal ("YES"));
          CHECK (boolVal ("1"));
          CHECK (boolVal ("+"));
          
          CHECK (not boolVal ("false"));
          CHECK (not boolVal ("False"));
          CHECK (not boolVal ("FALSE"));
          CHECK (not boolVal ("fAlSe"));
          CHECK (not boolVal ("no"));
          CHECK (not boolVal ("No"));
          CHECK (not boolVal ("NO"));
          CHECK (not boolVal ("0"));
          CHECK (not boolVal ("-"));
          
          CHECK (boolVal ("yes "));
          CHECK (boolVal (" Yes"));
          CHECK (boolVal (" +  "));
          CHECK (not boolVal (" \n0 "));
          
          VERIFY_ERROR (INVALID, boolVal("")         );
          VERIFY_ERROR (INVALID, boolVal(" ")        );
          VERIFY_ERROR (INVALID, boolVal("Ja")       );
          VERIFY_ERROR (INVALID, boolVal("truth")    );
          VERIFY_ERROR (INVALID, boolVal("falsehood"));
          VERIFY_ERROR (INVALID, boolVal("11")       );
          VERIFY_ERROR (INVALID, boolVal("+1")       );
          VERIFY_ERROR (INVALID, boolVal("↯")        );
          
          
          CHECK (isYes ("true"));
          CHECK (isYes ("True"));
          CHECK (isYes ("tRuE"));
          CHECK (isYes ("TRUE"));
          CHECK (isYes ("yes"));
          CHECK (isYes ("Yes"));
          CHECK (isYes ("1"));
          CHECK (isYes ("+"));
          
          CHECK (isYes (" True   "));
          CHECK (isYes (" \n\n 1 \t "));

          CHECK (not isYes (" True and False"));
          CHECK (not isYes ("+2"));
          CHECK (not isYes ("no"));
          CHECK (not isYes ("1010"));
          CHECK (not isYes ("↯"));
          CHECK (not isYes (" "));
          CHECK (not isYes (""));
        }
    };
  
  LAUNCHER (UtilParseBool_test, "unit common");
  
  
}} // namespace util::test

