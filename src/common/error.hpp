/*
  ERROR.hpp  -  Cinelerra Exception Interface
 
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


#ifndef CINELERRA_ERROR_H
#define CINELERRA_ERROR_H

#include <exception>


namespace cinelerra
  {

  
  /**
   * Interface and Baseclass of all Exceptions thrown 
   * from within cinelerra (C++) code. Common operations
   * for getting an diagnostic message and for obtaining
   * the root cause, i.e. the frist exception encountered
   * in a chain of exceptions. 
   */
  class Error : public std::exception
    {
    public:
      /** yield a diagnostig message characterizing the problem */
      virtual const char* what () const throw();
      
      /** If this exception was caused by a chain of further exceptions,
       *  return the first one registered in this throw sequence.
       *  This works only, if every exceptions thrown as a consequence
       *  of another exception is propperly constructed by passing
       *  the original exception to the constructor
       */
      std::exception rootCause () const throw();

    private:
      /** a copy of the first exception encountered in this exception chain */
      std::exception cause;

    };
    
    
    
    
  /* === Exception Subcategories === */
    
  namespace error
    {
    
    class Logic : public Error
      {
        
      };
      
    class Config : public Error
      {
        
      };
      
    class State : public Error
      {
        
      };
      
    class Invalid : public Error
      {
        
      };
      
    class External : public Error
      {
        
      };
      
      
      
      
  } // namespace error

} // namespace cinelerra
#endif
