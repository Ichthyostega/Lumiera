/*
  ACCESS-CASTED.hpp  -  util template to access a value using conversion or cast as appropriate

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file access-casted-o.hpp
 ** Helper for accessing a value, employing either conversion or downcast,
 ** depending on the relation of the source type (type of the original value)
 ** and the target type (type we need within the usage context). 
 ** When instantiating AcessCasted<TAR>, we get a template static function
 ** \c AcessCasted<TAR>::access<SRC>(SRC& elm), but the actual implementation
 ** is chosen using std::type_traits. If no sensible implementation can be
 ** selected, \c EmptyVal<TAR>::create() is invoked instead, which by default
 ** creates a NULL value or similar by using the no-argument ctor of the
 ** type TAR. Alternatively, you may define an specialisation of EmptyVal,
 ** e.g. throwing an exception instead of creating a NULL value. 
 ** 
 ** @deprecated old obsolete version              ///////////////////////////////////////////TICKET #738 clean-up access-casted, rewrite variant (4/2015)
 ** @todo This is the obsoleted old version: It was excessively permissive, which 
 **       I've learned to view as a danger, since it encourages a sloppy programming style.
 ** @see lumiera::WrapperPtr usage example to access a variant record
 ** @see lib::InPlaceAnyHolder usage example to access a subclass in embedded storage
 ** 
 */


#ifndef UTIL_ACCESS_CASTED_O_H
#define UTIL_ACCESS_CASTED_O_H

#include <type_traits>



namespace util {
  
  namespace {
    using std::remove_pointer;
    using std::remove_reference;
    using std::is_convertible;
    using std::is_polymorphic;
    using std::is_base_of;
    
    
    template <typename SRC, typename TAR>
    struct can_cast : std::false_type {};
    
    template <typename SRC, typename TAR>
    struct can_cast<SRC*,TAR*>          { enum { value = is_base_of<SRC,TAR>::value };};
    
    template <typename SRC, typename TAR>
    struct can_cast<SRC*&,TAR*>         { enum { value = is_base_of<SRC,TAR>::value };};
    
    template <typename SRC, typename TAR>
    struct can_cast<SRC&,TAR&>          { enum { value = is_base_of<SRC,TAR>::value };};
    
    
    template <typename T>
    struct hasRTTI
      {
        typedef typename remove_pointer<
                typename remove_reference<T>::type>::type TPlain;
      
        enum { value = is_polymorphic<TPlain>::value };
      };
    
    template <typename SRC, typename TAR>
    struct use_dynamic_downcast
      {
        enum { value = can_cast<SRC,TAR>::value
                       &&  hasRTTI<SRC>::value
                       &&  hasRTTI<TAR>::value
             };
      };
    
    template <typename SRC, typename TAR>
    struct use_static_downcast
      {
        enum { value = can_cast<SRC,TAR>::value
                    && (  !hasRTTI<SRC>::value
                       || !hasRTTI<TAR>::value
                       )
             };
      };
    
    template <typename SRC, typename TAR>
    struct use_conversion
      {
        enum { value = is_convertible<SRC,TAR>::value
                    && !( use_static_downcast<SRC,TAR>::value
                        ||use_dynamic_downcast<SRC,TAR>::value
                        )
             };
      };
    
    
    
    template<typename X>
    struct EmptyVal
      {
        static X create()    { return X(); }
      };
    template<typename X>
    struct EmptyVal<X*&>
      {
        static X*& create()  { static X* nullP(0); return nullP; }
      };
    
  }
  
  
  
  
  template<typename RET>
  struct NullAccessor
    {
      typedef RET Ret;
      
      static RET access  (...) { return ifEmpty(); }
      static RET ifEmpty ()    { return EmptyVal<RET>::create(); }
    };
  
  template<typename TAR>
  struct AccessCasted_O : NullAccessor<TAR>
    {
      using NullAccessor<TAR>::access;
      
      template<typename ELM>
      static  typename std::enable_if< use_dynamic_downcast<ELM&,TAR>::value,
      TAR     >::type
      access (ELM& elem)
        {
          return dynamic_cast<TAR> (elem);
        }
      
      template<typename ELM>
      static  typename std::enable_if< use_static_downcast<ELM&,TAR>::value,
      TAR     >::type
      access (ELM& elem)
        {
          return static_cast<TAR> (elem);
        }
      
      template<typename ELM>
      static  typename std::enable_if< use_conversion<ELM&,TAR>::value,
      TAR     >::type
      access (ELM& elem)
        {
          return elem;
        }
    };
  

} // namespace util
#endif
