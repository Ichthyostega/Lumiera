/*
  P.hpp  -  customized shared_ptr with ordering and type relationships
 
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

/** @file p.hpp
 ** customized refcounting smart pointer.
 ** Template derived from std::tr1::shared_ptr adding total ordering and 
 ** type relationships implemented by forwarding to the pointees. In all other
 ** respects, it should behave exactly as shared_ptr and is able to cooperate
 ** and share ownership with other shared_ptr instantiations.
 ** 
 ** By default different instantiations of shared_ptr are completely unrelated types,
 ** even if using inherintance related type parameters for instantiation: a shared_ptr<Media>
 ** isn't some kind-of shared_ptr<Asset> -- we need to do an explicit static_ptr_cast. Another
 ** common problem is the definition of equality and ordering relations for shared_ptr: 
 ** equality is based on the equality of the managed pointers, while ordering is built upon
 ** the ref count. While generally this may be a good compromise, in our case it hinders treating
 ** the smart ptrs within the application almost as if they were the objects themselfs and proved
 ** an obstacle for writing generic helper functions.
 **
 ** the P template resolves these problems by implementing the ordering operators in terms of
 ** the corresponding operators on the pointee and by allowing to specify a base class smart-ptr
 ** as template parameter.
 **
 ** @see asset.hpp
 ** @see customsharedptrtest.cpp
 ** @see orderingofassetstest.cpp
 */


#ifndef LUMIERA_P_H
#define LUMIERA_P_H


#include <tr1/memory>


namespace lumiera {
  
  using std::tr1::shared_ptr;
  using std::tr1::weak_ptr;
  
  /**
   * Customised refcounting smart pointer template, built upon
   * std::tr1::shared_ptr, but forwarding type relationships and
   * ordering operators to the pointee objects.
   * @param TAR the visible pointee type
   * @param BASE the shared-ptr type used as implementation
   * @note if the BASE smart-ptr type used as implementation
   *       implies another pointer type than the one used on
   *       the interface (=type TAR), then every access to the 
   *       pointee causes an dynamic cast. Thus the pointee types
   *       need to support RTTI.
   */
  template<class TAR, class BASE =shared_ptr<TAR> >
  class P
    : public BASE
    {
    public:
                                 P ( )                      : BASE() {}
      template<class Y> explicit P (Y* p)                   : BASE(p) {}
      template<class Y, class D> P (Y* p, D d)              : BASE(p,d){}

                                 P (P const& r)             : BASE(r)  {}
      template<class Y>          P (shared_ptr<Y> const& r) : BASE(r)  {}
      template<class Y> explicit P (weak_ptr<Y> const& wr)  : BASE(wr) {}
      template<class Y> explicit P (std::auto_ptr<Y> & ar)  : BASE(ar) {}      
    
      
      P& operator= (P const& r)                               { BASE::operator= (r);  return *this; }  
      template<class Y> P& operator=(shared_ptr<Y> const& sr) { BASE::operator= (sr); return *this; }
      template<class Y> P& operator=(std::auto_ptr<Y> & ar)   { BASE::operator= (ar); return *this; }

      TAR* get() const        { return dynamic_cast<TAR*> (BASE::get()); }
      TAR& operator*() const  { return *get(); }
      TAR* operator->() const { return get();  }

      void swap(P& b)         { BASE::swap (b);}
      
    
    private: /* === friend operators injected into enclosing namespace for ADL === */
      template<typename _O_>
      friend inline bool
      operator== (P const& p, P<_O_> const& q) { return (p && q)? (*p == *q) : (!p && !q); }
  
      template<typename _O_>
      friend inline bool
      operator!= (P const& p, P<_O_> const& q) { return (p && q)? (*p != *q) : !(!p && !q); }
  
      template<typename _O_>
      friend inline bool
      operator<  (P const& p, P<_O_> const& q) { return (p && q) && (*p < *q); }
  
      template<typename _O_>
      friend inline bool
      operator>  (P const& p, P<_O_> const& q) { return (p && q) && (*q < *p); }

      template<typename _O_>
      friend inline bool
      operator<= (P const& p, P<_O_> const& q) { return (p && q)? (*p <= *q) : (!p && !q); }

      template<typename _O_>
      friend inline bool
      operator>= (P const& p, P<_O_> const& q) { return (p && q)? (*p >= *q) : (!p && !q); }
      
    };
   
    
} // namespace lumiera
#endif
