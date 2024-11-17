/*
  TUPLE-DIAGNOSTICS  -  helper for diagnostics of type tuples

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file tuple-diagnostics.hpp
 ** an extension to typelist-diagnostics.hpp, allowing to dump the \em contents of a Tuple datatype.
 ** With the help of our [generic string converter](\ref util::toString), and the BuildTupleAccessor
 ** defined within tuple-helper.hpp, we're able to show the type and contents of any data record
 ** based on std::tuple. For unit-testing, special formatting is provided for the Num<int>
 ** test types, which makes typelist and tuples of these types a good candidate for tests.
 ** 
 ** @see TupleHelper_test
 ** @see FunctionClosure_test
 ** @see TypelistManip_test
 **
 */


#ifndef META_TUPLE_DIAGNOSTICS_H
#define META_TUPLE_DIAGNOSTICS_H


#include "meta/typelist-diagnostics.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/format-string.hpp"
#include "lib/format-obj.hpp"

#include <string>

using std::string;


namespace util {
  
  template<int i>
  struct StringConv<lib::meta::Num<i>>
    {
      static std::string
      invoke (lib::meta::Num<i> num) noexcept
      {
        static util::_Fmt   constElm("(%i)");
        static util::_Fmt changedElm("{%i}");
        
        return string( (num.o_==i? constElm:changedElm) % int(num.o_));
      }
    };
  
}

namespace lib {
namespace meta {
namespace test {
  
  
  
  /* ===== printing Tuple types and contents ===== */ 
  
  
  template<typename TUP>
  inline                enable_if<is_Tuple<TUP>,
  string                >
  showType ()
  {
    using TypeList = typename RebindTupleTypes<TUP>::List;
    using DumpPrinter = InstantiateChained<TypeList, Printer, NullP>;
    
    return "TUPLE"
         + DumpPrinter::print();
  }
  
  // see the macros DISPLAY and DUMPVAL defined in typelist-diagnostics.hpp
  
  
  
  
}}} // namespace lib::meta::test
#endif
