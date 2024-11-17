/*
  DiffListGeneration(Test)  -  demonstrate list diff generation

   Copyright (C)
     2014,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file diff-list-generation-test.cpp
 ** unit test \ref DiffListGeneration_test
 */


#include "lib/test/run.hpp"
#include "lib/diff/list-diff-detector.hpp"
#include "lib/itertools.hpp"
#include "lib/util.hpp"

#include <string>
#include <vector>

using lib::append_all;
using util::isnil;
using std::string;
using std::vector;


namespace lib {
namespace diff{
namespace test{
  
  namespace {//Test fixture....
    
    using DataSeq = vector<string>;
    
    #define TOK(id) id(STRINGIFY(id))
    
    string TOK(a1), TOK(a2), TOK(a3), TOK(a4), TOK(a5);
    string TOK(b1), TOK(b2), TOK(b3), TOK(b4);
    
    using Interpreter = ListDiffInterpreter<string>;
    using DiffStep = ListDiffLanguage<string>::DiffStep;
    using DiffSeq = vector<DiffStep>;
    
    DiffStep_CTOR(ins);
    DiffStep_CTOR(del);
    DiffStep_CTOR(pick);
    DiffStep_CTOR(find);
    DiffStep_CTOR(skip);
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /***********************************************************************//**
   * @test Demonstration/Concept: how to derive a list diff representation
   *       from the comparison of two sequences. The changes necessary to
   *       transform one sequence into the other one are given as a linear
   *       sequence of elementary mutation operations.
   *       
   *       The change detector assumes elements with well defined identity
   *       and uses an index table for both sequences. The diff is generated
   *       progressively, demand-driven.
   *       
   * @see DiffListApplication_test
   */
  class DiffListGeneration_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          DataSeq toObserve({a1,a2,a3,a4,a5});
          DiffDetector<DataSeq> detector(toObserve);
          
          CHECK (!detector.isChanged());
          toObserve = {b1,a3,a5,b2,b3,a4,b4};
          CHECK (detector.isChanged());
          
          auto changes = detector.pullUpdate();
          CHECK (!isnil (changes));
          CHECK (!detector.isChanged());       // pullUpdate() also took a new snapshot
          
          DiffSeq generatedDiff;
          append_all (changes, generatedDiff);
          
          CHECK (generatedDiff == DiffSeq({del(a1)
                                         , del(a2)
                                         , ins(b1)
                                         , pick(a3)
                                         , find(a5)
                                         , ins(b2)
                                         , ins(b3)
                                         , pick(a4)
                                         , ins(b4)
                                         , skip(a5)
                                         }));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DiffListGeneration_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
