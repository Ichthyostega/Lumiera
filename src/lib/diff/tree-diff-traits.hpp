/*
  TREE-DIFF-TRAITS.hpp  -  definitions to control tree mutator binding

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

*/


/** @file tree-diff-traits.hpp
 ** Definitions and Properties to guide automated tree mutator binding.
 ** Decision how to access the target structure and how to construct
 ** a suitable TreeMutator as attached to this opaque target data.
 ** 
 ** @todo this is WIP as of 8/2016
 ** 
 ** @see DiffVirtualisedApplication_test
 ** @see DiffTreeApplication_test
 ** @see DiffListApplication_test
 ** @see GenNodeBasic_test
 ** @see tree-diff.hpp
 ** 
 */


#ifndef LIB_DIFF_TREE_DIFF_TRAITS_H
#define LIB_DIFF_TREE_DIFF_TRAITS_H


#include "lib/diff/tree-mutator.hpp"
#include "lib/diff/diff-mutable.hpp"
#include "lib/util.hpp"

#include <utility>
#include <stack>

namespace lib {
namespace diff{
  
  
  /* ======= Hints / Heuristics for the required TreeMutator buffer size ======= */
  
  /**
   * Heuristics to guide the allocation for nested TreeMutator.
   * When applying a structural (tree) diff, the (otherwise undisclosed)
   * target data structure needs to supply a TreeMutator implementation
   * properly wired to the internal opaque data elements. Typically, this
   * custom TreeMutator relies on several lambdas and closures, which
   * require a variable and hard to guess amount of storage for back pointers
   * and embedded parametrisation. More so, when the diff application opens
   * nested scopes within the target data. The TreeDiffMutatorBinding relies
   * on a (likewise opaque) ScopeManager implementation to maintain a stack
   * of heap allocated buffers, where the mentioned nested TreeMutator
   * implementations can be built and operated during the mutation process.
   * 
   * The default for buffer dimensions includes a safety margin and is thus
   * quite expensive -- even though this is just a temporary working buffer.
   * Thus we offer a hook for explicit or partial specialisations to control
   * the very common cases known to work with smaller buffer sizes.
   * 
   * \par future extensions
   * We might consider to make this system dynamic, in case buffer allocation
   * for tree diff application becomes an issue in general. We might then guard
   * the whole diff application location with try-catch blocks and allow thus
   * for learning the right setting at runtime; obviously we'd then also have to
   * memorise our findings somehow within the dynamic application configuration.
   * 
   * @see tree-diff-application.hpp
   * @see DiffVirtualisedApplication_test
   */
  template<class TAR>
  struct TreeMutatorSizeTraits
    {
      enum { siz = 200 };
    };
  
  
  
  
  
  /* ======= derive a TreeMutator binding for a given opaque data structure ======= */
  
  
  using meta::enable_if;
  using meta::Yes_t;
  using meta::No_t;
  using std::is_same;
  
  /**
   * helper to detect presence of a
   * TreeMutator builder function
   */
  template<typename T>
  class exposes_MutatorBuilder
    {
      
      META_DETECT_FUNCTION (void, buildMutator, (TreeMutator::Handle));
      
    public:
      enum{ value = HasFunSig_buildMutator<T>::value
          };
    };
  
  
  
  
  
  template<class TAR, typename SEL =void>
  struct TreeDiffTraits
    : std::false_type
    {
      static_assert (!sizeof(TAR), "TreeDiffTraits: Unable to access or build a TreeMutator for this target data.");
    };
  
  template<class TAR>
  struct TreeDiffTraits<TAR, enable_if<is_same<TAR, DiffMutable>>>
    : std::true_type
    {
      using Ret = DiffMutable&;
    };
  
  template<class TAR>
  struct TreeDiffTraits<TAR, enable_if<exposes_MutatorBuilder<TAR>>>
    : std::true_type
    {
      class Wrapper
        : public DiffMutable
        {
          TAR& subject_;
          
          /** implement the TreeMutator interface,
           *  by forwarding to a known implementation function
           *  on the wrapped target data type */
          virtual void
          buildMutator (TreeMutator::Handle handle)
            {
              subject_.buildMutator (handle);
            }
          
        public:
          Wrapper(TAR& subj)
            : subject_(subj)
            { }
        };
      
      using Ret = Wrapper;
    };
  
  template<class TAR>
  auto
  mutatorBinding (TAR& subject) -> typename TreeDiffTraits<TAR>::Ret
  {
     using Wrapper = typename TreeDiffTraits<TAR>::Ret;
     return Wrapper{subject};
  }
  
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_DIFF_TRAITS_H*/
