/*
  PARAMPROVIDER.hpp  -  interface denoting a source for actual Parameter values

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file paramprovider.hpp
 ** Core abstraction: an entity to expose a single parameter
 ** 
 ** @todo as of 2016, we still need to work out our parameter/automation concept.
 */


#ifndef STEAM_MOBJECT_PARAMPROVIDER_H
#define STEAM_MOBJECT_PARAMPROVIDER_H



namespace steam {
namespace mobject {

  template<class VAL> class Parameter;
  template<class VAL> class Interpolator;



  /**
   * A facility to get the actual value of a plugin/effect parameter
   */
  template<class VAL>
  class ParamProvider
    {
    protected:
      Parameter<VAL>* param;
      Interpolator<VAL>* ipo;


    public:
      virtual VAL getValue () ;
    };



}} // namespace steam::mobject
#endif /*STEAM_MOBJECT_PARAMPROVIDER_H*/
