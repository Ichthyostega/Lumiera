/*
  ACCESS-CASTED.hpp  -  util template to access a value using conversion or cast as appropriate

   Copyright (C)
     2008, 2015       Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file access-casted.hpp
 ** Helper for accessing a value, employing either a conversion or downcast,
 ** depending on the relation of the source type (type of the original value)
 ** and the target type (type we need within the usage context). 
 ** When instantiating AcessCasted<TAR>, we get a template static function
 ** \c AcessCasted<TAR>::access<SRC>(SRC&& elm), where the actual implementation
 ** is chosen using based on our type traits. If no sensible implementation can be
 ** selected, a static assertion will be triggered.
 ** 
 ** The possible conversion path is limited to options considered "safe"
 ** - automatic type conversions, as would happen when returning a value of type SRC
 **   from a function with return signature TAR
 ** - this possibly includes an up-cast from an implementation type to a base class
 ** - explicit copy construction when the type TAR is a plain value
 ** - downcast to implementation class \em only when a safe dynamic downcast can be
 **   performed, based on RTTI in the actual object.
 ** - additionally, a pointer will be formed by taking an address
 ** - and a reference will be initialised by dereferencing a pointer (with NULL check).
 ** - if valid and permitted by the rules of the language, r-value references and const
 **   values, references and pointers-to-const are supported as well.
 ** 
 ** @see lib::InPlaceAnyHolder usage example to access a subclass in embedded storage
 ** 
 */


#ifndef UTIL_ACCESS_CASTED_H
#define UTIL_ACCESS_CASTED_H

#include "lib/error.hpp"

#include <type_traits>
#include <utility>



namespace util {
  
  namespace error = lumiera::error;
  
  namespace { // implementation helper traits....
    
    using std::remove_pointer;
    using std::remove_reference;
    
    
    template <typename T>
    using PlainType = typename remove_pointer<
                      typename remove_reference<T>::type>::type;
    
    template <typename T>
    struct has_RTTI
      {
        static constexpr bool value = std::is_polymorphic<PlainType<T>>::value;
      };
    
    template <typename SRC, typename TAR>
    struct can_downcast
      {
        static constexpr bool value = std::is_base_of<PlainType<SRC>, PlainType<TAR>>::value
                                      && (  (   std::is_pointer<typename remove_reference<SRC>::type>::value
                                            &&  std::is_pointer<typename remove_reference<TAR>::type>::value
                                            )
                                          ||(  !std::is_pointer<typename remove_reference<SRC>::type>::value
                                            && !std::is_pointer<typename remove_reference<TAR>::type>::value
                                         ));
      };
    
    template <typename SRC, typename TAR>
    struct can_use_dynamic_downcast
      {
        static constexpr bool value =  !std::is_convertible<SRC,TAR>::value
                                     && can_downcast<SRC,TAR>::value
                                     && has_RTTI<SRC>::value
                                     && has_RTTI<TAR>::value;
      };
    
    template <typename SRC, typename TAR>
    struct can_use_conversion
      : std::is_convertible<SRC,TAR>
      { };
    
    template <typename SRC, typename TAR>
    struct can_take_address
      {
        static constexpr bool value =   !std::is_rvalue_reference<SRC>::value                       // considered dangerous
                                     && !std::is_pointer<typename remove_reference<SRC>::type>::value
                                     &&  std::is_pointer<typename remove_reference<TAR>::type>::value;
      };
    
    template <typename SRC, typename TAR>
    struct can_dereference
      {
        static constexpr bool value =  !std::is_pointer<typename remove_reference<TAR>::type>::value
                                     && std::is_pointer<typename remove_reference<SRC>::type>::value;
      };
    
    
    template <typename SRC, typename TAR>
    struct if_can_use_dynamic_downcast
      : std::enable_if< can_use_dynamic_downcast<SRC,TAR>::value, TAR>
      { };
    
    template <typename SRC, typename TAR>
    struct if_can_use_conversion
      : std::enable_if< can_use_conversion<SRC,TAR>::value, TAR>
      { };
    
    template <typename SRC, typename TAR>
    struct if_can_take_address
      : std::enable_if< can_take_address<SRC,TAR>::value, TAR>
      { };
    
    template <typename SRC, typename TAR>
    struct if_can_dereference
      : std::enable_if< can_dereference<SRC,TAR>::value, TAR>
      { };
      
  }//(End)helper traits
  
  
  
  
  /**
   * Helper template to access a given value,
   * possibly converted or casted in a safe way.
   */
  template<typename TAR>
  struct AccessCasted
    {
      typedef TAR Ret;
      
      
      template<typename SRC>
      static  typename if_can_use_dynamic_downcast<SRC&&,TAR>::type
      access (SRC&& elem)
        {
          try
            {
              return dynamic_cast<TAR> (std::forward<SRC>(elem));
            }
          catch (std::bad_cast& castError)
            {
              throw error::Invalid(castError
                                  ,"AccessCasted: not the expected runtime type; downcast failed"
                                  ,error::LUMIERA_ERROR_WRONG_TYPE);
            }
        }
      
      
      template<typename SRC>
      static  typename if_can_use_conversion<SRC&&,TAR>::type
      access (SRC&& elem)
        {
          return std::forward<SRC> (elem);
        }
      
      
      template<typename SRC>
      static  typename if_can_take_address<SRC&&,TAR>::type
      access (SRC&& elem)
        {
          return AccessCasted<TAR>::access (&elem);
        }
      
      
      template<typename SRC>
      static  typename if_can_dereference<SRC&&,TAR>::type
      access (SRC&& elem)
        {
          if (!elem)
            throw error::Invalid("AccessCasted: attempt to build a value or reference from a NULL pointer"
                                ,error::LUMIERA_ERROR_BOTTOM_VALUE);
          
          return AccessCasted<TAR>::access (*elem);
        }
      
      
      /** catch-all to signal failure of conversion */
      static TAR
      access (...)
        {
          // NOTE: if you see this assertion failure, none of the above predicates were true.
          //       Chances are that you requested a conversion that is logically impossible or dangerous,
          //       like e.g. taking a reference from an anonymous value parameter
          static_assert (!sizeof(TAR), "AccessCasted: No valid conversion or cast supported for these types.");
          throw error::Invalid("impossible or unsafe type conversion requested");
        }
    };
  
  
  
} // namespace util
#endif
