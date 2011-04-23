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
 ** C++ programming, based on the fundamental design of C++, which favours
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
 ** bound to explicitly put the objects somewhere in heap allocated memory
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
 ** the generated code). Consequently the ability to return a polymorphic object
 ** from a factory or configuration function <i>by value</i> would open a lot of
 ** straight forward design possibilities and concise formulations.
 ** 
 ** \par how to build a copyable value without knowing it's layout in detail
 ** 
 ** So the goal is to build a copyable and assignable type with value semantics,
 ** without disclosing the actual implementation and object layout at the usage site.
 ** This seemingly contradictory goal can be achieved, provided that
 ** - the space occupied by the actual implementation object is limited,
 **   so it can be placed as binary data into an otherwise opaque holder buffer
 ** - and the actual implementation object assists with copying and cloning
 **   itself, observing the actual implementation data layout
 ** 
 ** The PolymorphicValue template implements this idea, by exposing an copyable
 ** container with value semantics to the client code. On instantiation, a common
 ** base interface for the actual value objects needs to be provided; the resulting
 ** instance will be automatically convertible to this interface. Obviously this
 ** common interface must be an ABC or at least contain some virtual functions.
 ** Moreover, the PolymorphicValue container provides static builder functions,
 ** allowing to place a concrete instance of a subclass into the content buffer.
 ** After construction, the actual type of this instance will be forgotten
 ** (``type erasure''), but because the embedded vtable, on access the
 ** proper implementation functions will be invoked.
 ** 
 ** Expanding on that pattern, the copying and cloning operations of the whole
 ** container can be implemented by forwarding to appropriate virtual functions
 ** on the embedded payload (implementation) object -- the concrete implementation
 ** of these virtual functions can be assumed to know the real type and thus be
 ** able to invoke the correct copy ctor or assignment operator. For this to
 ** work, the interface needs to expose those copy and clone operations somehow
 ** as virtual functions. There are two alternatives to get at this point:
 ** - in the general case, the common base interface doesn't expose such operations.
 **   Thus we need to <i>mix in</i> an additional \em management interface; this
 **   can be done by \em subclassing the desired implementation type, because
 **   this concrete type is irrelevant after finishing the placement constructor.
 **   In order to re-access this management interface, so to be able to invoke
 **   the copy or clone operations, we need to do an elaborate re-cast operation,
 **   first going down to the leaf type and then back up into the mixed in
 **   management interface. Basically this operation is performed by using
 **   an \c dynamic_cast<CopyAPI&>(bufferContents)
 ** - but when the used client types provide some collaboration and implement
 **   this management interface either directly on the API or as an immediate
 **   sub-interface, then this copy/management interface is located within the
 **   direct inheritance chain and can be reached by a simple \c static_cast.
 **   Indeed, as we're just using a different meaning of the VTable, only a
 **   single indirection (virtual function call) is required at runtime in
 **   this case to invoke the copy ctor or assignment operator.
 ** 
 ** @see polymorphic-value-test.cpp
 ** @see opaque-holder.hpp other similar opaque inline buffer templates
 ** @see lib::time::Mutation usage example
 */


#ifndef LIB_POLYMORPHIC_VALUE_H
#define LIB_POLYMORPHIC_VALUE_H


#include "lib/error.hpp"
#include "lib/meta/duck-detector.hpp"
//#include "lib/bool-checkable.hpp"
//#include "lib/access-casted.hpp"
//#include "lib/util.hpp"

//#include <boost/noncopyable.hpp>
#include <boost/utility/enable_if.hpp>


namespace lib {
  
//  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
//  using util::isSameObject;
//  using util::unConst;
  
  
  namespace polyvalue { // implementation helpers...
    
    using boost::enable_if;
    using lumiera::Yes_t;
    using lumiera::No_t;
    
    struct EmptyBase{ };
    
    template<class IFA
            ,class BA = EmptyBase
            >
    class CopySupport
      : public BA
      {
      public:
        virtual ~CopySupport() { };
        virtual void cloneInto (void* targetBuffer)  const   =0;
        virtual void copyInto  (IFA& targetBase)     const   =0;
      };
    
    
    
    
    template<typename T>
    class exposes_copySupportFunctions
      {
        
        META_DETECT_FUNCTION(void, cloneInto, (void*) const);
        
      public:
        enum{ value = HasFunSig_cloneInto<T>::value
            };
      };
    
    
    template <class TY, class YES = void> 
    struct Trait
      {
        typedef CopySupport<TY> CopyAPI;
        enum{   ADMIN_OVERHEAD = 2 * sizeof(void*) };
        
        static CopyAPI&
        accessCopyHandlingInterface (TY& bufferContents)
          {
            REQUIRE (INSTANCEOF (CopyAPI, &bufferContents));
            return dynamic_cast<CopyAPI&> (bufferContents); 
          }
        
        typedef CopyAPI AdapterAttachment;
      };
    
    
    template <class TY>
    struct Trait<TY, typename enable_if< exposes_copySupportFunctions<TY> >::type>
      {
        typedef TY CopyAPI;
        enum{   ADMIN_OVERHEAD = 1 * sizeof(void*) };
        
        template<class IFA>
        static CopyAPI&
        accessCopyHandlingInterface (IFA& bufferContents)
          {
            REQUIRE (INSTANCEOF (CopyAPI, &bufferContents));
            return static_cast<CopyAPI&> (bufferContents); 
          }
        
        typedef EmptyBase AdapterAttachment;
      };
    
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
    , size_t storage             ///< maximum storage required for the target data to be held inline
    , class CPY = IFA            ///< special sub-interface to support copy operations
    >
  class PolymorphicValue
    {
      
      typedef polyvalue::Trait<CPY>     _Traits;
      typedef typename _Traits::CopyAPI _CopyHandlingAdapter;
      enum{ 
        siz = storage + _Traits::ADMIN_OVERHEAD 
      };
      
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
      
      
      template<class IMP>
      class Adapter
        : public IMP
        , public _Traits::AdapterAttachment
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
      
      
      _CopyHandlingAdapter&
      accessHandlingInterface ()  const
        {
          IFA& bufferContents = access<IFA>();
          _CopyHandlingAdapter& hap = _Traits::accessCopyHandlingInterface (bufferContents);
          return hap;                                                                   ////TODO cleanup unnecessary temporary
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
