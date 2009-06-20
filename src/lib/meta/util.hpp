/*
  UTIL.hpp  -  metaprogramming helpers and utilities
 
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


#ifndef LUMIERA_META_UTIL_H
#define LUMIERA_META_UTIL_H


  
namespace lumiera {
    
    
  /* types for figuring out the overload resolution chosen by the compiler */

  typedef char Yes_t;
  struct No_t { char padding[8]; };
    
    
    
  namespace typelist {
    
    /** semi-automatic detection if an instantiation is possible.
     *  Requires help by the template to be tested, which needs to define
     *  a typedef member \c is_defined. The embedded metafunction Test can be used
     *  as a predicate for filtering types which may yield a valid instantiation
     *  of the candidate template in question.
     *  \par
     *  A fully automated solution for this problem is impossible by theoretic reasons.
     *  Any non trivial use of such a \c is_defined trait would break the "One Definition Rule",
     *  as the state of any type can change from "partially defined" to "fully defined" over
     *  the course of any translation unit. Thus, even if there may be a \em solution out there,
     *  we can expect it to break at some point by improvements/fixes to the C++ Language.
     */
    template<template<class> class _CandidateTemplate_>
    struct Instantiation
      {
        
        template<class X>
        class Test
          {
            typedef _CandidateTemplate_<X> Instance;
            
            template<class U>
            static Yes_t check(typename U::is_defined *);
            template<class U>
            static No_t  check(...);
            
          public:
            static const bool value = (sizeof(Yes_t)==sizeof(check<Instance>(0)));
          };
      };
    
    
    /** Trait template for detecting a typelist type.
     *  For example, this allows to write specialisations with the help of
     *  boost::enable_if
     */
    template<typename TY>
    class is_Typelist
      {
        template<class X>
        static Yes_t check(typename X::List *);
        template<class>
        static No_t  check(...);
        
      public: 
        static const bool value = (sizeof(Yes_t)==sizeof(check<TY>(0)));
      };
    
    
    
  } // namespace typelist
  
} // namespace lumiera
#endif
