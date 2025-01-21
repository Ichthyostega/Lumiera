/*
  BRANCH-CASE.hpp  -  variant-like data type to capture different result types

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file branch-case.hpp
 ** A _Sum Type_ (variant) to capture values from a branched evaluation.
 ** While a _Product Type_ (tuple) holds a combination of individually typed values,
 ** a _Sum Type_ can hold any of these types, but only one at a time. Such a structure
 ** is needed when capturing results from an opaque (function-like) evaluation, which
 ** may yield different and incompatible result types, depending on circumstances.
 ** For illustration, this might be a _success_ and a _failure_ branch, but it might
 ** also be the data model from parsing a syntax with alternative branches, where
 ** each branch is bound to generate a different result object.
 ** 
 ** ## Technicalities
 ** While the basic concept is simple, a C++ implementation of this scheme poses some
 ** challenges, due to the fact that precise type information is only given at compile
 ** time. For the implementation presented here, an additional requirement was not to
 ** use any virtual (run-time) dispatch and also not to rely on heap storage. So the
 ** implementation must embody all payload data into a buffer within the BranchCase
 ** object, while storing an additional selector-mark to identify the number of the
 ** actual case represented by this instance.
 ** 
 ** The type is parametrised similar to a tuple, i.e. with a variadic type sequence.
 ** Each position in this sequence corresponds to one branch of the result system.
 ** The selected branch must be identified at instance creation, while also providing
 ** a proper initialiser for the branch's value. This design has several ramifications:
 ** - the object can not be default created, because an _empty_ state would not be valid
 ** - since type parameters can be arbitrary, we can not rely on _covariance_ for a
 **   return type; this implies that the embodied branch data value can only be
 **   accessed and retrieved when the invoker knows the branch-number at compile time.
 ** Usually this is not a serious limitation though, since code using such an evaluation
 ** must employ additional contextual knowledge anyway in order to draw any tangible
 ** conclusions. So the receiving code typically will have a branching and matching
 ** structure, based on probing the branch-selector value. Another situation might be
 ** that the data types from several branches are actually compatible, while the
 ** evaluation as such is written in a totally generic fashion and can thus not
 ** exploit such knowledge. This is typically the case when parsing a syntax;
 ** each branch might provide a regular expression match, but the receiving
 ** logic of course must know how to interpret the matching capture groups.
 ** 
 ** In any case, every possible branch for access must be somehow instantiated at
 ** compile time, because this is the only way to use the type information. This
 ** observation leads to a further access-scheme, based on a _visitor-functor._
 ** Typically this will be a generic (templated) lambda function, which must be able
 ** to really handle all the possible data types; a recursive evaluation is generated
 ** at compile time, which checks all possible branch-numbers and invokes only that
 ** branch holding the value applicable at runtime. This scheme is used as foundation
 ** to implement most of the internal functionality, notably the constructors,
 ** copy-constructors and the swap function, thereby turning BranchCase into
 ** a fully copyable, movable and assignable type (limited by the capabilities
 ** of the payload types of course).
 ** @warning This is a low-level implementation facility. The implementation itself
 **   can not check any type safety at runtime and will thus access the payload buffer
 **   blindly as instructed. When used within a proper framework however, full
 **   type safety can be achieved, based on the fact that any instance is always
 **   tied to one valid branch provided to the compiler.
 ** @see BranchCase_test
 ** @see parser.hpp "usage example"
 */


#ifndef LIB_BRANCH_CASE_H
#define LIB_BRANCH_CASE_H


#include "lib/meta/util.hpp"

#include <utility>
#include <cstddef>
#include <tuple>

namespace lib {
  
  using std::move;
  using std::forward;
  using std::decay_t;
  using std::tuple;
  
  
  namespace {// Metaprogramming helper
    
    template<typename...TYPES>
    struct _MaxBuf;
    
    template<>
    struct _MaxBuf<>
      {
        static constexpr size_t siz = 0;
        static constexpr size_t align = 0;
      };
    
    template<typename T, typename...TYPES>
    struct _MaxBuf<T,TYPES...>
      {
        static constexpr size_t siz   = std::max (sizeof(T),_MaxBuf<TYPES...>::siz);
        static constexpr size_t align = std::max (alignof(T),_MaxBuf<TYPES...>::align);
      };
  }//(End)Meta-helper
  
  
  
  /*********************************************************************//**
   * A _Sum Type_ to hold alternative results from a branched evaluation.
   * @tparam TYPES sequence of all the types corresponding to all branches
   * @remark an instance is locked into a specific branch, as designated
   *         by the index in the type sequence. The payload object is
   *         placed inline, into an opaque buffer. You need to know the
   *         branch-number in order to re-access the (typed) content.
   */
  template<typename...TYPES>
  class BranchCase
    {
    public:
      static constexpr auto TOP = sizeof...(TYPES) -1;
      static constexpr auto SIZ = _MaxBuf<TYPES...>::siz;
      
      template<size_t idx>
      using SlotType = std::tuple_element_t<idx, tuple<TYPES...>>;

    protected:
      BranchCase() = default; ///< @internal default-created state is **invalid**
      
      /** selector field to designate the chosen branch */
      size_t branch_{0};
      
      /** opaque inline storage buffer
       *  with suitable size and alignment */
      alignas(_MaxBuf<TYPES...>::align)
        std::byte buffer_[SIZ];
      
      
      template<typename TX, typename...INITS>
      TX&
      emplace (INITS&&...inits)
        {
          return * new(&buffer_) TX(forward<INITS> (inits)...);
        }
      
      template<typename TX>
      TX&
      access ()
        {
          return * std::launder (reinterpret_cast<TX*> (&buffer_[0]));
        }
      
      /** apply generic functor to the currently selected branch */
      template<size_t idx, class FUN>
      auto
      selectBranch (FUN&& fun)
        {
          if constexpr (0 < idx)
            if (branch_ < idx)
              return selectBranch<idx-1> (forward<FUN>(fun));
          return fun (get<idx>());
        }
      
      
    public:
      /**
       * Accept a _visitor-functor_ (double dispatch).
       * @note the functor or lambda must be generic and indeed
       *       able to handle every possible branch type.
       * @warning can only return single type for all branches.
       */
      template<class FUN>
      auto
      accept (FUN&& visitor)
        {
          return selectBranch<TOP> (forward<FUN> (visitor));
        }
      
     ~BranchCase()
        {
          accept ([this](auto& it)
                        { using Elm = decay_t<decltype(it)>;
                          access<Elm>().~Elm();
                        });
        }
      
      /** Standard constructor: select branch and provide initialiser */
      template<typename...INITS>
      BranchCase (size_t idx, INITS&& ...inits)
        : branch_{idx}
        {
          accept ([&,this](auto& it)
                        { using Elm = decay_t<decltype(it)>;
                          if constexpr (std::is_constructible_v<Elm,INITS...>)
                              this->emplace<Elm> (forward<INITS> (inits)...);
                        });
        }
      
      BranchCase (BranchCase const& o)
        {
          branch_ = o.branch_;
          BranchCase& unConst = const_cast<BranchCase&> (o);
          unConst.accept ([this](auto& it)
                        { using Elm = decay_t<decltype(it)>;
                          this->emplace<Elm> (it);
                        });
        }
      
      BranchCase (BranchCase && ro)
        {
          branch_ = ro.branch_;
          ro.accept ([this](auto& it)
                        { using Elm = decay_t<decltype(it)>;
                          this->emplace<Elm> (move (it));
                        });
        }
      
      friend void
      swap (BranchCase& o1, BranchCase& o2)
        {
          using std::swap;
          BranchCase tmp;
          tmp.branch_ = o1.branch_;
          o1.accept ([&](auto& it)
                        { using Elm = decay_t<decltype(it)>;
                          tmp.emplace<Elm> (move (o1.access<Elm>()));
                        });
          swap (o1.branch_,o2.branch_);
          o1.accept ([&](auto& it)
                        { using Elm = decay_t<decltype(it)>;
                          o1.emplace<Elm> (move (o2.access<Elm>()));
                        });
          o2.accept ([&](auto& it)
                        { using Elm = decay_t<decltype(it)>;
                          o2.emplace<Elm> (move (tmp.access<Elm>()));
                        });
        }
      
      BranchCase&
      operator= (BranchCase ref)
        {
          swap (*this, ref);
          return *this;
        }
      
      
      size_t
      selected()  const
        {
          return branch_;
        }
      
      /** re-access the value, using compile-time slot-index param.
       * @warning must use the correct slot-idx (unchecked!)
       */
      template<size_t idx>
      SlotType<idx>&
      get()
        {
          return access<SlotType<idx>>();
        }
    };
  
}// namespace lib
#endif/*LIB_BRANCH_CASE_H*/
