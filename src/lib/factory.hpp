/*
  FACTORY.hpp  -  template for object/smart-pointer factories

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


#ifndef LIB_FACTORY_H
#define LIB_FACTORY_H

#include <tr1/memory>



namespace lib {
  namespace factory {
    
    /**
     * Wrapping any object created by the Factory into some smart ptr class.
     * The Factory class inherits this functionality, so it can be exchanged
     * independently from the actual object creation behaviour. For example,
     * an Factory implementing some elaborate subclass creation scheme could
     * be instantiated to either produce auto-ptrs or shared-ptrs. 
     */
    template
      < class T,                     // the product to be created
        class SMP =std::auto_ptr<T> // smart-pointer actually returned
      >
    class Wrapper
        {
        protected:
          SMP wrap (T* product) { return SMP (product); }
          
        public:
          typedef SMP PType;
        };
    
    
    /**
     * Basic Factory Template, for defining flexible Factory classes.
     * These encapsulate the logic for creating of new objects, maybe
     * delegating the memory allocation to the backend layer. Usually,
     * the clients get just a smart-pointer or similar handle to the
     * created object, which will manage the ownership.
     * 
     * The provided default implementation uses just std::auto_ptr.
     */
    template
      < class T,                  // the product to be created
        class WR = Wrapper<T>    // used for fabricating the wrapper
      >
    class Factory :  public WR
      {
      public:
        /** Object creating facility.
         *  Intended to be over/written/ with a variant taking
         *  the appropriate number of parameters and maybe
         *  using some special custom allocator.
         *  Note: non-virtual.
         */
        typename WR::PType operator() () { return wrap (new T ); }
        
      };
    
    
    
    
    
    /* -- some example and default instantiations -- */
    
    using std::tr1::shared_ptr;
    
    /** 
     * a frequently used instantiation of the Wrapper,
     * Utilising the refcounting tr1::shared_ptr.
     */
    template<class T>
    class Wrapper<T, shared_ptr<T> >
      {
      protected:
        shared_ptr<T> wrap (T* product) { return shared_ptr<T> (product); }
        
      public:
        typedef shared_ptr<T> PType;
      };
    
    
    /** 
     * Shortcut: commonly used (partial) instantiation of the Factory,
     * generating refcounting shared_ptr wrapped Objects. Built upon
     * the corresponding special instantiation of the Wrapper template.
     */
    template<class T>
    class RefcountFac : public Factory<T, Wrapper<T,shared_ptr<T> > >
      {
      public:
        typedef shared_ptr<T> PType;
      };
     
    
    /** 
     *  another convenience instantiation: auto_ptr-to-Impl-Factory.
     *  Creating an implementation subclass and wraps into auto_ptr.
     *  @warning the TImpl object will typically be destroyed by the
     *           smart ptr using an T*, so ~T() needs to be virtual.
     */
    template
      < class T,       // Interface class
        class TImpl   // Implementation class to be created
      >
    class PImplFac : public Factory<T, Wrapper<T> >
      {
      public:
        typedef std::auto_ptr<T> PType;
        
        PType operator() (){ return wrap (static_cast<T*> (new TImpl)); };
      };
      
      
      
  } // namespace factory
  
  /// @note Factory can be usable as-is (wraps into std::auto_ptr)
  using factory::Factory;
  
  
} // namespace lib
#endif
