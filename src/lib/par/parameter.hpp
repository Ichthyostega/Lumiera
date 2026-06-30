/*
  PARAMETER.hpp  -  generic parameter values

   Copyright (C)
     2008,2026        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file parameter.hpp
 ** A generic type value that can be discovered and automated.
 ** 
 ** @todo this is a draft and placeholder, as of 2026
 */


#ifndef LIB_PAR_PARAMETER_H
#define LIB_PAR_PARAMETER_H




namespace lib {
namespace par {
  
  
  
  /**
   * Descriptor and container to hold a generic parameter.
   * The value can be fixed or supplied by an automation function.
   */
  class Parameter
    {
    public:
      template<typename VAL>
      VAL get();
      
    protected:
      
    };
  
  
  
}} // namespace lib::par
#endif /*LIB_PAR_PARAMETER_H*/
