/*
  DIAGNOSTIC-OUTPUT.hpp  -  some helpers for investigation and value output

   Copyright (C)
     2022,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file diagnostic-output.hpp
 ** Helpers typically used while writing tests.
 ** In the finished test, we rather want values be asserted explicitly in the
 ** test code, but while building the test, it is often necessary to see the types
 ** and the actual values of intermediary results, to get a clue where matters go south.
 ** 
 ** @see test-helper.hpp
 ** 
 */


#ifndef LIB_TEST_DIAGNOSTIC_OUTPUT_H
#define LIB_TEST_DIAGNOSTIC_OUTPUT_H


#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"

#include <boost/lexical_cast.hpp>
#include <string>


using std::string;

namespace lib {
namespace test{
  
}} // namespace lib::test




/* === test helper macros === */

/**
 * Macro to print types and expressions to STDOUT,
 * using Lumiera's string conversion framework
 */
#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::test::showType<_TY_>() <<endl;
#define SHOW_EXPR(_XX_) \
    cout << "#--◆--# " << STRINGIFY(_XX_) << " ? = " << util::toString(_XX_) <<endl;
#define SHOW_EXPL(_XX_) \
    cout << "#--◆--# " << STRINGIFY(_XX_) << " ? = " << boost::lexical_cast<string>(_XX_) <<endl;


#endif /*LIB_TEST_DIAGNOSTIC_OUTPUT_H*/
