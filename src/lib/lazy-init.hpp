/*
  LAZY-INIT.hpp  -  a self-initialising functor

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file lazy-init.hpp
 ** Building block to allow delayed initialisation of infrastructure tied to a functor.
 ** This solution is packaged as a mix-in template and engages a hidden mechanism with
 ** considerable trickery. It attempts to solve a problem arising notoriously when building
 ** elaborate processing by composing functions and user-provided configuration lambdas;
 ** the very point of this construction style is to tap into internal context to involve
 ** deep details of the implementation without the need to represent these as structures
 ** on API level. Unfortunately this has the consequence that capture-by-reference is
 ** all over the place, breeding instability. The only solution to defeat this instability
 ** is to lock an enclosing implementation scope into a fixed memory location, which boils
 ** down to using non-copyable classes. This solution may be in conflict to the intended
 ** use, especially when building DSLs, configuration frameworks or symbolic processing,
 ** where entities are _value like_ from a semantic point of view. The solution pursued
 ** here is to define some linkage for operational state, which allows to lock a scope
 ** to a fixed memory location. Assuming that a typical usage scenario will first
 ** require setup, then proceed to processing, this solution attempts to tie
 ** the usage restrictions to the lifecycle — hopefully hiding the concern
 ** from users sight altogether.
 ** 
 ** # Initialisation mechanism
 ** This mix-in assumes that there is a function somewhere, which activates the actual
 ** processing, and this processing requires initialisation to be performed reliably
 ** before first use. Thus, a _»trojan functor«_ is placed into this work-function,
 ** with the goal to activate a „trap“ on first use. This allows to invoke the actual
 ** initialisation, which is also configured as a functor, and which is the only part
 ** the client must provide actively, to activate the mechanism. Several initialisation
 ** steps can be attached consecutively, and will later be triggered in sequence.
 ** 
 ** There is one _gory detail_ however: the initialisation hook needs the actual instance
 ** pointer valid *at the time of actual initialisation*. And since initialisation shall
 ** be performed automatically, the trap mechanism needs a way to derive this location,
 ** relying on minimal knowledge only. This challenge can only be overcome by assuming
 ** that the »trojan functor« itself is stored somehow embedded into the target object
 ** to be initialised. If there is a fixed distance relation in memory, then the target
 ** can be derived from the self-position of the functor; if this assumption is broken
 ** however, memory corruption and SEGFAULT might ensue. These assumptions are covered
 ** by an assertion and unit tests; as long as the function and the LazyInit instance
 ** are arranged in a fixed memory layout, this scheme should work. Do not place one
 ** or the other into a virtual base class though.
 ** 
 ** @todo 11/2023 at the moment I am just desperately trying to get a bye-product of my
 **       main effort into usable shape and salvage an design idea that sounded clever
 **       on first thought. I am fully aware that »lazy initialisation« is something
 **       much more generic, but I am also aware of the potential of the solution
 **       coded here. Thus I'll claim that generic component name, assuming that
 **       time will tell if we need a more generic framework to serve this
 **       purpose eventually....
 ** @see LazyInit_test
 ** @see lib::RandomDraw usage example
 ** @see vault::gear::TestChainLoad::Rule where this setup matters
 */


#ifndef LIB_LAZY_INIT_H
#define LIB_LAZY_INIT_H


#include "lib/error.h"
#include "lib/meta/function.hpp"
#include "lib/opaque-holder.hpp"
#include "lib/util.hpp"

#include <functional>
#include <utility>
#include <memory>


namespace lib {
  namespace err = lumiera::error;
  
  using lib::meta::_Fun;
  using lib::meta::_FunArg;
  using lib::meta::has_Sig;
  using util::unConst;
  using std::function;
  using std::forward;
  using std::move;
  
  using RawAddr = void const*;
  
  
  namespace {// the anonymous namespace of horrors...
    
    inline ptrdiff_t
    captureRawAddrOffset (RawAddr anchor, RawAddr subject)
    {
      // Dear Mr.Compiler, please get out of my way.
      // I just genuinely want to shoot myself into my foot...
      char* anchorAddr  = reinterpret_cast<char*> (unConst(anchor));
      char* subjectAddr = reinterpret_cast<char*> (unConst(subject));
      return subjectAddr - anchorAddr;
    }
    
    template<class TAR>
    inline static TAR*
    relocate (RawAddr anchor, ptrdiff_t offset)
    {
      char* anchorAddr = reinterpret_cast<char*> (unConst(anchor));
      char* adjusted   = anchorAddr + offset;
      void* rawTarget  = reinterpret_cast<void*> (adjusted);
      return static_cast<TAR*> (rawTarget);
    }
    
    
    /**
     * @internal *implementation defined* : offset of a payload placed directly
     *  into a std::function when the _small object optimisation_ applies.
     * @warning relies on implementation behaviour not guaranteed by the standard,
     *          while supported by all known implementations today. This is
     *          exploited as a trick to allow for automatic late initialisation
     *          in a situation, were a functor needs to capture references.
     */
    const ptrdiff_t FUNCTOR_PAYLOAD_OFFSET =
      []{
          size_t slot{42};
          std::function<RawAddr(void)> probe = [slot]{ return RawAddr(&slot); };
          RawAddr functor = &probe;
          RawAddr payload = probe();
          if (not util::isCloseBy(functor, payload))
            throw err::Fatal{"Unable to use lib::LazyInit because std::function does not "
                             "apply small-object optimisation with inline storage."};
          return captureRawAddrOffset (functor,payload);
        }();
    //
  }//(End)low-level manipulations
  
  
  
  
  /**
   * »Trojan Function« builder.
   * Generates a Lambda to invoke a delegate for the actual computation.
   * On invocation, the current storage location of the λ is determined.
   */
  template<class SIG>
  class TrojanFun
    {
      template<class DEL, typename RET, typename... ARGS>
      static auto
      buildTrapActivator (DEL* delegate, _Fun<RET(ARGS...)>)
        {
          return [delegate]
                 (ARGS ...args) -> RET
                    {
                      auto currLocation = &delegate;
                      auto& functor = (*delegate) (currLocation);
                      //
                      return functor (forward<ARGS> (args)...);
                    };
        }
    public:
      
      /**
       * Invocation: build a Lambda to activate the »Trap«
       * and then to forward the invocation to the actual
       * function, which should have been initialised
       * by the delegate invoked.
       * @param delegate a functor object to forward invocation;
       *        the delegate must return a reference to the
       *        actual function implementation to invoke.
       *        Must be heap-allocated.
       * @return a lightweight lambda usable as trigger.
       */
      template<class DEL>
      static auto
      generateTrap (DEL* delegate)
        {
          static_assert (_Fun<DEL>(), "Delegate must be function-like");
          using Ret = typename _Fun<DEL>::Ret;
          static_assert (_Fun<Ret>(), "Result from invoking delegate must also be function-like");
          static_assert (has_Sig<Ret, SIG>(), "Result from delegate must expose target signature");
          
          REQUIRE (delegate);
          
          return buildTrapActivator (delegate, _Fun<SIG>());
        }
    };
  
  
  
  
  
  struct EmptyBase { };
  
  /**************************************************************//**
   * Mix-in for lazy/delayed initialisation of an embedded functor.
   * This allows to keep the overall object (initially) copyable,
   * while later preventing copy once the functor was »engaged«.
   * Initially, only a »trap« is installed into the functor,
   * invoking an initialisation closure on first use.
   */
  template<class PAR =EmptyBase>
  class LazyInit
    : public PAR
    {
      template<class SIG>
      using DelegateType = std::function<std::function<SIG>&(RawAddr)>;
      
      using PlaceholderType = DelegateType<void(void)>;
      using HeapStorage = InPlaceBuffer<PlaceholderType>;
      using PendingInit = std::shared_ptr<HeapStorage>;
      
      /** manage heap storage for a pending initialisation closure */
      PendingInit pendingInit_;
      
      
      PendingInit const&
      __trapLocked (PendingInit const& init)
        {
          if (not init)
            throw err::State{"Component was already configured with a processing function, "
                             "which binds into a fixed object location. It can not be moved anymore."
                            , err::LUMIERA_ERROR_LIFECYCLE};
          return init;
        }
      
      PendingInit&&
      __trapLocked (PendingInit && init)
        {
          if (not init)
            throw err::State{"Component was already configured with a processing function, "
                             "which binds into a fixed object location. It can not be moved anymore."
                            , err::LUMIERA_ERROR_LIFECYCLE};
          return move (init);
        }
      
      
      
    protected:
      struct MarkDisabled{};
      
      /** @internal allows derived classes to leave the initialiser deliberately disabled */
      template<typename...ARGS>
      LazyInit (MarkDisabled, ARGS&& ...parentCtorArgs)
        : PAR(forward<ARGS> (parentCtorArgs)...)
        , pendingInit_{}
        { }
      
      
    public:
      /** prepare an initialiser to be activated on first use */
      template<class SIG, class INI, typename...ARGS>
      LazyInit (std::function<SIG>& targetFunctor, INI&& initialiser, ARGS&& ...parentCtorArgs)
        : PAR(forward<ARGS> (parentCtorArgs)...)
        , pendingInit_{}
        { 
          installInitialiser (targetFunctor, forward<INI> (initialiser));
        }
      
      
      LazyInit (LazyInit const& ref)
        : PAR{ref}
        , pendingInit_{__trapLocked (ref.pendingInit_)}
        { }
      
      LazyInit (LazyInit && rref)
        : PAR{move (rref)}
        , pendingInit_{__trapLocked (move(rref.pendingInit_))}
        { }
      
      LazyInit&
      operator= (LazyInit const& ref)
        {
          if (not util::isSameObject (ref, *this))
            {
              PAR::operator= (ref);
              pendingInit_ = __trapLocked (ref.pendingInit_);
            }
          return *this;
        }
      
      LazyInit&
      operator= (LazyInit && rref)
        {
          if (not util::isSameObject (rref, *this))
            {
              PAR::operator= (move (rref));
              pendingInit_ = __trapLocked (move (rref.pendingInit_));
            }
          return *this;
        }
      
      
      bool
      isInit()  const
        {
          return not pendingInit_;
        }
      
      template<class SIG>
      void
      installEmptyInitialiser()
        {
          pendingInit_.reset (new HeapStorage{emptyInitialiser<SIG>()});
        }
      
      template<class SIG, class INI>
      void
      installInitialiser (std::function<SIG>& targetFunctor, INI&& initialiser)
        {
          pendingInit_ = prepareInitialiser (targetFunctor, forward<INI> (initialiser));
        }
      
      
    private: /* ========== setup of the initialisation mechanism ========== */
      template<class SIG>
      DelegateType<SIG>
      emptyInitialiser()
        {
          using TargetFun = std::function<SIG>;
          return DelegateType<SIG>([disabledFunctor = TargetFun()]
                                   (RawAddr) -> TargetFun&
                                     {
                                       return disabledFunctor;
                                     });
        }
      
      template<class SIG, class INI>
      PendingInit
      prepareInitialiser (std::function<SIG>& targetFunctor, INI&& initialiser)
        {
          if (isInit() and targetFunctor)
            {// object is already »engaged« — no need to delay init
              using ExpectedArg = _FunArg<INI>;
              initialiser (static_cast<ExpectedArg> (this));
              return PendingInit(); // keep engaged; no pending init
            }
          // else: prepare delayed init...
          PendingInit storageHandle{
            new HeapStorage{
              buildInitialiserDelegate (targetFunctor, forward<INI> (initialiser))}};
           // place a »Trojan« into the target functor to trigger initialisation on invocation...
          targetFunctor = TrojanFun<SIG>::generateTrap (getPointerToDelegate<SIG> (*storageHandle));
          return storageHandle;
        }
      
      template<class SIG>
      static DelegateType<SIG>*
      getPointerToDelegate (HeapStorage& buffer)
        {
          return reinterpret_cast<DelegateType<SIG>*> (&buffer);
        }
      
      template<class SIG>
      static std::function<SIG>
      maybeInvoke (PendingInit const& pendingInit, RawAddr location)
        {
          if (not pendingInit) // no pending init -> empty TargetFun
            return std::function<SIG>();
          auto* pendingDelegate = getPointerToDelegate<SIG>(*pendingInit);
          return (*pendingDelegate) (location); // invoke to create new TargetFun
        }
      
      template<class SIG, class INI>
      DelegateType<SIG>
      buildInitialiserDelegate (std::function<SIG>& targetFunctor, INI&& initialiser)
        {
          using TargetFun = std::function<SIG>;
          using ExpectedArg = _FunArg<INI>;
          return DelegateType<SIG>{
                     [performInit = forward<INI> (initialiser)
                     ,previousInit = move (pendingInit_)
                     ,targetOffset = captureRawAddrOffset (this, &targetFunctor)]
                     (RawAddr location) -> TargetFun&
                        {// apply known offset backwards to find current location of the host object
                          TargetFun* target = relocate<TargetFun> (location, -FUNCTOR_PAYLOAD_OFFSET);
                          LazyInit* self = relocate<LazyInit> (target, -targetOffset);
                          REQUIRE (self);
                              // tie storage to this (possibly recursive) call
                          auto storageHandle = move(self->pendingInit_);
                            // setup  target as it would be with eager init
                          (*target) = maybeInvoke<SIG> (previousInit, location);
                          // invoke init, possibly downcast to derived *self
                          performInit (static_cast<ExpectedArg> (self));
                          return *target; // back to the »Trojan« to yield first result
                        }};
        }
    };
  
  
  
} // namespace lib
#endif /*LIB_LAZY_INIT_H*/
