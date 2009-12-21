/*
  WRAPPER.hpp  -  some smart wrapping and reference managing helper templates
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
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
 **
 ** @see lib::TransformIter
 ** 
 */


#ifndef LIB_WRAPPER_H
#define LIB_WRAPPER_H

#include "lib/error.hpp"
#include "lib/bool-checkable.hpp"
#include "lib/util.hpp"

//#include <tr1/memory>
#include <tr1/functional>     ////////////////////////TODO only because of tr1::ref_wrapper, used by AssignableRefWrapper -- can we get rid of this import?


namespace lib {
namespace wrapper {
  
//using std::tr1::shared_ptr;
//using std::tr1::weak_ptr;
  using util::unConst;
  using util::isSameObject;
  using lumiera::error::LUMIERA_ERROR_BOTTOM_VALUE;
  
  
  
  /** 
   * Extension to boost::reference_wrapper: 
   * Allows additionally to re-bind to another reference,
   * almost like a pointer. For example this allows to cache
   * results returned from an API call by reference.
   * @warning potentially dangerous 
   */
  template<typename TY>
  class AssignableRefWrapper
    : public std::tr1::reference_wrapper<TY>
    {
      typedef std::tr1::reference_wrapper<TY> RefWrapper;
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
  
  
  
  namespace impl {
    template<typename TY>
    struct ItemWrapperStorage
      {
        char content_[sizeof(TY)];
        char created_;
        
      };
  }
  
  /**
   * Universal value/ref wrapper behaving like a pointer.
   * A copyable, assignable value object, not managing ownership.
   * It can be default constructed and \c bool evaluated to detect
   * an empty holder. The value is retrieved pointer-like, by
   * explicit dereferentiation. (Contrast this to boost::ref,
   * where the original reference is retrieved by conversion)
   * 
   * The purpose of this template is to be able to remember
   * pretty much any kind of value or pointer or reference,
   * and to subsume this handling in a single template.
   * An example would be to remember the value yielded
   * by a function, without any further assumptions
   * regarding this function.
   */
  template<typename TY>
  class ItemWrapper
    : public BoolCheckable<ItemWrapper<TY> >
    {
      mutable 
      char content_[sizeof(TY)];
      bool created_;
      
      void
      build (TY const& ref)
        {
          new(&content_) TY(ref);
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
      
      
    public:
      ItemWrapper()
        : created_(false)
        { }
      
      explicit
      ItemWrapper(TY const& o)
        {
          build (o);
        }
      
     ~ItemWrapper()
        {
          discard();
        }
      
      
      /* == copy and assignment == */
     
      ItemWrapper (ItemWrapper const& ref)
        {
          if (ref.isValid())
            build (*ref);
        }
      
      ItemWrapper&
      operator= (ItemWrapper const& ref)
        {
          if (!ref.isValid())
            discard();
          else
            (*this) = (*ref);
          
          return *this;
        }
      
      template<typename X>
      ItemWrapper&
      operator= (X const& something) ///< accept anything assignable to TY
        {
          if (isSameObject (something, access() ))
            {
              if (created_)
                access() = something;
              else
                build (something);
            }
          
          return *this;
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
   * to be default constructed to \c NULL and to be re-assigned.
   */
  template<typename TY>
  class ItemWrapper<TY &>
    : public BoolCheckable<ItemWrapper<TY &> >
    {
//      mutable 
      TY * content_;
      
      
    public:
      ItemWrapper()
        : content_()
        { }
      
      explicit
      ItemWrapper(TY& o)
        : content_( &o )
        { }
      
      
       /* using default copy and assignment */
      
      /** allowing to re-bind the reference */
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
  
  
  
}} // namespace lib::wrap
#endif
