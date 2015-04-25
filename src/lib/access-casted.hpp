/*
  ACCESS-CASTED.hpp  -  util template to access a value using conversion or cast as appropriate

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file access-casted.hpp
 ** Helper for accessing a value, employing either conversion or downcast,
 ** depending on the relation of the source type (type of the original value)
 ** and the target type (type we need within the usage context). 
 ** When instantiating AcessCasted<TAR>, we get a template static function
 ** \c AcessCasted<TAR>::access<SRC>(SRC& elm), but the actual implementation
 ** is chosen using boost::type_traits. If no sensible implementation can be
 ** selected, \c EmptyVal<TAR>::create() is invoked instead, which by default
 ** creates a NULL value or similar by using the no-argument ctor of the
 ** type TAR. Alternatively, you may define an specialisation of EmptyVal,
 ** e.g. throwing an exception instead of creating a NULL value. 
 ** 
 ** @todo WIP 4/2015 is being rewritten           ///////////////////////////////////////////TICKET #738 needs slight overhaul and clean-up
 ** @see lumiera::WrapperPtr usage example to access a variant record
 ** @see lib::InPlaceAnyHolder usage example to access a subclass in embedded storage
 ** 
 */


#ifndef UTIL_ACCESS_CASTED_H
#define UTIL_ACCESS_CASTED_H

#include "lib/error.hpp"

#include <type_traits>
#include <utility>



namespace util {
  using std::remove_pointer;
  using std::remove_reference;
  
  namespace error = lumiera::error;
  
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
      static constexpr bool value = std::is_base_of<PlainType<SRC>, PlainType<TAR>>::value;
    };
  
  template <typename SRC, typename TAR>
  struct can_use_dynamic_downcast
    {
      static constexpr bool value =  !std::is_convertible<SRC,TAR>::value
                                   && can_downcast<SRC,TAR>::value
                                   && has_RTTI<TAR>::value;
    };
  
  template <typename SRC, typename TAR>
  struct can_use_conversion
    : std::is_convertible<SRC,TAR>
    { };
  
  template <typename SRC, typename TAR>
  struct can_take_address
    {
      static constexpr bool value =  !std::is_pointer<typename remove_reference<SRC>::type>::value
                                   && std::is_pointer<typename remove_reference<TAR>::type>::value;
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
    
  
      
  template<typename TAR>
  struct AccessCasted
    {
      typedef TAR Ret;
      
      template<typename SRC>
      static  typename if_can_use_dynamic_downcast<SRC&&,TAR>::type
      access (SRC&& elem)
        {
          std::cerr << "downcast-"<<lib::test::showRefKind<SRC>()<<" && ->"<<lib::test::showRefKind<SRC&&>()<<std::endl;
          return dynamic_cast<TAR> (std::forward<SRC>(elem));
        }
      
      template<typename SRC>
      static  typename if_can_use_conversion<SRC&&,TAR>::type
      access (SRC&& elem)
        {
          std::cerr << "convert-"<<lib::test::showRefKind<SRC>()<<" && ->"<<lib::test::showRefKind<SRC&&>()<<std::endl;
          return std::forward<SRC> (elem);
        }
      
      template<typename SRC>
      static  typename if_can_take_address<SRC&&,TAR>::type
      access (SRC&& elem)
        {
          std::cerr << "address-"<<lib::test::showRefKind<SRC>()<<" && ->"<<lib::test::showRefKind<SRC&&>()<<std::endl;
          return AccessCasted<TAR>::access (&elem);
        }
      
      template<typename SRC>
      static  typename if_can_dereference<SRC&&,TAR>::type
      access (SRC&& elem)
        {
          std::cerr << "deref-"<<lib::test::showRefKind<SRC>()<<" && ->"<<lib::test::showRefKind<SRC&&>()<<std::endl;
          if (!elem)
            throw error::Invalid("AccessCasted: attempt to build a value or reference from a NULL pointer"
                                ,error::LUMIERA_ERROR_BOTTOM_VALUE);
          
          return AccessCasted<TAR>::access (*elem);
        }
    };
  
  
} // namespace util
#endif
