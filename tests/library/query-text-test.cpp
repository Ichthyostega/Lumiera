/*
  QueryText(Test)  -  verify syntactical query representation

   Copyright (C)
     2012,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file query-text-test.cpp
 ** unit test \ref QueryText_test
 */



#include "lib/test/run.hpp"
#include "lib/query-util.hpp"
#include "lib/query-text.hpp"
#include "lib/util.hpp"

#include <string>
//#include <map>

using util::contains;
using util::isnil;
using std::string;

namespace lib {
namespace test{
  
  namespace { // test fixture...
  
  } //(End) test fixture
  
  
  
  
  /***************************************************************************//**
   * @test cover basic properties of the generic syntactical query representation.
   *       - build from query string
   *       
   * @todo this is placeholder code. The actual query representation needs to be written      /////////////////////////////TICKET #899
   * @see QueryText
   */
  class QueryText_test : public Test
    {
      
      virtual void run (Arg)
        {
          emptyRepresentation();
          build_from_string();
          useHashValue();
        }
      
      
      void emptyRepresentation ()
        {
          QueryText noText;
          
          CHECK (isnil (noText));
          CHECK (isnil (string(noText)));
        }
      
      
      void build_from_string ()
        {
          QueryText eternal ("beats(lumiera, duke_nukem_forever).");
          CHECK (contains (string(eternal), "lumiera"));
          CHECK (contains (string(eternal), "beats"));
          CHECK (contains (string(eternal), "duke_nukem_forever"));
          CHECK (eternal.hasAtom ("lumiera"));
        }
      
      
      void useHashValue ()
        {
          QueryText one("one");
          QueryText two("two");
          
          CHECK (0 < hash_value(one));
          CHECK (0 < hash_value(two));
          CHECK (hash_value(one) != hash_value(two));
        }
      
      
    };
  
  
  
  /** Register this test class... */
  LAUNCHER(QueryText_test, "unit common");
  
}} // namespace lib::test
