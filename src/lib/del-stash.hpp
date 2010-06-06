/*
  DEL-STASH.hpp  -  collect and execute deleter functions
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file del-stash.hpp
 ** Collecting and finally triggering deleter functions.
 ** This building block for custom memory management allows memorising how to kill an object.
 ** Frequently, custom allocation schemes have to deal with type erased elements, where the
 ** full typed context is only available during construction. When implementing these objects
 ** without vtable, we need a reliable way of recalling the correct destructor to invoke.
 ** Typically, such entry objects are to be de-allocated in bulk during shutdown, with
 ** the possibility to deallocate some objects beforehand explicitly.
 ** 
 ** The implementation is based on using a vector in a stack-like fashion, thus the
 ** deallocation of individual objects might degenerate in performance.
 ** 
 ** @see DelStash_test
 ** @see AdviceSystem usage example
 **
 */



#ifndef LIB_DEL_STASH_H
#define LIB_DEL_STASH_H


#include "lib/error.hpp"

//#include <functional>
//#include <boost/functional/hash.hpp>      /////TODO better push the hash implementation into a cpp file (and btw, do it more seriously!)

//#include <iostream>
//#include <string>
#include <boost/noncopyable.hpp>


namespace lib {

//  using boost::hash_value;
  
  
  /** 
   * Manage a collection of deleter functions.
   * This component can memorise addresses and deleter functions
   * and trigger deletion of single objects, or delete all objects
   * on demand or automatically on shutdown.
   */
  class DelStash
    : boost::noncopyable
    {
      
    public:
      DelStash (size_t elms_to_reserve =0)
        { 
          if (elms_to_reserve)
            {
              //
            }
        }
      
     ~DelStash ()
        {
          try { killAll(); }
          
          catch(...)
            {
              Symbol errID = lumiera_error();
              WARN (memory, "Problems on de-allocation: %s", errID.c());
            }
        }
      
      size_t
      size ()  const
        {
          UNIMPLEMENTED ("killer size");
        }
      
      
      template<typename TY>
      void
      manage (TY* obj)
        {
          UNIMPLEMENTED ("accept typed ptr for later killing");      
        }
      
      template<typename TY>
      void
      manage (TY& obj)
        {
          UNIMPLEMENTED ("accept typed obj ref for later killing");      
        }
      
      template<typename TY>
      void
      manage (void *obj)
        {
          UNIMPLEMENTED ("accept object by void* and type information");      
        }
      
      
      template<typename TY>
      void
      kill (TY* obj)
        {
          UNIMPLEMENTED ("pick and kill object denoted by address");      
        }
      
      template<typename TY>
      void
      kill (TY& obj)
        {
          UNIMPLEMENTED ("pick and kill object denoted by reference");      
        }
      
      void
      killAll ()
        {
          UNIMPLEMENTED ("mass kill");
        }
    };
  
  
  
  
} // namespace lib
#endif
