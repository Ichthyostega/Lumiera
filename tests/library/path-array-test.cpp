/*
  PathArray(Test)  -  verify container abstraction for a sequence of path elements

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file path-array-test.cpp
 ** unit test \ref PathArray_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/path-array.hpp"
#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include <string>


using std::string;
using lib::Symbol;
using util::isnil;
using util::join;



namespace lib  {
namespace test {
  
  using LERR_(INDEX_BOUNDS);
  using LERR_(INVALID);
  
  using ParrT = lib::PathArray<5>;
  
  
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
          verify_boundaries();
          verify_comparisons();
        }
      
      
      void
      verify_basics()
        {
          ParrT undef;
          CHECK (isnil (undef));
          
          ParrT parr{"Γ","Δ","Θ","Ξ","Σ","Ψ","Φ","Ω"};
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
          parr = ParrT{nullptr,nullptr,"Θ",nullptr,nullptr,nullptr,"Φ"};
          CHECK (7 == parr.size());
          
          // representation is trimmed and filled
          CHECK ("Θ/*/*/*/Φ" == string(parr));
          CHECK (Symbol::EMPTY == parr[0]);
          CHECK (Symbol::EMPTY == parr[1]);
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
          ParrT parr;
          CHECK (isnil (parr));
          // path is iterable
          CHECK ("" == join(parr));
          
          parr = ParrT{"Γ","Δ","Θ","Ξ","Σ","Ψ","Φ","Ω"};
          CHECK (8 == parr.size());
          // path is iterable
          CHECK ("ΓΔΘΞΣΨΦΩ" == join(parr,""));
          
          // iteration matches index order
          uint i=0;
          for (ParrT::iterator ii = parr.begin(); ii; ++ii, ++i)
            CHECK (parr[i] == *ii);
          CHECK (8 == i);
          
          
          // path information is automatically trimmed and filled
          parr = ParrT{nullptr,nullptr,"Θ",nullptr,nullptr,nullptr,"Φ"};
          CHECK (7 == parr.size());
          CHECK ("Θ***Φ" == join(parr,""));
          
          parr = ParrT{nullptr,"Δ",nullptr,"Ξ",nullptr,nullptr,nullptr};
          CHECK (4 == parr.size());
          CHECK ("Δ*Ξ" == join(parr,""));
          
          parr = ParrT{nullptr,"Δ",nullptr,"Ξ",nullptr,nullptr,"Φ"};
          CHECK (7 == parr.size());
          CHECK ("Δ*Ξ**Φ" == join(parr,""));
          
          parr = ParrT{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,"Ω",nullptr};
          CHECK (16 == parr.size());
          CHECK ("Ω" == join(parr,""));
          
          // index numbering starts at absolute root
          CHECK ("Ω" == *parr.begin());
          CHECK (Symbol::EMPTY == parr[0]);
          CHECK (Symbol::EMPTY == parr[1]);
          CHECK (Symbol::EMPTY == parr[2]);
          CHECK ("Ω"           == parr[15]);
          VERIFY_ERROR (INDEX_BOUNDS, parr[16]);
          
          // but iteration starts with actual content
          parr = ParrT{nullptr,nullptr,"Θ","Ξ","Σ","Ψ","Φ","Ω"};
          CHECK ("Θ" == *parr.begin());
          CHECK ( 2  == parr.indexOf (*parr.begin()));
          CHECK ("Θ" == parr[2]);
          
          parr = ParrT{nullptr,nullptr,nullptr,nullptr,"Σ","Ψ","Φ","Ω"};
          CHECK ("Σ" == *parr.begin());
          CHECK ( 4  == parr.indexOf (*parr.begin()));
          CHECK ( 5  == parr.indexOf (*++parr.begin()));
          CHECK ( 6  == parr.indexOf (*++++parr.begin()));
          
          Literal some{"muck"};
          VERIFY_ERROR (INVALID, parr.indexOf (some));
        }
      
      
      /** @test cover some tricky corner cases of
       *   the bound checks and normalisation routine.
       * @remark the container used for this test has an
       *   inline chunk size of 5, which means any further
       *   elements are in heap allocated extension storage.
       *   Normalisation seamlessly surpasses that boundary.
       */
      void
      verify_boundaries()
        {
          ParrT parr;
          CHECK ("" == join(parr));
          CHECK (0 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ","Σ","Ψ","Φ","Ω"};
          CHECK ("ΓΔΘΞΣΨΦΩ" == join(parr,""));
          CHECK (8 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ","Σ","Ψ","Φ",""};
          CHECK ("ΓΔΘΞΣΨΦ" == join(parr,""));
          CHECK (7 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ","Σ","Ψ","Φ",nullptr};
          CHECK ("ΓΔΘΞΣΨΦ" == join(parr,""));
          CHECK (7 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ","Σ","Ψ",nullptr,""};
          CHECK ("ΓΔΘΞΣΨ" == join(parr,""));
          CHECK (6 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ","Σ","",nullptr,""};
          CHECK ("ΓΔΘΞΣ" == join(parr,""));
          CHECK (5 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ","Σ",nullptr,"",nullptr};
          CHECK ("ΓΔΘΞΣ" == join(parr,""));
          CHECK (5 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ","Σ",nullptr,nullptr,nullptr};
          CHECK ("ΓΔΘΞΣ" == join(parr,""));
          CHECK (5 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ","Σ",nullptr,nullptr,nullptr,""};
          CHECK ("ΓΔΘΞΣ" == join(parr,""));
          CHECK (5 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ","Σ","","",""};
          CHECK ("ΓΔΘΞΣ" == join(parr,""));
          CHECK (5 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ","","","Φ",""};
          CHECK ("ΓΔΘΞ**Φ" == join(parr,""));
          CHECK (7 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ",nullptr,"","Φ",""};
          CHECK ("ΓΔΘΞ**Φ" == join(parr,""));
          CHECK (7 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ","",nullptr,"Φ",""};
          CHECK ("ΓΔΘΞ**Φ" == join(parr,""));
          CHECK (7 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ",nullptr,nullptr,"Φ",""};
          CHECK ("ΓΔΘΞ**Φ" == join(parr,""));
          CHECK (7 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ","","","Φ",nullptr};
          CHECK ("ΓΔΘΞ**Φ" == join(parr,""));
          CHECK (7 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ","","","Φ",nullptr,""};
          CHECK ("ΓΔΘΞ**Φ" == join(parr,""));
          CHECK (7 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ","","","Φ",nullptr,"",nullptr};
          CHECK ("ΓΔΘΞ**Φ" == join(parr,""));
          CHECK (7 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ","","","Φ",nullptr,"",nullptr,""};
          CHECK ("ΓΔΘΞ**Φ" == join(parr,""));
          CHECK (7 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ","",nullptr,"",nullptr,"",nullptr,""};
          CHECK ("ΓΔΘΞ" == join(parr,""));
          CHECK (4 == parr.size());
          
          parr = ParrT{"Γ","Δ","Θ","Ξ",nullptr,"",nullptr,"",nullptr,"",nullptr};
          CHECK ("ΓΔΘΞ" == join(parr,""));
          CHECK (4 == parr.size());
          
          parr = ParrT{"","Δ","Θ","Ξ","","",""};
          CHECK ("ΔΘΞ" == join(parr,""));
          CHECK (4 == parr.size());
          CHECK ("" == parr[0]);
          
          parr = ParrT{nullptr,"Δ","Θ","Ξ","","",""};
          CHECK ("ΔΘΞ" == join(parr,""));
          CHECK (4 == parr.size());
          CHECK ("" == parr[0]);
          
          parr = ParrT{nullptr,"Δ",nullptr,"Ξ","","",""};
          CHECK ("Δ*Ξ" == join(parr,""));
          CHECK (4 == parr.size());
          CHECK ("" == parr[0]);
          CHECK ("Δ" == parr[1]);
          CHECK ("*" == parr[2]);
          CHECK ("Ξ" == parr[3]);
          
          parr = ParrT{nullptr,"",nullptr,"Ξ","","",""};
          CHECK ("Ξ" == join(parr,""));
          CHECK (4 == parr.size());
          CHECK ("" == parr[0]);
          CHECK ("" == parr[1]);
          CHECK ("" == parr[2]);
          CHECK ("Ξ" == parr[3]);
          
          parr = ParrT{nullptr,"",nullptr,"Ξ","","Ψ",""};
          CHECK ("Ξ*Ψ" == join(parr,""));
          CHECK (6 == parr.size());
          CHECK ("" == parr[0]);
          CHECK ("" == parr[1]);
          CHECK ("" == parr[2]);
          CHECK ("Ξ" == parr[3]);
          CHECK ("*" == parr[4]);
          CHECK ("Ψ" == parr[5]);
          
          parr = ParrT{nullptr,"",nullptr,"Ξ",nullptr,"Ψ",""};
          CHECK ("Ξ*Ψ" == join(parr,""));
          CHECK (6 == parr.size());
          CHECK ("" == parr[0]);
          CHECK ("" == parr[1]);
          CHECK ("" == parr[2]);
          CHECK ("Ξ" == parr[3]);
          CHECK ("*" == parr[4]);
          CHECK ("Ψ" == parr[5]);
          
          parr = ParrT{nullptr,"",nullptr,"Ξ",nullptr,nullptr,"Φ",""};
          CHECK ("Ξ**Φ" == join(parr,""));
          CHECK (7 == parr.size());
          CHECK ("" == parr[0]);
          CHECK ("" == parr[1]);
          CHECK ("" == parr[2]);
          CHECK ("Ξ" == parr[3]);
          CHECK ("*" == parr[4]);
          CHECK ("*" == parr[5]);
          CHECK ("Φ" == parr[6]);
          
          parr = ParrT{nullptr,"",nullptr,"Ξ","",nullptr,"Φ",""};
          CHECK ("Ξ**Φ" == join(parr,""));
          CHECK (7 == parr.size());
          CHECK ("" == parr[0]);
          CHECK ("" == parr[1]);
          CHECK ("" == parr[2]);
          CHECK ("Ξ" == parr[3]);
          CHECK ("*" == parr[4]);
          CHECK ("*" == parr[5]);
          CHECK ("Φ" == parr[6]);
          
          parr = ParrT{nullptr,"",nullptr,"Ξ",nullptr,"","Φ",""};
          CHECK ("Ξ**Φ" == join(parr,""));
          CHECK (7 == parr.size());
          CHECK ("" == parr[0]);
          CHECK ("" == parr[1]);
          CHECK ("" == parr[2]);
          CHECK ("Ξ" == parr[3]);
          CHECK ("*" == parr[4]);
          CHECK ("*" == parr[5]);
          CHECK ("Φ" == parr[6]);
          
          parr = ParrT{nullptr,"",nullptr,"Ξ","","","Φ",""};
          CHECK ("Ξ**Φ" == join(parr,""));
          CHECK (7 == parr.size());
          CHECK ("" == parr[0]);
          CHECK ("" == parr[1]);
          CHECK ("" == parr[2]);
          CHECK ("Ξ" == parr[3]);
          CHECK ("*" == parr[4]);
          CHECK ("*" == parr[5]);
          CHECK ("Φ" == parr[6]);
          
          parr = ParrT{"",nullptr,"",nullptr,"Σ","","Φ",""};
          CHECK ("Σ*Φ" == join(parr,""));
          CHECK (7 == parr.size());
          CHECK ("" == parr[0]);
          CHECK ("" == parr[1]);
          CHECK ("" == parr[2]);
          CHECK ("" == parr[3]);
          CHECK ("Σ" == parr[4]);
          CHECK ("*" == parr[5]);
          CHECK ("Φ" == parr[6]);
          
          parr = ParrT{"",nullptr,"",nullptr,"Σ",nullptr,"Φ",""};
          CHECK ("Σ*Φ" == join(parr,""));
          CHECK (7 == parr.size());
          CHECK ("" == parr[0]);
          CHECK ("" == parr[1]);
          CHECK ("" == parr[2]);
          CHECK ("" == parr[3]);
          CHECK ("Σ" == parr[4]);
          CHECK ("*" == parr[5]);
          CHECK ("Φ" == parr[6]);
          
          parr = ParrT{"",nullptr,"",nullptr,"","Ψ",nullptr,"Ω",""};
          CHECK ("Ψ*Ω" == join(parr,""));
          CHECK (8 == parr.size());
          CHECK ("" == parr[0]);
          CHECK ("" == parr[1]);
          CHECK ("" == parr[2]);
          CHECK ("" == parr[3]);
          CHECK ("" == parr[4]);
          CHECK ("Ψ" == parr[5]);
          CHECK ("*" == parr[6]);
          CHECK ("Ω" == parr[7]);
          
          parr = ParrT{nullptr,"",nullptr,"",nullptr,"Ψ",nullptr,"Ω",""};
          CHECK ("Ψ*Ω" == join(parr,""));
          CHECK (8 == parr.size());
          CHECK ("" == parr[0]);
          CHECK ("" == parr[1]);
          CHECK ("" == parr[2]);
          CHECK ("" == parr[3]);
          CHECK ("" == parr[4]);
          CHECK ("Ψ" == parr[5]);
          CHECK ("*" == parr[6]);
          CHECK ("Ω" == parr[7]);
          
          parr = ParrT{nullptr,"",nullptr,"",nullptr,"","Φ",nullptr,"ω",""};
          CHECK ("Φ*ω" == join(parr,""));
          CHECK (9 == parr.size());
          CHECK ("" == parr[0]);
          CHECK ("" == parr[1]);
          CHECK ("" == parr[2]);
          CHECK ("" == parr[3]);
          CHECK ("" == parr[4]);
          CHECK ("" == parr[5]);
          CHECK ("Φ" == parr[6]);
          CHECK ("*" == parr[7]);
          CHECK ("ω" == parr[8]);
          
          parr = ParrT{"",nullptr,"",nullptr,"",nullptr,"Φ",nullptr,"ω",""};
          CHECK ("Φ*ω" == join(parr,""));
          CHECK (9 == parr.size());
          CHECK ("" == parr[0]);
          CHECK ("" == parr[1]);
          CHECK ("" == parr[2]);
          CHECK ("" == parr[3]);
          CHECK ("" == parr[4]);
          CHECK ("" == parr[5]);
          CHECK ("Φ" == parr[6]);
          CHECK ("*" == parr[7]);
          CHECK ("ω" == parr[8]);
          
          parr = ParrT{"",nullptr,"",nullptr,"",nullptr,"Φ",nullptr,"ω","*"};
          CHECK ("Φ*ω" == join(parr,""));
          CHECK (9 == parr.size());
          CHECK ("" == parr[0]);
          CHECK ("" == parr[1]);
          CHECK ("" == parr[2]);
          CHECK ("" == parr[3]);
          CHECK ("" == parr[4]);
          CHECK ("" == parr[5]);
          CHECK ("Φ" == parr[6]);
          CHECK ("*" == parr[7]);
          CHECK ("ω" == parr[8]);
          
          parr = ParrT{"",nullptr,"",nullptr,"",nullptr,"Φ",nullptr,"*",""};
          CHECK ("Φ" == join(parr,""));
          CHECK (7 == parr.size());
          CHECK ("" == parr[0]);
          CHECK ("" == parr[1]);
          CHECK ("" == parr[2]);
          CHECK ("" == parr[3]);
          CHECK ("" == parr[4]);
          CHECK ("" == parr[5]);
          CHECK ("Φ" == parr[6]);
          
          parr = ParrT{"",nullptr,"",nullptr,"",nullptr,""};
          CHECK ("" == join(parr,""));
          CHECK (0 == parr.size());
        }
      
      
      /** @test verify equality comparison
       * Equality of PathArray is based on overall size, position and _normalised_ content.
       * @note especially how `"*"` might match "" or `nullptr` at corresponding positions.
       */
      void
      verify_comparisons()
        {
          CHECK (ParrT("Γ","Δ","Θ","Ξ","Σ","Ψ","Φ","Ω") == ParrT("Γ","Δ","Θ","Ξ","Σ","Ψ","Φ","Ω"));
          CHECK (ParrT("Γ","Δ","Θ","Ξ","Σ","Ψ","Φ","Ω") != ParrT("Γ","Δ","Θ","Σ","Ξ","Ψ","Φ","Ω"));
          CHECK (ParrT("Γ","Δ","Θ","Ξ","Σ","Ψ","Φ","Ω") != ParrT("Γ","Δ","Θ","Ξ","Ξ","Ψ","Φ","Ω"));
          CHECK (ParrT("Γ","Δ","Θ","Ξ","Σ","Ψ","Φ","Ω") != ParrT("" ,"Γ","Δ","Θ","Ξ","Σ","Ψ","Φ"));
          CHECK (ParrT("Γ","Δ","Θ","Ξ","Σ","Ψ","Φ","" ) != ParrT("" ,"Γ","Δ","Θ","Ξ","Σ","Ψ","Φ"));
          CHECK (ParrT("Γ","Δ","Θ","Ξ","Σ","Ψ","Φ","" ) != ParrT("" ,"Γ","Δ","Θ","Ξ","Σ","Ψ","" ));
          CHECK (ParrT("" ,"Δ","Θ","Ξ","Σ","Ψ","Φ","" ) != ParrT("*","Δ","Θ","Ξ","Σ","Ψ","Φ","" ));
          CHECK (ParrT("" ,"Δ","Θ","Ξ","Σ","Ψ","Φ","" ) != ParrT("" ,"Δ","Θ","Ξ","" ,"Σ","Ψ","Φ"));
          CHECK (ParrT("" ,"Δ","Θ","Ξ","Σ","Ψ","Φ","" ) != ParrT("" ,"Δ","Θ","Ξ","Σ***Ψ","Φ","" ));
          CHECK (ParrT("" ,"Δ","Θ","Ξ","Σ","" ,"Φ","" ) != ParrT("" ,"Δ","Θ","Ξ","*","Ψ","Φ","" ));
          CHECK (ParrT("" ,"Δ","Θ","Ξ","Σ","" ,"Φ","" ) != ParrT("" ,"Δ","Θ","Ξ","Σ","* ","Φ",""));
          CHECK (ParrT("" ,"Δ","Θ","Ξ","Σ","" ,"Φ","" ) != ParrT("" ,"Δ","Θ","Ξ","Σ","**","Φ",""));
          CHECK (ParrT("" ,"Δ","Θ","Ξ","Σ","Ψ","Φ","" ) == ParrT("" ,"Δ","Θ","Ξ","Σ","Ψ","Φ","*"));
          CHECK (ParrT("" ,"Δ","Θ","Ξ","Σ","", "Φ","" ) == ParrT("" ,"Δ","Θ","Ξ","Σ","*","Φ","" ));
          CHECK (ParrT("" ,"Δ","Θ","Ξ","Σ","Ψ","Φ","" ) == ParrT(nullptr,"Δ","Θ","Ξ","Σ","Ψ","Φ"));
          CHECK (ParrT("" ,"Δ","Θ","Ξ","Σ","Ψ","Φ","" ) == ParrT("" ,"Δ","Θ","Ξ","Σ","Ψ","Φ",nullptr));
          CHECK (ParrT("" ,"Δ","Θ","Ξ","Σ","Ψ","Φ","" ) == ParrT("" ,"Δ","Θ","Ξ","Σ","Ψ","Φ"));
          CHECK (ParrT("" ,"Δ","Θ","Ξ","" ,"Ψ","Φ","" ) == ParrT("" ,"Δ","Θ","Ξ",nullptr,"Ψ","Φ"));
          CHECK (ParrT("" ,"Δ","Θ","Ξ","*","Ψ","Φ","" ) == ParrT("" ,"Δ","Θ","Ξ",nullptr,"Ψ","Φ"));
          CHECK (ParrT("" ,"Δ","Θ","Ξ","*","Ψ","Φ","" ) == ParrT("" ,"Δ","Θ","Ξ","", "Ψ","Φ"));
          
          ParrT src{"Γ","Δ","Θ","Ξ","Σ","Ψ","Φ","Ω"};
          ParrT copy{src};
          CHECK (not isnil(copy));
          CHECK (src == copy);
          
          ParrT target {std::move (copy)};
          CHECK (src == target);
          CHECK (copy != target);
          CHECK (copy != src);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (PathArray_test, "unit stage");
  
  
}} // namespace lib::test
