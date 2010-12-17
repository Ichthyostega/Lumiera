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


#ifndef PROC_MOBJECT_PARAMETER_H
#define PROC_MOBJECT_PARAMETER_H




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
  
  
  
} // namespace mobject
#endif
