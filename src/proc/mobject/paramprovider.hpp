/*
  PARAMPROVIDER.hpp  -  interface denoting a source for actual Parameter values
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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


#ifndef MOBJECT_PARAMPROVIDER_H
#define MOBJECT_PARAMPROVIDER_H



namespace mobject
  {

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



} // namespace mobject
#endif
