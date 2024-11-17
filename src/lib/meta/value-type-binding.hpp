/*
  VALUE-TYPE-BINDING.hpp  -  control type variations for custom containers

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file value-type-binding.hpp
 ** Type re-binding helper template for custom containers and adapters.
 ** This header defines a trait template which is used by the Iterator
 ** adapters and similar custom containers to figure out the value-,
 ** pointer- and reference types when wrapping iterators or containers.
 ** 
 ** When writing a generic container or adapter, there is typically a point
 ** where you'll need some variation of the payload type: you may want to expose
 ** a reference, or you might need a pointer to the type, to implement a forwarding
 ** `operator->()`. On a technical level, this turns out surprisingly tricky, since
 ** we often don't know the exact "incantation" of the payload type and might thus
 ** end up forming a pointer to a rvalue reference or likewise illegal constructs.
 ** 
 ** Within the STL, there is a convention to provide nested typedefs to indicate
 ** type variations in relation to the basic payload type of the container. We
 ** follow this convention and support especially the
 ** - `value_type` (what is conceived to be "in" the container or iterator)
 ** - a simple (LValue) reference to the payload
 ** - a pointer at the payload.
 ** 
 ** A custom container should likewise provide such type definitions, and the
 ** type rebinding helper template defined in this header makes it easy to
 ** provide such nested type definitions in a flexible way. This usage also
 ** creates an *Extension Point*: when some payload type requires special
 ** treatment, an explicit specialisation to this rebinding trait may be
 ** injected alongside with the definition of the payload type.
 ** 
 ** @see ValueTypeBinding_test
 ** @see iter-adapter.hpp
 ** @see scope-path.hpp usage example (explicit specialisation)
 */


#ifndef LIB_META_VALUE_TYPE_BINDING_H
#define LIB_META_VALUE_TYPE_BINDING_H


#include "lib/error.hpp"
#include "lib/meta/trait.hpp"




namespace lib {
namespace meta {
  
  namespace { // Helper trait to detect nested value_type binding definitions
    
    template<typename TY>
    class has_nested_ValueTypeBindings
      {
        template<typename X, typename XX = typename X::value_type
                           , typename XY = typename X::reference
                           , typename XZ = typename X::pointer
                >
        struct Probe
          { };
        
        template<class X>
        static Yes_t check(Probe<X> * );
        template<class>
        static No_t  check(...);
        
      public:
        static const bool value = (sizeof(Yes_t)==sizeof(check<TY>(0)));
      };
    
    template<class X>
    struct use_ValueTypebindings
      : __and_<has_nested_ValueTypeBindings< remove_reference_t<X> >
              ,__not_<is_StringLike< remove_reference_t<X> >
                     >
              >
      { };
  }
  
  
  /**
   * Type re-binding helper template for creating nested typedefs
   * usable by custom containers and iterator adapters or similar.
   * - this trait provides a value-, reference- and pointer type,
   *   similar to what the STL does.
   * - references are stripped, otherwise the base type is passed through
   * - _but_ when the base type in turn provides such nested typedefs,
   *   they are picked up and retrieved as result.
   * @note client code might define specialisations
   *       to handle tricky situations (like e.g. const_reverse_iter)
   */
  template<typename TY, typename SEL =void>
  struct ValueTypeBinding
    {
      using value_type = typename RefTraits<TY>::Value;
      using reference  = typename RefTraits<TY>::Reference;
      using pointer    = typename RefTraits<TY>::Pointer;
    };
  
  /** specialisation for classes providing STL style type binding definitions */
  template<typename TY>
  struct ValueTypeBinding<TY,      enable_if<use_ValueTypebindings<TY>> >
    {
      using _SrcType = typename RefTraits<TY>::Value;
      
      using value_type = typename _SrcType::value_type;
      using reference  = typename _SrcType::reference;
      using pointer    = typename _SrcType::pointer;
    };
  
  
  
}} // namespace lib::meta
#endif /*LIB_META_VALUE_TYPE_BINDING_H*/
