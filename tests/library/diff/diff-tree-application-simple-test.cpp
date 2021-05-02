/*
  DiffTreeApplicationSimple(Test)  -  demonstrate the basics of tree diff representation

  Copyright (C)         Lumiera.org
    2019,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file diff-tree-application-simple-test.cpp
 ** unit test \ref DiffTreeApplicationSimple_test.
 ** Demonstrates the basic concept of reshaping structured data
 ** through a tree-diff sequence.
 */


#include "lib/test/run.hpp"
#include "lib/diff/mutation-message.hpp"
#include "lib/diff/tree-diff-application.hpp"
#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include <string>
#include <vector>

using std::string;
using std::vector;


namespace lib {
namespace diff{
namespace test{
  
  namespace {//Test fixture....
    
    // some symbolic values to be used within the diff
    const GenNode VAL_A("a"),
                  VAL_B("b"),
                  VAL_C("c"),
                  VAL_D("d");
    
    /** render the child elements as string data for test/verification */
    string
    contents (Rec const& object)
    {
      return util::join (transformIterator (object.scope()
                                           ,[](GenNode const& n) { return n.data.get<string>(); }
                                           ));
    }
    
    string
    contents (vector<string> const& strings)
    {
      return util::join (strings);
    }
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /****************************************************************************//**
   * @test Demonstration/Concept: apply a "tree diff" to reshape structured data.
   *       - the demo_one() constructs a "GenNode object",
   *         which is then mutated by applying a diff.
   *       - the demo_two() uses a STL collection (vector) as _opaque data structure,_
   *         establishes a _diff binding_ to that structure and then applies basically
   *         the same diff to mutate the target data.
   * @remarks This test is meant as introductory example to explain the meaning of
   *       the terms "diff", "diff verbs", "application", "mutation", "target data"
   *       and thus to show the basic ideas of Lumiera's »Diff Framework«. As can be
   *       expected, these examples are somewhat artificial and everything is made
   *       up as to look simple, while deliberately a lot of technical intricacies
   *       will be swept under the carpet.
   * @see DiffTreeApplication_test extended demonstration of possible diff operations
   * @see DiffComplexApplication_test handling arbitrary data structures
   * @see GenericRecord_test
   * @see GenNode_test
   * @see DiffListApplication_test
   * @see diff-tree-application.hpp
   * @see tree-diff.hpp
   * @see tree-diff-traits.hpp
   */
  class DiffTreeApplicationSimple_test
    : public Test
    , TreeDiffLanguage
    {
      
      virtual void
      run (Arg)
        {
          demo_one();
          demo_two();
        }
      
      /**
       * a change represented symbolically as »diff sequence«.
       * This is a sequence of _verbs_ to describe what should be done
       * in order to mutate the target data. This example can be read as
       * - first accept an existing element "a" as-is
       * - after that insert a new element "d" into the sequence
       * - next delete an existing element "b" from the sequence
       * - and finally accept an existing element "c" into the result
       */
      MutationMessage
      someDiff()
        {
          return { pick(VAL_A)
                 , ins (VAL_D)
                 , del (VAL_B)
                 , pick(VAL_C)
                 };
        }
      
      
      /** @test mutate a Record<GenNode> by applying the [sample diff](\ref #someDiff) */
      void
      demo_one()
        {
          Rec::Mutator subject;
          subject.scope (VAL_A, VAL_B, VAL_C);
          
          CHECK ("a, b, c" == contents(subject));
          
          DiffApplicator<Rec::Mutator>{subject}.consume (someDiff());
          
          CHECK ("a, d, c" == contents(subject));
        }
      
      
      /** @test mutate a STL collection opaquely by applying the sample diff */
      void
      demo_two()
        {
                  struct Opaque
                    : DiffMutable
                    , std::vector<string>
                    {
                      using std::vector<string>::vector;
                      
                      void
                      buildMutator (TreeMutator::Handle buff)  override
                        {
                          buff.emplace(
                            TreeMutator::build()
                              .attach (collection (*this)
                                      ));
                        }
                      
                    };
          
          
          Opaque subject{"a","b","c"};
          CHECK ("a, b, c" == contents(subject));
          
          DiffApplicator<Opaque>{subject}.consume (someDiff());
          
          CHECK ("a, d, c" == contents(subject));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DiffTreeApplicationSimple_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
