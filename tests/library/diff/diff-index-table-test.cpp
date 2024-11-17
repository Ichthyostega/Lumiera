/*
  DiffIndexTable(Test)  -  simple sequence lookup table

   Copyright (C)
     2015,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file diff-index-table-test.cpp
 ** unit test \ref DiffIndexTable_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/diff/index-table.hpp"

#include <utility>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::swap;


namespace lib {
namespace diff{
namespace test{
  
  using lumiera::error::LUMIERA_ERROR_LOGIC;
  
  namespace {//Test fixture....
    
    using DataSeq = vector<string>;
    using Index = IndexTable<string>;
    
    #define TOK(id) id(STRINGIFY(id))
    
    string TOK(a1), TOK(a2), TOK(a3), TOK(a4), TOK(a5);
    string TOK(b1), TOK(b2), TOK(b3), TOK(b4);
    
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /*****************************************************************************//**
   * @test Cover a simple lookup table helper, used to support diff generation.
   *       The IndexTable takes a snapshot of generic sequence data on construction.
   *       After that, it is immutable and supports lookup by element as key and
   *       membership check.
   *       
   * @see IndexTable
   * @see DiffListApplication_test
   */
  class DiffIndexTable_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          verifySnapshot();
          sequenceIteration();
          duplicateDetection();
          copy_and_move();
        }
      
      
      void
      simpleUsage()
        {
          DataSeq data({a5,a2,a1,a4,a3});
          Index idx(data);
          
          CHECK (5 == idx.size());
          
          CHECK (idx.contains(a1));
          CHECK (idx.contains(a2));
          CHECK (idx.contains(a3));
          CHECK (idx.contains(a4));
          CHECK (idx.contains(a5));
          
          CHECK (!idx.contains(b1));
          CHECK (!idx.contains(b2));
          
          
          CHECK (a5 == idx.getElement(0));
          CHECK (a2 == idx.getElement(1));
          CHECK (a1 == idx.getElement(2));
          CHECK (a4 == idx.getElement(3));
          CHECK (a3 == idx.getElement(4));
          
          
          CHECK (0 == idx.pos(a5));
          CHECK (1 == idx.pos(a2));
          CHECK (2 == idx.pos(a1));
          CHECK (3 == idx.pos(a4));
          CHECK (4 == idx.pos(a3));
        }
      
      
      void
      verifySnapshot()
        {
          DataSeq data({a5,a2,a1,a4,a3});
          Index idx(data);
          
          data.clear();
          data.push_back(b1);
          
          CHECK (5 == idx.size());
          
          CHECK (idx.contains(a1));
          CHECK (idx.contains(a2));
          CHECK (idx.contains(a3));
          CHECK (idx.contains(a4));
          CHECK (idx.contains(a5));
          
          CHECK (!idx.contains(b1));
          CHECK (!idx.contains(b2));
          
          CHECK (0 == idx.pos(a5));
          CHECK (1 == idx.pos(a2));
          CHECK (2 == idx.pos(a1));
          CHECK (3 == idx.pos(a4));
          CHECK (4 == idx.pos(a3));
        }
      
      
      void
      sequenceIteration()
        {
          DataSeq data({a5,a2,a1,a4,a3});
          
          size_t i = 0;
          for (auto elm : Index(data))
            {
              CHECK (elm == data[i++]);
            }
        }
      
      
      void
      duplicateDetection()
        {
          DataSeq data({a5,a2,a1,a4,a2,a3});
          
          VERIFY_ERROR(LOGIC, Index idx(data));
        }
      
      
      void
      copy_and_move()
        {
          DataSeq seqA({a5,a4,a1,a2,a3});
          DataSeq seqB({b4,b3,b2,b1});
          
          Index idxA(seqA);
          Index idxB(seqB);
          CHECK (5 == idxA.size());
          CHECK (4 == idxB.size());
          
          CHECK ( idxA.contains(a1));
          CHECK (!idxA.contains(b1));
          CHECK (!idxB.contains(a1));
          CHECK ( idxB.contains(b1));
          
          swap (idxA, idxB);
          
          CHECK (!idxA.contains(a1));
          CHECK ( idxA.contains(b1));
          CHECK ( idxB.contains(a1));
          CHECK (!idxB.contains(b1));
          
          idxB = idxA;
          CHECK (4 == idxA.size());
          CHECK (4 == idxB.size());
          
          CHECK (!idxA.contains(a1));
          CHECK ( idxA.contains(b1));
          CHECK (!idxB.contains(a1));
          CHECK ( idxB.contains(b1));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DiffIndexTable_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
