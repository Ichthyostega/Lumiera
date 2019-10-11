/*
  SCOPED-HOLDER-TRANSFER.hpp  -  using ScopedHolder within a STL vector 

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


/** @file scoped-holder-transfer.hpp
 ** A mechanism to take ownership without allowing copy.
 ** @deprecated obsolete concept, superseded by C++11 rvalue references
 ** @todo expunge this!
 */



#ifndef LIB_SCOPEDHOLDER_TRANSFER_H
#define LIB_SCOPEDHOLDER_TRANSFER_H

#include "lib/error.hpp"
#include <memory>


namespace lib {
  
  
  
  
  /**
   * Addendum to scoped-holder.hpp for transferring the lifecycle
   * management to another instance. Using these wrappers within
   * STL vector and similar containers may result in the need to
   * do a re-allocation in response to a request to grow.
   * Obviously such a feature needs support by the objects being
   * wrapped, which should provide an operation for transferring 
   * lifecycle management in a controlled fashion. This behaviour
   * is similar to std::auto_ptr, but because we use a separate
   * dedicated operation, we avoid some of the dangers pertaining
   * the use of the latter: just taking the "value" can't kill
   * the managed object. 
   * \par
   * To implement this feature we need
   * - a custom allocator to be used by the vector. By default
   *   it is built as a thin proxy round std::allocator.
   * - the \em noncopyable type to be managed within the vector
   *   needs to provide a custom extension point: when the
   *   allocator detects the need to transfer control between
   *   two instances, it will invoke a free function named
   *   <tt> transfer_control(TY& from, TY& to)</tt> intended
   *   to be found by ADL. Note: in case this function throws,
   *   it <i>must not have any side effects</i>.
   * - besides, the \em noncopyable type needs to provide an
   *   <tt>operator bool()</tt> yielding true iff currently
   *   containing an managed object. This is similar to
   *   std::unique_ptr or even the behaviour of a plain
   *   old raw pointer, which is equivalent to \c true
   *   when the pointer isn'T \c NULL
   * @deprecated obsoleted by C++11 rvalue references
   * 
   */
  template<class TY, class PAR = std::allocator<TY>>
  class Allocator_TransferNoncopyable
    {
      typedef Allocator_TransferNoncopyable<TY,PAR> _ThisType;
      
      PAR par_;
      
      
    public:
      typedef typename PAR::size_type       size_type;
      typedef typename PAR::difference_type difference_type; 
      typedef typename PAR::pointer         pointer;
      typedef typename PAR::const_pointer   const_pointer;
      typedef typename PAR::reference       reference;
      typedef typename PAR::const_reference const_reference;
      typedef typename PAR::value_type      value_type;

      template<typename XX>
        struct rebind
        { typedef Allocator_TransferNoncopyable<XX, PAR> other; };

      Allocator_TransferNoncopyable()  { }

      Allocator_TransferNoncopyable(const _ThisType& allo)
        : par_(allo.par_) { }
      Allocator_TransferNoncopyable(const PAR& allo)
        : par_(allo) { }

      template<typename X>
      Allocator_TransferNoncopyable(const std::allocator<X>&) { }

      ~Allocator_TransferNoncopyable() { }

      
      //------------proxying-the-parent-allocator------------------------------------
      
      size_type     max_size()                      const { return par_.max_size(); }
      pointer       address(reference r)            const { return par_.address(r); }
      const_pointer address(const_reference cr)     const { return par_.address(cr); }
      pointer       allocate(size_type n, const void *p=0){ return par_.allocate(n,p); }
      void          deallocate(pointer p, size_type n)    {        par_.deallocate(p,n); }
      void          destroy(pointer p)                    {        par_.destroy(p); }
      
      
      void 
      construct (pointer p, const TY& ref)
        {
          new(p) TY();   /////////////////////TICKET #1204
          ASSERT (p);
          ASSERT (!(*p), "protocol violation: target already manages another object.");
          if (ref)
            transfer_control (const_cast<TY&>(ref), *p);
        }
    };
  
  template<typename TY1, typename TY2, class ALO>
  inline bool
  operator== (Allocator_TransferNoncopyable<TY1,ALO> const&, Allocator_TransferNoncopyable<TY2, ALO> const&)
  { return true; }

  template<typename TY1, typename TY2, class ALO>
  inline bool
  operator!= (Allocator_TransferNoncopyable<TY1,ALO> const&, Allocator_TransferNoncopyable<TY2,ALO> const&)
  { return false; }

  
  
  
} // namespace lib
#endif
