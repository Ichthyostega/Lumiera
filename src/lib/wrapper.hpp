/*
  WRAPPER.hpp  -  some smart wrapping and reference managing helper templates

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file wrapper.hpp
 ** Library implementation: smart-pointer variations, wrappers and managing holders.
 ** This is (intended to become) a loose collection of the various small helper templates
 ** for wrapping, containing, placing or handling a somewhat \em problematic other object.
 ** Mostly these are implemented to suit a specific need and then factored out later on.
 ** - AssignableRefWrapper is not used anymore as of 12/09
 ** - ItemWrapper is a similar concept, but more like a smart-ptr. Moreover,
 **   it can be instantiated with a value type, a pointer or a reference type,
 **   yielding the same behaviour in all cases (useful for building templates)
 ** - FunctionResult is the combination of ItemWrapper with a functor object
 **   to cache the function result value.
 ** 
 ** @see lib::TransformIter
 ** 
 ** @todo 2017 consider to split off the FunctionResult into a dedicated header to reduce includes
 ** 
 */


#ifndef LIB_WRAPPER_H
#define LIB_WRAPPER_H

#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/function-closure.hpp"
#include "lib/meta/util.hpp"
#include "lib/util.hpp"

#include <functional>


namespace lib {
namespace wrapper {
  
  using util::unConst;
  using util::isSameObject;
  using lib::meta::_Fun;
  using lumiera::error::LUMIERA_ERROR_BOTTOM_VALUE;
  
  using std::function;
  
  
  /**
   * Extension to std::reference_wrapper:
   * Allows additionally to re-bind to another reference,
   * almost like a pointer. Helpful for building templates.
   * @warning potentially dangerous
   */
  template<typename TY>
  class AssignableRefWrapper
    : public std::reference_wrapper<TY>
    {
      typedef std::reference_wrapper<TY> RefWrapper;
    public:
      
      explicit AssignableRefWrapper(TY& ref)
        : RefWrapper(ref)
        { }
      
      AssignableRefWrapper&
      operator= (RefWrapper const& o)
        {
          RefWrapper::operator= (o);
          return *this;
        }
      
      AssignableRefWrapper&
      operator= (TY& newRef)
        {
          (*this) = RefWrapper(newRef);
          return *this;
        }
    };
  
  
  
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
   * Universal value/ref wrapper behaving like a pointer.
   * A copyable, assignable value object, not managing ownership.
   * It can be default constructed and `bool` evaluated to detect
   * an empty holder. The value is retrieved pointer-like, by
   * explicit dereferentiation. (Contrast this to std::ref,
   * where the original reference is retrieved by conversion)
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
      
      using TY_unconst = typename meta::UnConst<TY>::Type ;
      
      
      mutable
      char content_[sizeof(TY)];
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
          return reinterpret_cast<TY&> (content_);
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
        {
          build (o);
        }
      explicit
      ItemWrapper(TY && ro)
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
      
      template<typename X>
      ItemWrapper&
      operator= (X&& something) ///< accept anything assignable to TY
        {
          if (!isSameObject (something, access() ))
            {
              if (created_)
                access() = std::forward<X>(something);
              else
                build (std::forward<X>(something));
            }
          
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
                                        , LUMIERA_ERROR_BOTTOM_VALUE);
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
   * to be default constructed to `NULL` and to be re-assigned.
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
      
      
      
      /* == value access == */
      TY&
      operator* ()  const
        {
          if (!content_)
            throw lumiera::error::State ("accessing uninitialised reference wrapper"
                                        , LUMIERA_ERROR_BOTTOM_VALUE);
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
  
  
  
  
  /**
   * Extension of ItemWrapper: a function remembering
   * the result of the last invocation. Initially, the "value"
   * is bottom (undefined, NIL), until the function is invoked
   * for the first time. After that, the result of the last
   * invocation can be accessed by `operator* ()`
   * 
   * @note non-copyable. (removing this limitation would
   *       require a much more expensive implementation,
   *       by implementing operator() ourselves)
   */
  template<typename SIG>
  class FunctionResult
    : public function<SIG>
    , util::NonCopyable
    {
      using Res = typename _Fun<SIG>::Ret;
      using ResWrapper = ItemWrapper<Res>;
      
      ResWrapper lastResult_;
      
      
      Res
      captureResult (Res res)
        {
          lastResult_ = res;
          return res;
        }
      
    public:
      /** default ctor yields an object
       *  locked to \em invalid state */
      FunctionResult () { }
      
      /** Create result-remembering functor
       *  by binding the given function. Explanation:
       *  - `*this` is a _function_
       *  - initially it is defined as invalid
       *  - then we build the function composition of
       *    the target function, and a function storing
       *    the result value into the ResWrapper member
       *  - define ourselves by assigning the resulting
       *    composite function
       */
      template<typename FUN>
      FunctionResult (FUN targetFunction)
        {
          using std::bind;
          using std::placeholders::_1;
          using lib::meta::func::chained;
                                                      // note: binding "this" mandates noncopyable
          function<Res(Res)> doCaptureResult  = bind (&FunctionResult::captureResult, this, _1 );
          function<SIG> chainedWithResCapture = chained (targetFunction, doCaptureResult);
          
          function<SIG>::operator= (chainedWithResCapture); // define the function (baseclass)
        }
      
      
      Res& operator*() const { return *lastResult_; }
      bool isValid ()  const { return lastResult_.isValid(); }
     
      explicit
      operator bool()  const { return isValid(); }
    };
  
  
}} // namespace lib::wrap
#endif
