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

  namespace factory{ class VanillaAllocator; }//////////////////////////////////TODO
  
  /**
   * Configurable template for creating Factory classes.
   * These encapsulate the creating of new objects, indeed
   * delegating the memory allocation to the backend layer.
   * The clients get just a smart-pointer or similar handle
   * to the created object, which will manage the ownership.
   * 
   * The provided default implementation uses just std::auto_ptr,
   * but delegates the allocation to cinelerra's backend-layer.
   * 
   */
  template
    <
      class T,                                    // the product to be created
      template <class> class SMP = std::auto_ptr,// smart-pointer actually returned
      class ALO  = factory::VanillaAllocator     // Allocator facility to be used //////////////TODO
    >
  class Factory : protected ALO
    {
    public:
      /** Object creating facility.
       *  Intended to be over/written/ with a variant taking
       *  the appropriate number of parameters and using the
       *  (privately inherited) functions of the allocator.
       *  Note: non-virtual.
       */
      SMP<T> operator() (){ return SMP<T> (new T ); };

      typedef SMP<T> ptype;
      
    private:
      void operator= (const Factory&); // copy prohibited
    };
    
    
    
  /* -- some example and default instantiiations -- */  
    
  namespace factory
    {
    /**
     * Example Allocator using just the normal C++ memory management.
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
    
    
    using std::tr1::shared_ptr;

    /** a frequently used instantiation of the Factory,
     *  using the refcounting shared_ptr from Boost
     *  and for allocation just our default Allocator
     */
    template<class T>
    class RefcountPtr : public Factory<T, shared_ptr>
      {
        /** let the smart-Ptr use the custom operator delete, 
         *  which may be defined in our Allocator baseclass.
         */
        static void destroy (T* victim) { delete victim; };
        
      public:
        shared_ptr<T> operator() ()     { return shared_ptr<T> (new T, &destroy ); }
      };
      
      
    /** another convienience instantiiation: auto_ptr-Factory,
     *  actually creating a subclass of the returned type
     */
    template<class T, class TImpl>
    class SubclassPtr : public Factory<T>
      {
        typedef std::auto_ptr<T> aP;
        
      public:
        aP operator() (){ return aP (new TImpl ); };
      };
      
      
      
  } // namespace factory

} // namespace cinelerra
#endif
