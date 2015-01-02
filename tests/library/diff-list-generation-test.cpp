/*
  DiffListGeneration(Test)  -  demonstrate list diff generation

  Copyright (C)         Lumiera.org
    2014,               Hermann Vosseler <Ichthyostega@web.de>

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


#include "lib/test/run.hpp"
#include "lib/diff/list-diff.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/itertools.hpp"
#include "lib/util.hpp"

#include <string>
#include <vector>

using lib::append_all;
using util::isnil;
using std::string;
using std::vector;
using std::move;
using std::swap;


namespace lib {
namespace diff{
  //#########################
  
  template<typename VAL>
  class IndexTable
    {
    public:
      template<class SEQ>
      IndexTable(SEQ const& seq)
        {
          UNIMPLEMENTED("build index");
        }
    };
  
  
  template<class SEQ>
  class DiffDetector
    {
      using Val = typename SEQ::value_type;
      using Idx = IndexTable<Val>;
      
      Idx refIdx_;
      SEQ const& currentData_;
      
      
      using DiffStep = typename ListDiffLanguage<Val>::DiffStep;
      
      /**
       * @internal state frame for diff detection and generation.
       * A diff generation process is built on top of an "old" reference point
       * and a "new" state of the underlying sequence. Within this reference frame,
       * an demand-driven evaluation of the differences is handed out to the client
       * as an iterator. While consuming this evaluation process, both the old and
       * the new version of the sequence will be traversed once. In case of re-orderings,
       * a nested forward lookup similar to insertion sort will look for matches in the
       * old sequence, rendering the whole evaluation quadratic in worst-case.
       */
      class DiffFrame
        {
          Idx old_;
          Idx* new_;
          
        public:
          DiffFrame(Idx& current, Idx&& refPoint)
            : old_(refPoint)
            , new_(&current)
            { }
          
          
          /* === Iteration control API for IterStateWrapper== */
          
          friend bool
          checkPoint (DiffFrame const& frame)
          {
            UNIMPLEMENTED("is the diff calculation exhausted?");
          }
          
          friend DiffStep&
          yield (DiffFrame const& frame)
          {
            REQUIRE (checkPoint (frame));
            UNIMPLEMENTED("yield the diff verb to describe the current point in diff");
          }
          
          friend void
          iterNext (DiffFrame & frame)
          {
            UNIMPLEMENTED("diff generation core: consume the current state and re-establish the invariant");
          }
        };
      
      
      
      
    public:
      explicit
      DiffDetector(SEQ const& refSeq)
        : refIdx_(refSeq)
        , currentData_(refSeq)
        { }
      
      
      /** does the current state of the underlying sequence differ
       * from the state embodied into the last reference snapshot taken?
       * @remarks will possibly evaluate and iterate the whole sequence
       */
      bool
      isChanged()  const
        {
          UNIMPLEMENTED("change detection");
        }
      
      
      /** Diff is a iterator to yield a sequence of DiffStep elements */
      using Diff = lib::IterStateWrapper<DiffStep, DiffFrame>;
      
      /** Diff generation core operation.
       * Take a snapshot of the \em current state of the underlying sequence
       * and establish a frame to find the differences to the previously captured
       * \em old state. This possible difference evaluation is embodied into a #Diff
       * iterator and handed over to the client, while the snapshot of the current state
       * becomes the new reference point from now on.
       * @return iterator to yield a sequence of DiffStep tokens, which describe the changes
       *         between the previous reference state and the current state of the sequence.
       * @note takes a new snapshot to supersede the old one, i.e. updates the DiffDetector.
       * @warning the returned iterator retains a reference to the current (new) snapshot.
       *         Any concurrent modification leads to undefined behaviour. You must not
       *         invoke #pullUpdate while another client still explores the result
       *         of an old evaluation.
       */
      Diff
      pullUpdate()
        {
          Idx mark (currentData_);
          swap (mark, refIdx_);  // mark now refers to old reference point
          return Diff(DiffFrame(refIdx_, move(mark)));
        }
    };
  
  //#########################
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
          CHECK (!detector.isChanged());
          
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
                                         , skip(a5)
                                         , ins(b4)
                                         }));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DiffListGeneration_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
