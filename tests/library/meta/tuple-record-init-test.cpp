/*
  TupleRecordInit(Test)  -  to build a tuple from a GenNode sequence

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/

/** @file tuple-record-init-test.cpp
 ** unit test \ref TupleRecordInit_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/meta/tuple-record-init.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"

#include <string>

using lib::Symbol;
using lib::Variant;
using lib::idi::EntryID;
using lib::diff::Rec;
using lib::diff::MakeRec;
using lib::diff::GenNode;
using lib::meta::Types;
using lib::meta::Tuple;
using lib::meta::buildTuple;
using lib::time::Duration;
using lib::time::Time;
using lib::hash::LuidH;

using lumiera::error::LERR_(WRONG_TYPE);

using std::string;
using std::tuple;
using std::get;

namespace lib  {
namespace meta {
namespace test {
  
  
  
  
  
  /*************************************************************************//**
   * @test Metaprogramming: how to unload the contents of a runtime typed
   *       variant sequence into ctor arguments of a (compile time typed) tuple.
   * 
   * This involves two problems
   * - how to combine iteration, compile-time indexing and run-time access.
   * - how to overcome the runtime-to-compile-time barrier, using a
   *   pre-generated double-dispatch (visitor).
   * 
   * The concrete problem leading to the development of such a generic
   * converter was the necessity to receive a command invocation
   * parameter tuple from a Record<GenNode> sent via the UI-Bus.
   * @see ElementExtractor
   * @see GenNodeAccessor
   * @see BusTerm_test::commandInvocation
   * @see stage::test::Nexus::prepareDiagnosticCommandHandler
   * @see ui-bus.hpp UI-Bus
   */
  class TupleRecordInit_test : public Test
    {
      virtual void
      run (Arg)
        {
          show_simpleUsage();
          verify_errorHandling();
        }
      
      
      void
      show_simpleUsage()
        {
          using NiceTypes = Types<string, int>;
          using UgglyTypes = Types<EntryID<long>, Symbol, int, int64_t, double, Duration>; // various conversions and an immutable type (Duration)
          
          Rec args = MakeRec().scope("lalü", 42);
          Rec urgs = MakeRec().scope("lalü", "lala", 12, 34, 5.6, Time(7,8,9));
          
          cout << args <<endl;
          cout << urgs <<endl;
          
          cout << buildTuple<NiceTypes> (args) <<endl;
          cout << buildTuple<UgglyTypes> (urgs) <<endl;
        }
      
      
      void
      verify_errorHandling()
        {
          Rec args = MakeRec().scope("surprise", 42);
          
          using TooMany = Types<string, int, long>;
          VERIFY_ERROR (WRONG_TYPE, buildTuple<TooMany> (args));      // number of types in tuple exceeds capacity of the supplied argument record
          
          using Unsigned = Types<string, uint>;
          using Floating = Types<string, float>;
          using Narrowing = Types<string, short>;
          VERIFY_ERROR (WRONG_TYPE, buildTuple<Unsigned> (args));     // dangerous conversion from signed to unsigned int is prohibited
          VERIFY_ERROR (WRONG_TYPE, buildTuple<Floating> (args));     // conversion from integral to floating point element is prohibited
          VERIFY_ERROR (WRONG_TYPE, buildTuple<Narrowing> (args));    // narrowing conversion from int to short is prohibited
          
          // yet other (non-numeric) conversions are still possible
          Rec timeArg = MakeRec().scope(Time(1,2,3,4));
          using TupStr = Types<string>;
          Tuple<TupStr> tup = buildTuple<TupStr> (timeArg);
          
          CHECK (std::get<string> (tup) == "4:03:02.001");
          CHECK (string(Time(1,2,3,4))  == "4:03:02.001");
          
          
          // conversions from LUID elements are handled restrictively
          Rec hashArg = MakeRec().scope("random", LuidH());
          VERIFY_ERROR (WRONG_TYPE, buildTuple<Unsigned> (args));
          VERIFY_ERROR (WRONG_TYPE, buildTuple<Floating> (args));
          VERIFY_ERROR (WRONG_TYPE, buildTuple<Narrowing> (args));
          
          using ToSizeT = Types<string, size_t>;
          VERIFY_ERROR (WRONG_TYPE, (buildTuple<ToSizeT> (args)));    // not even conversion to size_t is allowed
          
          struct Dummy
            {
              HashVal hash;
              
              Dummy (LuidH const& luid)
                : hash(luid)
                { }
            };
          
          using WithDummy = Types<string, Dummy>;
          
          Tuple<WithDummy> tup2 = buildTuple<WithDummy> (hashArg);    // while any type explicitly constructible from LUID are permitted.
          VERIFY_ERROR (WRONG_TYPE, buildTuple<WithDummy> (args));    // building a Dummy from int(42) is disallowed, of course
          
          HashVal h = get<Dummy>(tup2).hash;
          CHECK (h == hashArg.child(1).data.get<LuidH>());            // note: the narrowing conversion happens within LuidH::operator HashVal()
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TupleRecordInit_test, "unit meta");
  
  
  
}}} // namespace lib::meta::test
