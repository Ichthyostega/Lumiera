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
