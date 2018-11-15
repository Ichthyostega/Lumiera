/*
  INTERPOLATOR.hpp  -  denotes a facility to get (continuously interpolated) parameter values

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


/** @file interpolator.hpp
 ** Core abstraction: automation parameter interpolator
 ** Each interpolator implementation has the ability to resolve intermediary
 ** values and to provide a parameter value for _every arbitrary point in time._
 */


#ifndef STEAM_MOBJECT_INTERPOLATOR_H
#define STEAM_MOBJECT_INTERPOLATOR_H



namespace steam {
namespace mobject {
  
  
  /**
   * Provides the implementation for getting the actual value
   * of a time varying or automated effect/plugin parameter
   */
  template<class VAL>
  class Interpolator
    {
      ///////////////
    };
  
  
  
}} // namespace steam::mobject
#endif /*STEAM_MOBJECT_INTERPOLATOR_H*/
