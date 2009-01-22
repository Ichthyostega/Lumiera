/*
  SINGLETON-REF.hpp  -  helper template providing singleton-like access for implementation code
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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

/** @file singleton-ref.hpp
 ** Helper for singleton-kind access without managing object creation and lifecycle. 
 ** A typical usage scenario is when implementing C Language Interfaces without any
 ** canonical access to some "this" pointer.  
 **
 ** @see gui::NotificationService usage example
 */


#ifndef LIB_SINGLETON_REF_H
#define LIB_SINGLETON_REF_H


#include "lib/error.hpp"

#include <boost/noncopyable.hpp>


namespace lib {

  namespace singleton {
  
    /***************************************
     * Detail/Policy class specifying
     * how the SingletonRef can be accessed
     */
    template<class TY>
    class AccessAsReference
      : boost::noncopyable
      {
        TY* obj_;
        
        typedef AccessAsReference<TY> _ThisType;
        
      public:
        void
        open (TY* instance)
          {
            ASSERT (!obj_, "Lifecycle error");
            obj_ = instance;
          }
        
        void
        close ()
          {
            ASSERT (obj_, "Lifecycle error");
            obj_ = 0;
          }
        
        TY* 
        operator-> ()  const
          {
            if (!obj_)
              throw lumiera::error::State("Target currently not available.");
            return obj_;
          }
        
        
        typedef TY* _ThisType::*unspecified_bool_type;
        
        /** implicit conversion to "bool" */
        operator unspecified_bool_type()  const // never throws
          {
            return obj_?  &_ThisType::obj_ : 0;
          }
        
        bool operator! ()  const { return !obj_; }
        
      };
    
  } // namespace Singleton
  
  
  
  
  /*************************************************************
   * Helper template providing singleton access without managing
   * object creation and lifecycle.
   * @param TY the actual type to be made accessible
   * @param B  a base class to inherit from; defaults to noncopyable
   * @param Accessor how to implement the static instance access
   */
  template< class TY
          , class B                       = boost::noncopyable
          , template<class> class Access  = singleton::AccessAsReference
          >
  struct SingletonRef
    : public B
    {
      
      typedef Access<TY> Accessor;
      Accessor& accessor_;
      
      SingletonRef(TY * instance, Accessor& acc) 
        : accessor_(acc)
        { 
          accessor_.open (instance); 
        }
      
      ~SingletonRef()
        {
          accessor_.close ();
        }
    };
  
  
  
} // namespace lib
#endif
