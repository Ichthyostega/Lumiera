/*
  FunTrampoline(Test)  -  demonstrate building a static dispatcher table

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file fun-trampoline-test.cpp
 ** unit test \ref FunTrampoline_test
 */



#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/format-string.hpp"
#include "lib/format-obj.hpp"

#include "lib/meta/fun-trampoline.hpp"


using test::showType;
using std::operator""s;
using std::string;
using util::_Fmt;


namespace lib  {
namespace meta {
namespace test {
  
  namespace { // Builder to populate the trampoline table
    
    template<typeseq SEQ>
    struct Stringify
      {
        using Tuple = ElmTypes<SEQ>::Tup;
        using Functor = string(*)(Tuple const&);
        
        template<size_t idx>
        constexpr auto
        build()
          {
            using ElmT = std::tuple_element_t<idx, Tuple>;
            
            return [](Tuple const& tuple) -> string
                    {
                      static _Fmt render{"%02d: «%s»%|20t||%s"};
                      return {render % idx
                                     % util::showType<ElmT>()
                                     % util::toString<ElmT> (getElm<idx> (tuple))};
                    };
          }
      };
  }
  
  
  
  /*************************************************************//**
   * @test demonstrate the setup of a static functor trampoline.
   * @see TupleIdxAdaptor_test
   */
  class FunTrampoline_test : public Test
    {
      virtual void
      run (Arg)
        {
          demonstrate_TupleAccessor();
        }
      
      
      
      /** @test build a trampoline table to access and handle tuple fields
       *      - [setup a builder](\ref Stringify) to populate the table.
       *      - use a typedef to provide the actual tuple type
       *      - without further ado, the trampoline is ready
       */
      void
      demonstrate_TupleAccessor()
        {
          auto tup = std::make_tuple (1, 2.3, '4', "⁵₅⁵", "😈"s);
          
          using Tramp = TupStringer<decltype(tup)>;
          CHECK (Tramp::size()   == 5);
          CHECK (Tramp::memSiz() == 5 * sizeof(void*));
          
          CHECK (showType<Tramp::ResType>() == "string"_expect);
          
          CHECK (Tramp::dispatch(0)(tup) == "00: «int»         |1"_expect  );
          CHECK (Tramp::dispatch(1)(tup) == "01: «double»      |2.3"_expect);
          CHECK (Tramp::dispatch(2)(tup) == "02: «char»        |4"_expect  );
          CHECK (Tramp::dispatch(3)(tup) == "03: «const char *»|⁵₅⁵"_expect);
          CHECK (Tramp::dispatch(4)(tup) == "04: «string»      |😈"_expect  );
          
          getElm<3>(tup) = "🚀💣🔥";
          CHECK (Tramp::dispatch(3)(tup) == "03: «const char *»|🚀💣🔥"_expect);
        }
      
      template<tup Tup>
      using TupStringer = FunTrampoline<Stringify, typename ElmTypes<Tup>::Seq>;
    };
  
  LAUNCHER (FunTrampoline_test, "unit common");
  
  
}}} // namespace lib::meta::test
