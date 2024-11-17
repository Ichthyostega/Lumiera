/*
  VIRTUAL-COPY-SUPPORT.hpp  -  helper to support copying of erased sub-types

   Copyright (C)
     2015,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file virtual-copy-support.hpp
 ** Helper for building »virtual copy« operations.
 ** Especially in conjunction with type erasure, sometimes we have to deal with
 ** situations, where we want to copy or assign an object without knowing the
 ** full implementation type. Obviously, the default (compiler generated) operations
 ** will not work in such situations -- even worse, they will slice the objects on
 ** copy. The reason is: we _must know the full implementation and storage layout_
 ** of a type do provide any meaningful copy, assignment or move operation.
 ** 
 ** A possible workaround is to call into the actual implementation type through
 ** a virtual function (which requires a VTable): Even if everyone else has ceded
 ** ("erased") any knowledge regarding the exact implementation type, the function
 ** pointers in the VTable still implicitly hold onto that precise implementation type,
 ** since they were set up during construction, where the type was still available.
 ** Such a scheme of dealing with "opaque" copy operations is known as **virtual copy**
 ** — it can be dangerous and tricky to get right and preferably it is used only
 ** in flat and effectively closed class hierarchies.
 ** 
 ** This helper template simplifies the construction of such a scheme.
 ** - a base interface defines the available virtual copy operations
 ** - a set of CRTP-style templates covers all the cases of
 **   - full copy support
 **   - copy construction but no assignment
 **   - only move construction allowed
 **   - non-copyable type
 ** - we use type traits and a policy template to pick the correct implementation
 **   for a given data type. Any assignment or copy operations not supported by the
 **   target type will be replaced by an implementation which raises a runtime error
 **   (Exception); due to relying on virtual functions, unfortunately the check and
 **   error can not happen at compile time.
 ** 
 ** \par prerequisites
 ** The actual implementation type needs to provide the necessary standard / custom
 ** copy and assignment operations with the usual signature. Moreover, the implementation
 ** type provides a static function `downcast(Base&)` to perform a suitable dynamic
 ** or static downcast from the interface type to the concrete implementation type.
 ** 
 ** # Usage
 ** 
 ** The provided virtual operations are to be used in "backward" direction: invoked
 ** on the source and affecting the target. The source, through the VTable, knows its
 ** precise type and data layout. The target is handed in as parameter and treated
 ** by the `downcast()` function — which preferably performs a dynamic cast
 ** (or at least asserts the correct type). This whole scheme can only work for
 ** copy and assignment of objects, which actually have the same implementation
 ** type -- it will never be possible to "cross copy" to an completely unrelated
 ** target type, at least not generically.
 ** 
 ** Usage example
 ** \code{.cpp}
 **   Variant (Variant const& ref)
 **     {
 **       ref.buffer().copyInto (&storage_);
 **     }
 **   
 **   Variant&
 **   operator= (Variant const& ovar)
 **     {
 **       ovar.buffer().copyInto (this->buffer());
 **       return *this;
 **     }
 **     
 **   Buffer const&
 **   buffer()  const
 **     {
 **       return *reinterpret_cast<const Buffer*> (&storage_);
 **     }
 ** \endcode
 ** In this example, the concrete implementation of the \a Buffer interface
 ** will mix in the Policy template with the implementations of the virtual copy
 ** operations. The copy constructor uses the virtual `copyInto(void*)` to perform
 ** a "placement new", whereas the assignment operator calls the virtual `copyInto(Buffer&)`
 ** to downcast the target \a Buffer and in the end invokes the assignment operator on
 ** the concrete \a Buffer implementation subclass.
 ** 
 ** @warning please make sure that _only the virtual copy operation_ is invoked, since
 **          this operation will delegate to the copy operation on the implementation class
 **          and thus already invoke the whole chain of custom / compiler generated copy
 **          implementations. Ignoring this warning can lead to insidious slicing or partial
 **          copies. Additionally, if you _really need multiple level deep_ inheritance,
 **          you need to mix in the copy implementations on _every_ level _again,_ and
 **          you need to provide custom copy operations on every level.
 ** @warning please ensure the target storage for copy/clone is properly aligned. TICKET #1204
 ** 
 ** @see VirtualCopySupport_test
 ** @see lib::Variant usage example
 ** 
 */


#ifndef LIB_META_VIRTUAL_COPY_SUPPORT_H
#define LIB_META_VIRTUAL_COPY_SUPPORT_H


#include "lib/error.hpp"

#include <type_traits>
#include <utility>


namespace lib {
namespace meta{
  
  using std::move;
  
  using std::is_move_constructible;
  using std::is_copy_constructible;
  using std::is_copy_assignable;
  using std::__and_;
  using std::__not_;
  
  namespace error = lumiera::error;
  
  
  
  struct EmptyBase { };
  
  template<class IFA, class BASE = EmptyBase>
  class VirtualCopySupportInterface
    : public BASE
    {
    public:
      virtual ~VirtualCopySupportInterface()  { }
      
      virtual void  copyInto (void* targetStorage)  const =0;
      virtual void  moveInto (void* targetStorage)        =0;
      virtual void  copyInto (IFA&         target)  const =0;
      virtual void  moveInto (IFA&         target)        =0;
    };
  
  
  
  template<class I, class D, class B =I>
  class NoCopyMoveSupport
    : public B
    {
      virtual void
      copyInto (void*)  const override
        {
          throw error::Logic{"Copy construction invoked but target is noncopyable"};
        }
      
      virtual void
      moveInto (void*)  override
        {
          throw error::Logic{"Move construction invoked but target is noncopyable"};
        }
      
      virtual void
      copyInto (I&)  const override
        {
          throw error::Logic{"Assignment invoked but target is not assignable"};
        }
      
      virtual void
      moveInto (I&)  override
        {
          throw error::Logic{"Assignment invoked but target is not assignable"};
        }
    };
  
  
  template<class I, class D, class B =I>
  class MoveSupport
    : public NoCopyMoveSupport<I,D,B>
    {
      virtual void
      copyInto (void*)  const override
        {
          throw error::Logic{"Copy construction invoked but target allows only move construction"};
        }
      
      virtual void
      moveInto (void* targetStorage)  override
        {
          D& src = static_cast<D&> (*this);
          new(targetStorage) D(move(src));
        }
    };
  
  
  template<class I, class D, class B =I>
  class CloneSupport
    : public MoveSupport<I,D,B>
    {
      virtual void
      copyInto (void* targetStorage)  const override
        {
          D const& src = static_cast<D const&> (*this);
          new(targetStorage) D(src);
        }
    };
  
  
  template<class I, class D, class B =I>
  class FullCopySupport
    : public CloneSupport<I,D,B>
    {
      virtual void
      copyInto (I& target)  const override
        {
          D&       t = D::downcast(target);
          D const& s = static_cast<D const&> (*this);
          t = s;
        }
      
      virtual void
      moveInto (I& target)  override
        {
          D& t = D::downcast(target);
          D& s = static_cast<D&> (*this);
          t = move(s);
        }
    };
  
  
  
  
  namespace { // helpers to select suitable variant of copy support...
    
    template<class X>
    struct supports_only_move
      : __and_<is_move_constructible<X>
              ,__not_<is_copy_constructible<X>>
              ,__not_<is_copy_assignable<X>>
              >
      { };
    
    template<class X>
    struct supports_cloning
      : __and_<is_move_constructible<X>
              ,is_copy_constructible<X>
              ,__not_<is_copy_assignable<X>>
              >
      { };
    
    template<class X>
    struct supports_copy_and_assignment
      : __and_<is_move_constructible<X>
              ,is_copy_constructible<X>
              ,is_copy_assignable<X>
              >
      { };
  }
  
  
  
  
  /** Policy to pick a suitable implementation of "virtual copy operations".
   * @note You need to mix in the VirtualCopySupportInterface
   */
  template<class X, class SEL=void>
  struct CopySupport
    {
      template<class I, class D, class B =I>
      using Policy = NoCopyMoveSupport<I,D,B>;
    };
  
  template<class X>
  struct CopySupport<X,              enable_if<supports_only_move<X>> >
    {
      template<class I, class D, class B =I>
      using Policy = MoveSupport<I,D,B>;
    };
  
  template<class X>
  struct CopySupport<X,              enable_if<supports_cloning<X>> >
    {
      template<class I, class D, class B =I>
      using Policy = CloneSupport<I,D,B>;
    };
  
  template<class X>
  struct CopySupport<X,              enable_if<supports_copy_and_assignment<X>> >
    {
      template<class I, class D, class B =I>
      using Policy = FullCopySupport<I,D,B>;
    };
  
  
  
  
}} // namespace lib::meta
#endif /*LIB_META_VIRTUAL_COPY_SUPPORT_H*/
