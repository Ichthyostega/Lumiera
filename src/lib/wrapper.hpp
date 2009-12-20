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

//#include "lib/bool-checkable.hpp"

//#include <tr1/memory>


namespace lib {
namespace wrap {
  
//using std::tr1::shared_ptr;
//using std::tr1::weak_ptr;
  
  
  
  /** 
   * Extension to boost::reference_wrapper: 
   * Allows additionally to re-bind to another reference,
   * almost like a pointer. For example this allows to cache
   * results returned from an API call by reference.
   * @warning potentially dangerous 
   */
  template<typename TY>
  class AssignableRefWrapper
    : public boost::reference_wrapper<TY>
    {
      typedef boost::reference_wrapper<TY> RefWrapper;
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
      typedef typename impl::ItemWrapperStorage<TY> Item;
      
      Item item_;
      
    public:
      ItemWrapper()
        : item_()
        { }
      
      explicit
      ItemWrapper(TY& o)
        : item_(o)
        { }
      
      //note: using default copy ctor and assignment operator!
      
      template<typename X>
      ItemWrapper&
      operator= (X const& something) ///< accept anything assignable to TY
        {
          item_ = something;
        }
      
      /* == value access == */
      TY&
      operator* ()
        {
          return item_;
        }
      
      bool
      isValid ()  const
        {
          return item_.isValid();   
        }
      
      void
      reset ()
        {
          item_.clear();
        }
    };
  
  
  
}} // namespace lib::wrap
#endif
