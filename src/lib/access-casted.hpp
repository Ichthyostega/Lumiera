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
  
  
  template <typename T>
  struct has_RTTI
    {
      using PlainType = typename remove_pointer<
                        typename remove_reference<T>::type>::type;
    
      static constexpr bool value = std::is_polymorphic<PlainType>::value;
    };
  
  template <typename SRC, typename TAR>
  struct can_downcast
    {
      using PlainSRC = typename remove_pointer<
                       typename remove_reference<SRC>::type>::type;
      using PlainTAR = typename remove_pointer<
                       typename remove_reference<TAR>::type>::type;
    
      static constexpr bool value = std::is_base_of<PlainSRC, PlainTAR>::value;
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
  struct if_can_use_dynamic_downcast
    : std::enable_if< can_use_dynamic_downcast<SRC,TAR>::value, TAR>
    { };
  
  template <typename SRC, typename TAR>
  struct if_can_use_conversion
    : std::enable_if< can_use_conversion<SRC,TAR>::value, TAR>
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
    };
  
  
} // namespace util
#endif
