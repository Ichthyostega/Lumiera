/*
  FUN-INLINE.hpp  -  abstract functor with local delegate

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file fun-inline.hpp
 ** A generic abstracted functor with the implementation stored locally.
 ** This template is an implementation building block and allows to create
 ** an abstraction barrier, similar to what std::function does. In contrast
 ** to the latter however, FunInline places the actual implementation delegate
 ** into an embedded opaque buffer, never using any heap memory.
 ** 
 ** @todo WIP-WIP 4/2026 this is more like a conceptual draft,
 **       not sure where this design exercise will lead to.
 ** @warning after investigating the implementation technique to some degree,
 **       it became clear that **copy support** is the greatest challenge.
 **       All »the rest« might be surprisingly simple to implement, yet
 **       the demand for a movable / copyable value-object effectively
 **       translates into something like a VTable — and the result
 **       would be similar to lib::PolymorphicValue — just with
 **       some special binding to the function operator.
 **       For this reason, FunInline is now declared to be util::NonCopyable
 ** @todo 4/2026 I've finally got this draft hammered out.
 **       It is far from what I wanted to achieve...
 **       Notably a second function pointer is used
 **       That is ugly, but it does the job.
 ** 
 ** @see FunInline_test
 ** 
 */


#ifndef LIB_META_FUN_INLINE_H_
#define LIB_META_FUN_INLINE_H_


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/typeseq-util.hpp"
#include "lib/meta/variadic-helper.hpp"

#include <utility>


namespace lib {
namespace meta{
  namespace err = lumiera::error;
  
  using std::forward;
  
  namespace test { class FunInline_test; }
  
  /**
   * An opaque front-end for a functor stored inline.
   * The type of the implementation delegate is erased
   * from the type, retaining only the generic signature.
   * @note this wrapper object is defined as NonCopyable,
   *       because anything else would require a VTable,
   *       and raise serious concerns regarding identity.
   * @warning deliberately **not threadsafe**
   * @see FunInline_test
   */
  template<class SIG
          ,size_t siz
          ,typename ALI = int64_t
          >
  class FunInline
    {
      static_assert (!sizeof(SIG), "function signature required");
    };
  
  
  template<typename RET, typename...ARGS
          ,size_t siz
          ,typename ALI
          >
  class FunInline<RET(ARGS...), siz, ALI>
    : util::NonCopyable
    {
      using _F = _Fun<RET(ARGS...)>;
      static_assert (_F{}, "function signature required");
      
      using Sig   = _F::Sig;
      using Ret   = _F::Ret;
      using Args  = _F::Args;
      using RArgs = ElmTypes<Args>::template Apply<std::add_rvalue_reference_t>;
      using IArgs = Prepend<void*, RArgs>::Seq;
      using ISig  = BuildFunType<Ret, IArgs>::Sig;
      using Dtor  = void(void*);
      
      /** virtualised delegation through function pointer(s) */
      ISig* invoker_{nullptr};
      Dtor* destroy_{nullptr};
      
      /** opaque storage buffer to emplace the functor delegate */
      alignas(ALI) mutable
        std::byte buf_[siz];
      
      
      /**
       * The Invoker template is instantiated with the type of the concrete
       * delegate functor (which resides in the opaque storage buffer).
       * The purpose of this template is to cause generation of a static
       * delegation function that can be invoked through the #invoker_ pointer.
       * In the typical usage situation, the delegate type \a FUNC is implemented
       * mostly with inline functions, and thus the bulk of the actual implementation
       * code will be emitted into this static delegation function. The template
       * \ref InvokerBuilder provides the blueprint for this code generation.
       */
      template<class FUN>
      struct Invoker
        {
          static Ret
          dispatch (void* instance, ARGS&&...args)
            {
              return functor(instance) (forward<ARGS> (args)...);
            }
          
          static void
          destroy (void* instance)
            {
              if (instance)
                functor(instance).~FUN();
            }
          
          static FUN&
          functor (void* instance)
            {
              REQUIRE (instance);
              return * static_cast<FUN*> (instance);
            }
          
          using Sig = _F::Sig;
          using ISig = decltype(dispatch);
        };
      
      /** allow the Unit test to poke into internals */
      friend class test::FunInline_test;
      
      
      
    public:
     ~FunInline(){ destroy(); }
      
      FunInline()  = default;
      
      template<class FUNC>
      FunInline (FUNC&& functor)
        {
          using Functor = std::decay_t<FUNC>;
          emplace<Functor> (forward<FUNC> (functor));
          activate<Functor>();
        }
      
      // NonCopyable
      
      
      operator bool()  const
        {
          return bool(destroy_) and bool(invoker_);
        }
      
      
      /* == Functor interface == */
      
      Ret
      operator() (ARGS&&... args)
        {
          if (not invoker_)
            throw err::State{"Attempt to invoke inactive function."};
          
          return (*invoker_) (&buf_, forward<ARGS> (args)...);
        }
      
      
    private: /* == Instance management == */
      
      void
      destroy()
        {
          if (destroy_)
            (*destroy_) (&buf_);
          destroy_ = nullptr;
          invoker_ = nullptr;
        }
      
      template<class FUNC>
      void
      activate()
        {
          ASSERT_VALID_SIGNATURE (FUNC, Sig);
          REQUIRE (!destroy_ and !invoker_, "Lifecycle logic broken");
          
          destroy_ = Invoker<FUNC>::destroy;
          invoker_ = Invoker<FUNC>::dispatch;
        }
      
      /** Abbreviation for placement new */
      template<class TY, typename...INIT>
      TY&
      emplace (INIT&& ...args)
        {
          static_assert (siz >= sizeof(TY),           "Embedded storage buffer too small");
          static_assert (alignof(ALI) >= alignof(TY), "Alignment requirements not fulfilled");
          
          destroy();
          try {
              return *new(&buf_) TY {std::forward<INIT> (args)...};
            }
          catch (...)
            {
              ENSURE (not destroy_);
              throw;
            }
        }
      
      /// @todo unused ... not required?
      template<typename XXX>
      XXX&
      accessAs()  const
        {
          return * std::launder (reinterpret_cast<XXX*> (&buf_));
        }
    };
  
  
}}// namespace lib::meta
#endif /*LIB_META_FUN_INLINE_H_*/
