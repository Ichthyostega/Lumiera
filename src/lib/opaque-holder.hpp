/*
  OPAQUE-HOLDER.hpp  -  buffer holding an object inline while hiding the concrete type 
 
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

/** @file opaque-holder.hpp
 ** Helper allowing type erasure while holding the actual object inline.
 ** Controlling the actual storage of objects usually binds us to commit
 ** to a specific type, thus ruling out polymorphism. But sometimes, when
 ** we are able to control the maximum storage for a family of classes, we
 ** can escape this dilemma by using the type erasure pattern combined with
 ** an inline buffer holding the object of the concrete subclass. Frequently
 ** this situation arises when dealing with functor objects.
 ** 
 ** This template helps building custom objects and wrappers based on this
 ** pattern: it provides an buffer for the target objects and controls access
 ** through a two-layer capsule; while the outer container provides a neutral
 ** interface, the inner container keeps track of the actual type using a
 ** vtable. OpaqueHolder can be empty; but re-accessing the concrete
 ** object requires knowledge of the actual type, similar to boost::any
 ** (but the latter uses heap storage). 
 ** 
 ** Using this approach is bound by specific stipulations regarding the
 ** properties of the contained object and the kind of access needed.
 ** When, to the contrary, the contained types are \em not related
 ** and you need to re-discover their concrete type, then maybe
 ** a visitor or variant record might be a better solution.
 ** 
 ** @see opaque-holder-test.cpp
 ** @see function-erasure.hpp usage example
 ** @see variant.hpp 
 */


#ifndef LIB_OPAQUE_HOLDER_H
#define LIB_OPAQUE_HOLDER_H


#include "lib/error.hpp"
#include "lib/bool-checkable.hpp"



namespace lib {
  
  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
  
  
  /**
   * Inline buffer holding and owning an object while concealing the
   * concrete type. Access to the contained object is similar to a
   * smart-pointer, but the object isn't heap allocated. OpaqueHolder
   * may be created empty, which can be checked by a bool test.
   * The whole compound is copyable if and only if the contained object
   * is copyable.
   */
  template
    < class BA
    , size_t siz = sizeof(BA) 
    >
  class OpaqueHolder
    : public BoolCheckable<OpaqueHolder<BA,siz> >
    {
      char content_[siz];
      
      typedef OpaqueHolder<BA,siz> _ThisType;
      
      
      
    public:
      OpaqueHolder()
//      : created_(0)
        { }
      
      template<class SUB>
      OpaqueHolder(SUB const& obj)
//      : created_(0)
        { }
      
      ~OpaqueHolder() { clear(); }
      
      
      void
      clear ()
        {
          UNIMPLEMENTED ("delete contained object, if any");
        }
      
      
      OpaqueHolder (OpaqueHolder const& ref)
//      : created_(must_be_empty (ref))
        { }
      
      OpaqueHolder&
      operator= (OpaqueHolder const& ref)
        {
          UNIMPLEMENTED ("copy operation");
          return *this;
        }
        
      
      BA&
      operator* ()  const  // never throws
        {
          ASSERT (!empty());
          return (BA&) content_;
        }
      
      BA* 
      operator-> ()  const // never throws
        {
          ASSERT (!empty());
          return (BA*) &content_;
        }
      
      template<class SUB>
      SUB& get()  const
        {
          UNIMPLEMENTED ("downcast to concrete type");
//          return (SUB*) &content_; 
        }
      
      bool empty() const
        {
          UNIMPLEMENTED ("check for empty container");
        }
      
      bool isValid() const
        {
          UNIMPLEMENTED ("empty check and forward to contained object");
        }
      
      
    };
  
  
  
  
} // namespace lib
#endif
