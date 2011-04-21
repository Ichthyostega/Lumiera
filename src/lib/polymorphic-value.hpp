/*
  POLYMORPHIC-VALUE.hpp  -  building opaque polymorphic value objects 

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file polymorphic-value.hpp
 ** A mechanism to allow for opaque polymorphic value objects.
 ** This template helps to overcome a problem frequently encountered in
 ** C++ programming, based on the fundamental design of C++, favouring
 ** explicit low-level control, copying of values and strict ctor-dtor pairs.
 ** Many object oriented design patterns build on polymorphism, where the
 ** actual type of an object isn't disclosed and collaborations rely on
 ** common interfaces. This doesn't mix well with the emphasis the C/C++
 ** language puts on efficient handling of small data elements as values
 ** and explicit control of the used storage; indeed several of the modern
 ** object oriented and functional programming techniques more or less
 ** assume the presence of a garbage collector or similar mechanism,
 ** so 'objects' need just to be mentioned by reference.
 ** 
 ** In C++ to employ many of the well known techniques, you're more or less
 ** bound to explicitly put the objects somewhere in hash allocated memory
 ** and then pass an interface pointer or reference into the actual algorithm.
 ** Often, this hinders a design based on constant values and small descriptor
 ** objects used inline, thus forcing into unnecessarily complex and heavyweight
 ** alternatives. While it's certainly pointless to fight the fundamental nature
 ** of the programming language, we may try to pull some (template based) trickery
 ** to make polymorphic objects fit better with the handling of small copyable
 ** value objects. Especially, C++ gives a special meaning to passing parameters
 ** as \c const& -- typically constructing an anonymous temporary object conveniently
 ** just for passing an abstraction barrier (while the optimiser can be expected to
 ** remove this barrier and the accompanying nominal copy operations altogether in
 ** the generated code). Thus, the ability to return a polymorphic object from a
 ** factory or configuration function <i>by value</i> would open a lot of
 ** straight forward design possibilities and concise formulations.
 ** 
 ** \par how to build a copyable value without knowing it's layout in detail
 ** 
 ** So the goal is to build a copyable and assignable type with value semantics,
 ** without disclosing the actual implementation and object layout at the usage site.
 ** This seemingly contradictory can be achieved, provided that
 ** - the space occupied by the actual implementation object is limited,
 **   so it can be placed as binary data into an otherwise opaque holder buffer
 ** - and the actual implementation object assists with copying and cloning
 **   itself, observing the actual implementation type and layout
 ** 
 ** @see polymorphic-value-test.cpp
 ** @see opaque-holder.hpp other similar opaque inline buffer templates
 ** @see lib::time::Mutation usage example
 */


#ifndef LIB_POLYMORPHIC_VALUE_H
#define LIB_POLYMORPHIC_VALUE_H


#include "lib/error.hpp"
//#include "lib/bool-checkable.hpp"
//#include "lib/access-casted.hpp"
//#include "lib/util.hpp"

//#include <boost/noncopyable.hpp>


namespace lib {
  
//  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
//  using util::isSameObject;
//  using util::unConst;
  
  
  namespace { // implementation helpers...
    
    
  }
  
  
  
  
  
  /**
   * Template to build polymorphic value objects.
   * Inline buffer with value semantics, yet holding and owning an object
   * while concealing the concrete type, exposing only the public interface.
   * Access to the contained object is by implicit conversion to this public
   * interface. The actual implementation object might be placed into the
   * buffer through a builder function; later, this buffer may be copied
   * and passed on without knowing the actual contained type.
   *  
   * For using PolymorphicValue, several \b assumptions need to be fulfilled
   * - any instance placed into OpaqueHolder is below the specified maximum size
   * - the caller cares for thread safety. No concurrent get calls while in mutation!
   * 
   * @warning when a create or copy-into operation fails with exception, the whole
   *          PolymorphicValue object is in undefined state and must not be used further.
   */
  template
    < class IFA                  ///< the nominal Base/Interface class for a family of types
    , size_t siz                 ///< maximum storage required for the targets to be held inline
    >
  class PolymorphicValue
    {
      mutable char buf_[siz];
      
      template<class IMP>
      IMP&
      access()  const
        {
          return reinterpret_cast<IMP&> (buf_);
        }
      
      void
      destroy()
        {
          access<IFA>().~IFA();
        }
      
//        REQUIRE (siz >= sizeof(IMP));
      
      template<class IMP>
      PolymorphicValue (IMP*)
        {
          new(&buf_) IMP();
        }
      
      template<class IMP, typename A1>
      PolymorphicValue (IMP*, A1& a1)
        {
          new(&buf_) IMP (a1);
        }
      
      template<class IMP, typename A1, typename A2>
      PolymorphicValue (IMP*, A1& a1, A2& a2)
        {
          new(&buf_) IMP (a1,a2);
        }
      
      template<class IMP, typename A1, typename A2, typename A3>
      PolymorphicValue (IMP*, A1& a1, A2& a2, A3& a3)
        {
          new(&buf_) IMP (a1,a2,a3);
        }
      
      
      class HandlingAdapter
        {
        public:
          virtual ~HandlingAdapter() { };
          virtual void cloneInto (void* targetBuffer)  const   =0;
          virtual void copyInto  (IFA& targetBase)     const   =0;
        };
      
      template<class IMP>
      class Adapter
        : public IMP
        , public HandlingAdapter
        {
          virtual void
          cloneInto (void* targetBuffer)  const
            {
              new(targetBuffer) Adapter(*this);
            }
          
          virtual void
          copyInto (IFA& targetBase)  const
            {
              REQUIRE (INSTANCEOF (Adapter, &targetBase));
              Adapter& target = static_cast<Adapter&> (targetBase);
              target = (*this);
            }
          
        public:
          /* using default copy and assignment */
          Adapter() : IMP() { }
          
          template<typename A1>
          Adapter (A1& a1) : IMP(a1) { }
          
          template<typename A1, typename A2>
          Adapter (A1& a1, A2& a2) : IMP(a1,a2) { }
          
          template<typename A1, typename A2, typename A3>
          Adapter (A1& a1, A2& a2, A3& a3) : IMP(a1,a2,a3) { }
        };
      
      
      HandlingAdapter&
      accessHandlingInterface ()  const
        {
          IFA& bufferContents = access<IFA>();
          REQUIRE (INSTANCEOF (HandlingAdapter, &bufferContents));
          HandlingAdapter& hap = dynamic_cast<HandlingAdapter&> (bufferContents); 
          return hap;
        }
      
    public:
      operator IFA& ()
        {
          return access<IFA>();
        }
      operator IFA const& ()  const
        {
          return access<IFA>();
        }
      
     ~PolymorphicValue()
        {
          destroy();
        }
      
      PolymorphicValue (PolymorphicValue const& o)
        {
          o.accessHandlingInterface().cloneInto (&buf_);
        }
      
      PolymorphicValue&
      operator= (PolymorphicValue const& o)
        {
          o.accessHandlingInterface().copyInto (this->access<IFA>());
          return *this;
        }
      
      template<class IMP>
      static PolymorphicValue
      build ()
        {
          Adapter<IMP>* type_to_build_in_buffer;
          return PolymorphicValue (type_to_build_in_buffer);
        }
      
      template<class IMP, typename A1>
      static PolymorphicValue
      build (A1& a1)
        {
          Adapter<IMP>* type_to_build_in_buffer;
          return PolymorphicValue (type_to_build_in_buffer, a1);
        }
      
      template<class IMP, typename A1, typename A2>
      static PolymorphicValue
      build (A1& a1, A2& a2)
        {
          Adapter<IMP>* type_to_build_in_buffer;
          return PolymorphicValue (type_to_build_in_buffer, a1,a2);
        }
      
      template<class IMP, typename A1, typename A2, typename A3>
      static PolymorphicValue
      build (A1& a1, A2& a2, A3& a3)
        {
          Adapter<IMP>* type_to_build_in_buffer;
          return PolymorphicValue (type_to_build_in_buffer, a1,a2,a3);
        }
      
      friend bool
      operator== (PolymorphicValue const& v1, PolymorphicValue const& v2)
      {
        return v1.access<IFA>() == v2.access<IFA>();
      }
      friend bool
      operator!= (PolymorphicValue const& v1, PolymorphicValue const& v2)
      {
        return ! (v1 == v2);
      }
    };
  
  
  
  
} // namespace lib
#endif
