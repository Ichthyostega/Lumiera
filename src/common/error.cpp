/*
  Error  -  Cinelerra Exception Interface
 
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
 
* *****************************************************/


#include "common/error.hpp"
#include "nobugcfg.h"

///////////////////////////////////TODO
#include <iostream>

extern void booo()
  {
    std::cerr << "Booooo!!!" << std::endl;
    std::cerr.flush();
  }
///////////////////////////////////TODO

namespace cinelerra
  {
  char* killme ="cinelerra Errrror. TODO real description needed";
  
  /** Description of the problem, including the internal char constant
   *  in accordance to cinelerras error identification scheme.
   *  If a ::rootCause() can be obtained, this will be included in the
   *  generated output as well. 
   */
  const char*
  Error::what () const  throw()
    {
      TODO("really implement cinelerra::Error description");
      return killme; 
    }
  
  
  /** If this exception was caused by a chain of further exceptions,
   *  return the first one registered in this throw sequence.
   *  This works only, if every exceptions thrown as a consequence
   *  of another exception is propperly constructed by passing
   *  the original exception to the constructor
   */
  std::exception 
  Error::rootCause() const  throw()
    {
      UNIMPLEMENTED("storing and managing root causes");
    }
  
  
  
} // namespace cinelerra
