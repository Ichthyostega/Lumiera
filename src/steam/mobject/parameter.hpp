/*
  PARAMETER.hpp  -  representation of an automatable effect/plugin parameter

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


/** @file parameter.hpp
 ** Core abstraction: parameter to be controlled and possibly automated.
 ** 
 ** @todo as of 2016, we still need to work out our parameter/automation concept.
 */


#ifndef STEAM_PROC_MOBJECT_PARAMETER_H
#define STEAM_PROC_MOBJECT_PARAMETER_H




namespace steam {
namespace mobject {
  
  template<class VAL> class ParamProvider;
  
  
  /**
   * Descriptor and access object for a plugin parameter.
   * Parameters may be provided with values from the session,
   * and this values may be automated.
   */
  template<class VAL>
  class Parameter
    {
    public:
      VAL getValue () ;
      
    protected:
      ParamProvider<VAL>* provider;
      
    };
  
  
  
}} // namespace steam::mobject
#endif /*STEAM_PROC_MOBJECT_PARAMETER_H*/
