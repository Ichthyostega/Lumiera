/*
  VALUE-TYPE-BINDING.hpp  -  control type variations for custom containers

   Copyright (C)
     2010,2024        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file value-type-binding.hpp
 ** Type re-binding helper template for custom containers and adapters.
 ** This header defines trait templates which are used by the Iterator
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
 ** - `value_type` : how to store what is conceived to be "in" the container or iterator)
 ** - a simple (LValue) reference to the payload: how you access some content
 ** - a to form pointer at the payload, which is relevant for `operator->`
 ** 
 ** A custom container should likewise provide such type definitions, and the
 ** type rebinding helper template defined in this header makes it easy to
 ** provide such nested type definitions in a flexible way. This usage also
 ** creates an *Extension Point*: when some payload type requires special
 ** treatment, an explicit specialisation to this rebinding trait may be
 ** injected alongside with the definition of the payload type.
 ** @warning A special twist may happen when processing a function in an iterator (pipeline).
 **          In this special case, `ValueTypeBinding` should _not_ be used, since the function
 **          might yield again a nested iterator; yet we want to treat that iterator as a value
 **          in that special case, and not look into the iterator or retrieve results from it.
 **          Notably this situation applies to any kind of transforming iterator.
 **          lib::meta::RefTraits should be used in such cases.
 ** 
 ** The CommonResultYield type rebinding helper allows to reconcile several
 ** essentially compatible result types; it is used in iterator pipelines,
 ** especially for the case of _child expansion,_ where some additional
 ** sub-sequences are to be integrated into a main sequence.
 ** 
 ** @see ValueTypeBinding_test
 ** @see iter-adapter.hpp
 ** @see scope-path.hpp usage example (explicit specialisation)
 */


#ifndef LIB_META_VALUE_TYPE_BINDING_H
#define LIB_META_VALUE_TYPE_BINDING_H


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
      using value_type = RefTraits<TY>::value_type;
      using reference  = RefTraits<TY>::reference;
      using pointer    = RefTraits<TY>::pointer;
    };
  
  /** specialisation for classes providing STL style type binding definitions */
  template<typename TY>
  struct ValueTypeBinding<TY,      enable_if<use_ValueTypebindings<TY>> >
    {
      using _SrcType   = RefTraits<TY>::value_type;
      
      using value_type = _SrcType::value_type;
      using reference  = _SrcType::reference;
      using pointer    = _SrcType::pointer;
    };
  
    
}} // namespace lib::meta
#endif /*LIB_META_VALUE_TYPE_BINDING_H*/
