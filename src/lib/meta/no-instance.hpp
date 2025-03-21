/*
  NO-INSTANCE.hpp  -  marker for pure metaprogramming types

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file no-instance.hpp
 ** Metaprogramming helper to prevent an entity to be ever instantiated at runtime.
 ** For Template Metaprogramming, we often define classes for the sole purpose of
 ** forming a distinct type. Such helper types are established during compilation
 ** and typically trigger the instantiation of further types from templates, while
 ** there is never the intention to emit any actual code from these entities.
 ** As a safety measure, such _strictly metaprogramming related_ facilities may be
 ** marked to provoke a compilation failure, when the compiler -- accidentally --
 ** attempts to generate code to incorporate such an entity into the runtime world.
 ** 
 ** \par example
 ** The classical example is a marker type used to select one of various implementation
 ** flavours of a given facility. In fact this helper was created to mark variations
 ** based on the time code format in use.
 ** @see timecode.hpp
 */


#ifndef LIB_META_NO_INSTANCE_H
#define LIB_META_NO_INSTANCE_H

  
namespace lib {
namespace meta{
  
  /** 
   * An Entity never to be instantiated.
   * Marker baseclass for elements used in metaprogramming only.
   * Every attempt to instantiate such an element will cause an
   * compilation failure
   */
  template<class X>
  struct NoInstance
    {
      NoInstance()                   { static_assert (!sizeof(X)); }
      NoInstance (NoInstance const&) { static_assert (!sizeof(X)); }
    };
  
}} // namespace lib::meta
#endif
