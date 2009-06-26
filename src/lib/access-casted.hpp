/*
  ACCESS-CASTED.hpp  -  util template to access a value using conversion or cast as appropriate
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
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
 ** @see lumiera::WrapperPtr usage example to access a variant record
 ** 
 */


#ifndef UTIL_ACCESS_CASTED_H
#define UTIL_ACCESS_CASTED_H

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_polymorphic.hpp>
#include <boost/type_traits/is_base_of.hpp>



namespace util {
  using boost::remove_pointer;
  using boost::remove_reference;
  using boost::is_convertible;
  using boost::is_polymorphic;
  using boost::is_base_of;
  using boost::enable_if;
  
  
  template <typename SRC, typename TAR>
  struct can_cast : boost::false_type {};
  
  template <typename SRC, typename TAR>
  struct can_cast<SRC*,TAR*>          { enum { value = is_base_of<SRC,TAR>::value };};
  
  template <typename SRC, typename TAR>
  struct can_cast<SRC*&,TAR*>         { enum { value = is_base_of<SRC,TAR>::value };};
  
  template <typename SRC, typename TAR>
  struct can_cast<SRC&,TAR&>          { enum { value = is_base_of<SRC,TAR>::value };};
  
  
  template <typename T>
  struct has_RTTI
    {
      typedef typename remove_pointer<
              typename remove_reference<T>::type>::type TPlain;
    
      enum { value = is_polymorphic<TPlain>::value };
    };
  
  template <typename SRC, typename TAR>
  struct use_dynamic_downcast
    {
      enum { value = can_cast<SRC,TAR>::value
                     &&  has_RTTI<SRC>::value
                     &&  has_RTTI<TAR>::value
           };
    };
  
  template <typename SRC, typename TAR>
  struct use_static_downcast
    {
      enum { value = can_cast<SRC,TAR>::value
                  && (  !has_RTTI<SRC>::value
                     || !has_RTTI<TAR>::value
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
  
  
  
  
  
  template<typename RET>
  struct NullAccessor
    {
      typedef RET Ret;
      
      static RET access  (...) { return ifEmpty(); }
      static RET ifEmpty ()    { return EmptyVal<RET>::create(); }
    };
  
  template<typename TAR>
  struct AccessCasted : NullAccessor<TAR>
    {
      using NullAccessor<TAR>::access;
      
      template<typename ELM>
      static  typename enable_if< use_dynamic_downcast<ELM&,TAR>, TAR>::type
      access (ELM& elem)
        {
          return dynamic_cast<TAR> (elem);
        }
      
      template<typename ELM>
      static  typename enable_if< use_static_downcast<ELM&,TAR>, TAR>::type
      access (ELM& elem)
        {
          return static_cast<TAR> (elem);
        }
      
      template<typename ELM>
      static  typename enable_if< use_conversion<ELM&,TAR>, TAR>::type
      access (ELM& elem)
        {
          return elem;
        }
    };
  

} // namespace util
#endif
