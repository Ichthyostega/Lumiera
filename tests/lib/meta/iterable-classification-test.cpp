/*
  IterableClassification(Test)  -  detecting iterability of a generic type

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file iterable-classification-test.cpp
 ** unit test \ref IterableClassification_test
 ** @see iter-adapter.hpp
 ** @see lib/meta/trait.hpp
 ** @see lib/meta/duck-detector.hpp
 */


#include "test/run.hpp"

#include "lib/meta/trait.hpp"
#include "lib/diff/record.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/itertools.hpp"

#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <set>


using std::string;
using std::vector;
using std::declval;
using lib::time::TimeVar;

namespace lib  {
namespace meta {
namespace test {
  
  namespace { // a custom test container....
    
    /// @note: a bare type definition is sufficient here....
    struct TestSource
      {
        TestSource(uint num);
        
        using RawIter  = vector<int>::iterator;
        using iterator = RangeIter<RawIter>;
        
        iterator begin() ;
        iterator end()   ;
      };
    
    inline auto transformedSource(TestSource& src){ return lib::transformIter (src.begin(), [](int){ return long(0); }); }
    inline auto filteredSource   (TestSource& src){ return lib::filterIter    (src.begin(), [](int){ return true;    }); }
    
    
    /// @note Dummy type that exposes a »*State Core*« interface.
    struct DummyCore
      {
        bool  checkPoint() const;
        uint& yield()      const;
        void  iterNext();
        friend bool operator== (DummyCore const&, DummyCore const&);
      };
    
  }//(End) test containers
  
  
  
  /*******************************************************************************//**
   * @test verify the (static) classification/detection of iterables.
   *       Using some SFINAE trickery, it is possible to detect the following
   *       - a _STL-like container_ with `begin()` and `end()`
   *       - a _Lumiera Forward Iterator_
   *       - a _State Core_
   * This test ensures this compile time detection works as intended;
   * it suffices thus to define some minimal types in order to access
   * the generated meta function value.
   * @see iter-adapter.hpp explanation of _Lumiera Forward Iterator_ and _State Core_
   * @todo 2026 many years later and this type detection has become a cornerstone
   *       of Lumiera's iterator pipeline framework. Since the language level has
   *       been raised recently to C++23, it should be considered to replace
   *       the SFINAE-based traits with Concept definitions.
   */
  class IterableClassification_test : public Test
    {
      
      void
      run (Arg)
        {
          // define a bunch of STL containers
          using TimeVector = std::vector<TimeVar>;
          using LongSet    = std::multiset<long> ;
          using CharMap    = std::map<int,char>  ;
          using BoolList   = std::list<bool>     ;
          using ShortDeque = std::deque<ushort>  ;
          using CustomCont = TestSource          ;
          
          // some types that comply with the "Lumiera Forward Iterator" concept
          using LumieraRangeIter       = TestSource::iterator;
          using LumieraFilteredIter    = decltype(    filteredSource (declval<TestSource&>()) );
          using LumieraTransformedIter = decltype( transformedSource (declval<TestSource&>()) );
          using CustomLumieraIter      = diff::Record<string>::iterator;
          
          // some types that comply with the "State Core" concept
          using CustomStateCore        = DummyCore;
          using LumieraCoreIter        = IterableDecorator<DummyCore>;
          
          
          // detect STL iteration
          CHECK ( true == can_STL_ForEach<TimeVector>::value );
          CHECK ( true == can_STL_ForEach<LongSet>   ::value );
          CHECK ( true == can_STL_ForEach<CharMap>   ::value );
          CHECK ( true == can_STL_ForEach<BoolList>  ::value );
          CHECK ( true == can_STL_ForEach<ShortDeque>::value );
          CHECK ( true == can_STL_ForEach<CustomCont>::value );
          
          CHECK (false == can_STL_ForEach<LumieraRangeIter>      ::value );
          CHECK (false == can_STL_ForEach<LumieraFilteredIter>   ::value );
          CHECK (false == can_STL_ForEach<LumieraTransformedIter>::value );
          CHECK (false == can_STL_ForEach<CustomLumieraIter>     ::value );
          
          CHECK (false == can_STL_ForEach<CustomStateCore>::value );
          CHECK (false == can_STL_ForEach<LumieraCoreIter>::value );
          
          // detect Lumiera Forward Iterator
          CHECK (false == can_LumieraIter<TimeVector>::value );
          CHECK (false == can_LumieraIter<LongSet>   ::value );
          CHECK (false == can_LumieraIter<CharMap>   ::value );
          CHECK (false == can_LumieraIter<BoolList>  ::value );
          CHECK (false == can_LumieraIter<ShortDeque>::value );
          CHECK (false == can_LumieraIter<CustomCont>::value );
          
          CHECK ( true == can_LumieraIter<LumieraRangeIter>      ::value );
          CHECK ( true == can_LumieraIter<LumieraFilteredIter>   ::value );
          CHECK ( true == can_LumieraIter<LumieraTransformedIter>::value );
          CHECK ( true == can_LumieraIter<CustomLumieraIter>     ::value );
          
          CHECK (false == can_LumieraIter<CustomStateCore>::value );
          CHECK ( true == can_LumieraIter<LumieraCoreIter>::value );     // Note: Lumiera iterator capability added as wrapper
          
          // detect State Core
          CHECK (false == is_StateCore<TimeVector>::value );
          CHECK (false == is_StateCore<LongSet>   ::value );
          CHECK (false == is_StateCore<CharMap>   ::value );
          CHECK (false == is_StateCore<BoolList>  ::value );
          CHECK (false == is_StateCore<ShortDeque>::value );
          CHECK (false == is_StateCore<CustomCont>::value );
          
          CHECK (false == is_StateCore<LumieraRangeIter>      ::value );
          CHECK (false == is_StateCore<LumieraFilteredIter>   ::value );
          CHECK (false == is_StateCore<LumieraTransformedIter>::value );
          CHECK (false == is_StateCore<CustomLumieraIter>     ::value );
          
          CHECK ( true == is_StateCore<CustomStateCore>::value );
          CHECK ( true == is_StateCore<LumieraCoreIter>::value );
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (IterableClassification_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
