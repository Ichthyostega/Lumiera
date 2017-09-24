/*
  PathArray(Test)  -  verify container abstraction for a sequence of path elements

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file path-array-test.cpp
 ** unit test \ref PathArray_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/path-array.hpp"
//#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"
//#include "lib/idi/entry-id.hpp"
//#include "lib/diff/gen-node.hpp"
#include "lib/util.hpp"

#include <string>


using std::string;
//using lib::idi::EntryID;
//using lib::diff::GenNode;
//using util::isSameObject;
using util::isnil;
using util::join;



namespace lib  {
namespace test {
  
//  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
  using lumiera::error::LUMIERA_ERROR_INDEX_BOUNDS;
  using lumiera::error::LUMIERA_ERROR_LOGIC;
  
  namespace { //Test fixture...
    
  }//(End)Test fixture
  
  
  /******************************************************************************//**
   * @test verify abstraction wrapper to handle some char pointers in the way of a
   *       standard sequence container with iteration and range checks.
   * 
   * @see path-array.hpp
   * @see UICoord_test
   * @see Navigator
   */
  class PathArray_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verify_basics();
          verify_iteration();
          verify_comparisons();
        }
      
      
      void
      verify_basics()
        {
          PathArray undef;
          CHECK (isnil (undef));
          
          PathArray parr{"Γ","Δ","Θ","Ξ","Σ","Ψ","Φ","Ω"};
          CHECK (not isnil (parr));
          CHECK (8 == parr.size());
          
          // indexed access
          CHECK ("Γ" == parr[0]);
          CHECK ("Δ" == parr[1]);
          CHECK ("Θ" == parr[2]);
          CHECK ("Ξ" == parr[3]);
          CHECK ("Σ" == parr[4]);
          CHECK ("Ψ" == parr[5]);
          CHECK ("Φ" == parr[6]);
          CHECK ("Ω" == parr[7]);
          
          VERIFY_ERROR (INDEX_BOUNDS, parr[8]);
          
          // string representation
          CHECK ("Γ/Δ/Θ/Ξ/Σ/Ψ/Φ/Ω" == string(parr));
          
          // new value can be assigned, but not altered in place
          parr = PathArray{nullptr,nullptr,"Θ",nullptr,nullptr,nullptr,"Φ"};
          CHECK (7 == parr.size());
          
          // representation is trimmed and filled
          CHECK ("Θ/*/*/*/Φ" == string(parr));
          CHECK (NULL == parr[0]);
          CHECK (NULL == parr[1]);
          CHECK ("Θ" ==  parr[2]);
          CHECK ("*" ==  parr[3]);
          CHECK ("*" ==  parr[4]);
          CHECK ("*" ==  parr[5]);
          CHECK ("Φ" ==  parr[6]);
          VERIFY_ERROR (INDEX_BOUNDS, parr[7]);
          VERIFY_ERROR (INDEX_BOUNDS, parr[8]);
        }
      
      
      void
      verify_iteration()
        {
          PathArray parr;
          CHECK (isnil (parr));
          // path is iterable
          CHECK ("" == join(parr));
          
          parr = PathArray{"Γ","Δ","Θ","Ξ","Σ","Ψ","Φ","Ω"};
          CHECK (8 == parr.size());
          // path is iterable
          CHECK ("ΓΔΘΞΣΨΦΩ" == join(parr,""));
          
          // iteration matches index order
          uint i=0;
          for (PathArray::iterator ii = parr.begin(); ii; ++ii, ++i)
            CHECK (parr[i] == *ii);
          CHECK (8 == i);
          
          
          // path information is automatically trimmed and filled
          parr = PathArray{nullptr,nullptr,"Θ",nullptr,nullptr,nullptr,"Φ"};
          CHECK (7 == parr.size());
          CHECK ("Θ***Φ" == join(parr,""));
          
          parr = PathArray{nullptr,"Δ",nullptr,"Ξ",nullptr,nullptr,"Φ"};
          CHECK (4 == parr.size());
          CHECK ("Δ*Ξ" == join(parr,""));
          
          parr = PathArray{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,"Ω",nullptr};
          CHECK (16 == parr.size());
          CHECK ("Ω" == join(parr,""));
          
          // index numbering starts at absolute root
          CHECK ("Ω" == *parr.begin());
          CHECK (nullptr == parr[0]);
          CHECK ("Ω"     == parr[15]);
          VERIFY_ERROR (INDEX_BOUNDS, parr[16]);
        }
      
      
      void
      verify_comparisons()
        {
          UNIMPLEMENTED ("verify comparison of UI coordinates");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (PathArray_test, "unit gui");
  
  
}} // namespace lib::test
