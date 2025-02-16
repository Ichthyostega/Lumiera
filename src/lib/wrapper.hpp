/*
  WRAPPER.hpp  -  some smart wrapping and reference managing helper templates

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file wrapper.hpp
 ** Library implementation: smart-pointer variations, wrappers and managing holders.
 ** This is (intended to become) a loose collection of the various small helper templates
 ** for wrapping, containing, placing or handling a somewhat \em problematic other object.
 ** Mostly these are implemented to suit a specific need and then factored out later on.
 ** - ReturnRef is similar to std::reference_wrapper, but with a function-like usage.
 ** - ItemWrapper is a similar concept, but used more like a smart-ptr. Notably,
 **   a value-object is stored inline, into an embedded buffer.
 **   Furthermore, ItemWrapper can be used to level differences between values,
 **   references and pointers, as it can be instantiated with any of them, offering
 **   (almost) uniform handling in all cases (useful for building templates)
 ** - FunctionResult is the combination of ItemWrapper with a functor object
 **   to cache the function result value. It was split off into a separate
 **   header \ref wrapper-function-result.hpp to reduce include impact
 ** @remark most of this helper collection became obsolete with the evolution of the
 **   standard library — with the exception of ItermWrapper, which turned out to be
 **   very effective and is now pervasively used as part of transforming functor
 **   pipelines, to cache the result of the transforming function invocation.
 ** @see lib::TransformIter
 ** @see lib::explore
 **
 */


#ifndef LIB_WRAPPER_H
#define LIB_WRAPPER_H

#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <cstddef>
#include <utility>
#include <type_traits>


namespace lib {
namespace wrapper {
  
  using util::isSameObject;
  using LERR_(BOTTOM_VALUE);
  
  
  
  /**
   * Reference wrapper implemented as constant function,
   * returning the (fixed) reference on invocation
   */
  template<typename T>
  class ReturnRef
    {
      T& ref_;
      
    public:
      ReturnRef(T& target) : ref_(target) { }
      T& operator() ()  const { return ref_;}
    };
  
  template<typename T>
  ReturnRef<T>
  refFunction (T& target)
  {
    return ReturnRef<T> (target);
  }
  
  
  
  
  
  
  /**
   * Universal value/ref wrapper accessible similar to a pointer.
   * A copyable, assignable value object to hold a given value within
   * an embedded inline buffer. It can be default constructed and `bool`
   * evaluated to detect an empty holder. The value is retrieved through
   * a pointer-like interface, by explicit dereferentiation. (Contrast this
   * to std::ref, where the original reference is retrieved by conversion).
   * @note when the embedded value is a pointer, ItemWrapper does _not_ take
   *       ownership of and manage data the pointer is pointing to.
   * 
   * The purpose of this template is to be able to remember
   * pretty much any kind of value or pointer or reference,
   * and to subsume this handling within a single template.
   * An example would be to remember the value yielded
   * by a function, without any further assumptions
   * regarding this function.
   */
  template<typename TY>
  class ItemWrapper
    {
      
      using TY_unconst = std::remove_const_t<TY>;
      
      
      alignas(TY) mutable
        std::byte content_[sizeof(TY)];
      
      bool created_;
      
      template<typename REF>
      void
      build (REF&& ref)
        {
          new(&content_) TY{std::forward<REF> (ref)};
          created_ = true;
        }
      
      void
      discard ()
        {
          if (created_) access().~TY();
          created_ = false;
        }
      
      TY&
      access ()  const
        {
          return * std::launder (reinterpret_cast<TY*> (&content_));
        }
      
      TY_unconst&
      access_unconst()  const  ///< used to assign new buffer contents
        {
          return const_cast<TY_unconst&> (access());
        }
      
    public:
      ItemWrapper()
        : created_(false)
        { }
      
      explicit
      ItemWrapper(TY const& o)
        : created_(false)
        {
          build (o);
        }
      explicit
      ItemWrapper(TY && ro)
        : created_(false)
        {
          build (std::move(ro));
        }
      
     ~ItemWrapper()
        {
          discard();
        }
      
      
      /* == copy and assignment == */
      
      ItemWrapper (ItemWrapper const& ref)
        : created_(false)
        {
          if (ref.isValid())
            build (*ref);
        }
      ItemWrapper (ItemWrapper && rref)
        : created_(false)
        {
          if (rref.isValid())
            {
              build (std::move (*rref));
              rref.discard();
            }
        }
      
      ItemWrapper&
      operator= (ItemWrapper const& cref)
        {
          if (!cref.isValid())
            discard();
          else
            this->operator= (*cref);
          
          return *this;
        }
      ItemWrapper&
      operator= (ItemWrapper & ref)
        {
          return *this = (ItemWrapper const&)ref;
        }
      ItemWrapper&
      operator= (ItemWrapper && rref)
        {
          if (!rref.isValid())
            discard();
          else
            {
              this->operator= (std::move(*rref));
              rref.discard();
            }
          
          return *this;
        }
      
      /**
       * Emulate »assignment« by discarding and then construction of a new payload
       * @param something from which TY can be (copy/move)constructed
       * @remark allows handling »move-only« types; for the typical use case, something
       *         new is fabricated in a lambda and then moved into the ItemWrapper; thus
       *         the performance overhead of destroy/re-created is not deemed relevant.
       *         SFINAE tricks on hidden/deleted assignment operator can be unreliable.
       */
      template<typename X>
      ItemWrapper&
      operator= (X&& something)
        {
          if (!isSameObject (something, access() ))
            {
              discard();
              build (std::forward<X>(something));
            }
          return *this;
        }
      
      /** implant a default-initialised instance of the payload type */
      ItemWrapper&
      defaultInit()
        {
          discard();
          build (TY());
          return *this;
        }
      
      operator bool()  const
        {
          return isValid();
        }
      
      
      /* == value access == */
      TY&
      operator* ()  const
        {
          if (!created_)
            throw lumiera::error::State ("accessing uninitialised value/ref wrapper"
                                        , LERR_(BOTTOM_VALUE));
          return access();
        }
      
      TY*
      operator-> ()  const
        {
          return & **this;
        }
      
      bool
      isValid ()  const
        {
          return created_;
        }
      
      void
      reset ()
        {
          discard();
        }
    };
  
  
  /**
   * Specialisation of the ItemWrapper to deal with references,
   * as if they were pointer values. Allows the reference value
   * to be default constructed to ⟂ (invalid) and to be re-assigned.
   */
  template<typename TY>
  class ItemWrapper<TY &>
    {
      
      TY * content_;
      
      
    public:
      ItemWrapper()
        : content_()
        { }
      
      explicit
      ItemWrapper(TY& o)
        : content_( &o )
        { }
      
      // using default copy and assignment
      
      operator bool()  const { return isValid(); }
      
      
      /** allow to re-bind the reference */
      ItemWrapper&
      operator= (TY& otherRef)
        {
          content_ = &otherRef;
          return *this;
        }
      
      ItemWrapper&
      defaultInit()  ///< @note just reset for this specialisation
        {
          reset();
          return *this;
        }
      
      
      /* == value access == */
      TY&
      operator* ()  const
        {
          if (!content_)
            throw lumiera::error::State ("accessing uninitialised reference wrapper"
                                        , LERR_(BOTTOM_VALUE));
          return *content_;
        }
      
      bool
      isValid ()  const
        {
          return bool(content_);
        }
      
      void
      reset ()
        {
          content_ = 0;
        }
    };
  
  
  /**
   * Fallback-specialisation for `ItemWrapper<void>`.
   * @remark This can be relevant when ItemWrapper is used to capture function results,
   *         yet the given function has return type `void` and is used for side-effects.
   */
  template<>
  class ItemWrapper<void>
    {
    public:
      ItemWrapper()
        { }
      
      // using default copy and assignment
      
      operator bool()  const { return true; }
      bool isValid ()  const { return true; }
      void reset ()          { /* NOP */ }
      
      /** @warning does nothing */
      void operator*() const { /* NOP */ }
      
      ItemWrapper&
      defaultInit()  ///< no effect for this specialisation
        {
          return *this;
        }
    };
  
  
  
  /** allow equality comparison if the wrapped types are comparable */
  template<typename TY>
  inline bool
  operator== (ItemWrapper<TY> const& w1, ItemWrapper<TY> const& w2)
  {
    return (!w1 && !w2)
        || ( w1 &&  w2 && (*w1)==(*w2));
  }
  template<typename TY>
  inline bool
  operator!= (ItemWrapper<TY> const& w1, ItemWrapper<TY> const& w2)
  {
    return not (w1 == w2);
  }
  
  
}} // namespace lib::wrap
#endif /*LIB_WRAPPER_H*/
