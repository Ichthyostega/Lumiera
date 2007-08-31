/*
  FACTORY.hpp  -  template for object/smart-pointer factories
 
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


#ifndef CINELERRA_FACTORY_H
#define CINELERRA_FACTORY_H

#include <tr1/memory>



namespace cinelerra
  {
  namespace factory
    {
    /**
     * Example NOP Allocator using just the normal C++ memory management.
     * The intended use is for a Factory instance to iherit from this class.
     * Specialized Allocators typically overload operator new and delete.
     */
    class VanillaAllocator {};
    
    /**
     * Example Allocator using plain C memory management. 
     */
    class MallocAllocator
      {
        void* operator new (size_t siz) { return malloc (siz); };
        void  operator delete (void* p) { if (p) free (p);     };
      };
      
    typedef VanillaAllocator DefaultALO;   // Allocator facility to be used by default //////////////TODO    
    
    /**
     * Wrapping any object created by the Factory into some smart ptr class.
     * The Factory class inherits this functionallity, so it can be exchanged
     * independently from the actual object creation behaviour. For example,
     * an Factory implementing some elaborate subclass creation scheme could
     * be intantiated to either procuce auto-ptrs or shared-ptrs. 
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
          
          typedef void (*DelHandler) (T* victim);  ///< custom deleter function
          void setDelHandler (DelHandler) {};      ///< if non-standard deleting is necessary
        };
    
    
    /**
     * Basic Factory Template, for definig flexible Factory classes.
     * These encapsulate the logic for creating of new objects, maybe
     * delegating the memory allocation to the backend layer. Usually,
     * the clients get just a smart-pointer or similar handle to the
     * created object, which will manage the ownership.
     * 
     * The provided default implementation uses just std::auto_ptr,
     * but delegates the allocation to cinelerra's backend-layer.
     * 
     * @todo actually do the delgation of memory management instead of using VanillaAllocator! 
     */
    template
      < class T,                    // the product to be created
        class WR = Wrapper<T>,     // used for fabricating the wrapper
        class ALO  = DefaultALO   // Allocator facility to be used
      >
    class Factory :  public WR, protected ALO
      {
      public:
        /** Object creating facility.
         *  Intended to be over/written/ with a variant taking
         *  the appropriate number of parameters and using the
         *  (privately inherited) functions of the allocator.
         *  Note: non-virtual.
         */
        typename WR::PType operator() () { return wrap (new T ); }
        
      protected:        
        /** this custom deleter function can be used if the
         *  operator delete call needs to be in the current scope.
         *  @see RefcountPtr
         */
        static void destroy (T* victim) { delete victim; };
      };
    
    
    
    
    /* -- some example and default instantiiations -- */  
    
    using std::tr1::shared_ptr;

    /** 
     * a frequently used instantiation of the Wrapper,
     * utilizing the refcounting shared_ptr from Boost.
     */
    template<class T>
    class Wrapper<T, shared_ptr<T> >
      {
        
      protected:
        shared_ptr<T> wrap (T* product) { return shared_ptr<T> (product, destroy_ ); }

      public:
        typedef shared_ptr<T> PType;

        
        Wrapper () : destroy_(&stdDelete) { }
        
        typedef void (*DelHandler) (T* victim);                     ///< custom deleter function
        void setDelHandler (DelHandler d) { this->destroy_ = d; }  ///< custom deleter used by shard_ptr
        
      private:
        DelHandler destroy_;
        static void stdDelete (T* victim) { delete victim; };   ///< by default just delete objects normally
      };
    
    
    /** 
     * Shortcut: commonly used (partial) instantiation of the Factory,
     * generating refcounting shared_ptr wrapped Objects. Built upon
     * the corresponding special intstantiation of the Wrapper template. 
     */
    template
      < class T,                 // the product to be created
        class ALO  = DefaultALO // Allocator facility to be used
      >
    class RefcountPtr : public Factory<T, Wrapper<T,shared_ptr<T> >, ALO>
      {
      public:
        typedef shared_ptr<T> PType;
        
        /** especially the shared_ptr will use Factory::destroy,
         *  so it is sufficient to make Factory a friend if the
         *  Target class to be produced has private ctor/dtors
         */ 
        RefcountPtr() { setDelHandler(&this->destroy); }
      };
     
    
    /** 
     *  another convienience instantiiation: auto_ptr-to-Impl-Factory.
     *  Creating an implementation subclass and wraps into auto_ptr.
     */
    template
      < class T,                  // Interface class
        class TImpl,             // Implementation class to be created
        class ALO  = DefaultALO // Allocator facility to be used
      >
    class PImplPtr : public Factory<T, Wrapper<T>, ALO>
      {
      public:
        typedef std::auto_ptr<T> PType;
        PType operator() (){ return wrap (new TImpl); };
      };
      
      
      
  } // namespace factory

  /// @note Factory can be usable as-is (wraps into std::auto_ptr)
  using factory::Factory;
  
  
} // namespace cinelerra
#endif
