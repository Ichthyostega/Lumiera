/*
  BUILDER-QUALIFIER-SUPPORT.hpp  -  accept arbitrary qualifier terms for builder functions

   Copyright (C)
     2022,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file builder-qualifier-support.hpp
 ** Mix-in to support builder functions to accept optional qualifier terms.
 ** For the classical _builder pattern_ we define a dedicated *builder* class,
 ** which in turn offers explicitly named setter or configuration functions.
 ** However, sometimes it is desirable just to offer some builder-like extension
 ** on regular constructor syntax, where optional additional parameters may be passed
 ** to adapt the basic setup. Unfortunately, a list of optional further argument tends
 ** to become hard to read, especially when some dummy arguments must be passed in order
 ** to supply a setting further down the list.
 ** 
 ** This mix-in template instead allows to define _qualifier terms,_ and to pass an arbitrary
 ** sequence of such terms to a constructor or builder function. These qualifiers look like
 ** "algebraic data types" at invocation site, i.e. they are named terms, optionally even accepting
 ** further arguments. Yet for the implementation, these qualifiers return a wrapped functor. When
 ** defined as _friend function,_ the generated qualifier functors may even manipulate private internals
 ** of the "Builder" or "Strategy" class.
 ** @warning since these qualifiers manipulate by side-effect, it is up to the user to ensure
 **          consistent settings and behaviour (typically within a terminal `build()` function).
 ** @note the `qualify()` function accepts the qualifier functors by-reference; thus it is up
 **          to the user to either pass them by-value, by reverence or even by perfect forwarding.
 ** 
 ** @see BuilderQualifierSupport_test
 ** @see ElementBoxWidget as usage example
 */


#ifndef LIB_BUILDER_QUALIFIER_SUPPORT_H
#define LIB_BUILDER_QUALIFIER_SUPPORT_H


#include <functional>


namespace lib {
  
  /**
   * Mix-in to accept and apply an arbitrary sequence of qualifier functors.
   * Typically these are used for Builder or Strategy definitions and applied
   * to some target object, which thereby is manipulated by the qualifiers by
   * side-effect.
   * @tparam TAR target type to apply the qualifier functors
   * @remark the intended usage is to mix-in this template privately and then
   *   to define _qualifier friend functions,_ each binding a suitable lambda
   *   to manipulate the internal settings. This way, at usage site, an arbitrary
   *   sequence of clearly labelled "algebraic terms" can be accepted, to tweak
   *   and adapt the basic features established on the Builder or Strategy.
   */
  template<class TAR>
  class BuilderQualifierSupport
    {
    protected:
      using Manipulator = std::function<void(TAR&)>;
      
      struct Qualifier
        : Manipulator
        {
          using Manipulator::Manipulator;
        };
      
      /** Main entrance point: apply the given qualifiers in sequence to the `target`. */
      template<class... QUALS>
      friend void qualify(TAR& target, Qualifier& qualifier, QUALS& ...qs)
      {
        qualifier(target);
        qualify(target, qs...);
      }
      
      friend void qualify(TAR&){ }
      
    public:
      // default construct and copyable
    };
  
  
}// namespace lib
#endif /*LIB_BUILDER_QUALIFIER_SUPPORT_H*/
