/*
  Factorytest  -  check basic workings of object/smart-pointer factory
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 
* *****************************************************/


#include "common/factory.hpp"

namespace cinelerra
  {
  
  class Blubb
    {
      int uii ;

    public:
      typedef factory::RefcountPtr<Blubb> Factory;
      static Factory create;
      
      Blubb() : uii(42) {} ;
    };
  
  /** a static Factory instance 
   *  for creating refcounting Ptrs to Blubb objects 
   */
  Blubb::Factory Blubb::create;
  
  std::tr1::shared_ptr<Blubb> huii = Blubb::create ();
  std::tr1::shared_ptr<Blubb> pfuii = huii;

} // namespace cinelerra
