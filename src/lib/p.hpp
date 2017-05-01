/*
  P.hpp  -  customised shared_ptr with ordering and type relationships

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

/** @file p.hpp
 ** Customised refcounting smart pointer.
 ** Template derived from std::shared_ptr adding total ordering and
 ** type relationships implemented by forwarding to the pointees. In all other
 ** respects, it should behave exactly as shared_ptr and is able to cooperate
 ** and share ownership with other shared_ptr instantiations.
 ** 
 ** By default different instantiations of shared_ptr are completely unrelated types,
 ** even if using inheritance related type parameters for instantiation: a shared_ptr<Media>
 ** isn't some kind-of shared_ptr<Asset> -- we need to do an explicit static_ptr_cast. Another
 ** common problem is the definition of equality and ordering relations for shared_ptr: 
 ** equality is based on the equality of the managed pointers, while ordering is built upon
 ** the ref count. While generally this may be a good compromise, in our case it hinders treating
 ** the smart ptrs within the application almost as if they were the objects themselves and proved
 ** an obstacle for writing generic helper functions.
 **
 ** the P template resolves these problems by implementing the ordering operators in terms of
 ** the corresponding operators on the pointee and by allowing to specify a base class smart-ptr
 ** as template parameter.
 **
 ** @see asset.hpp
 ** @see custom-shared-ptr-test.cpp
 ** @see orderingofassetstest.cpp
 */


#ifndef LIB_P_H
#define LIB_P_H


#include "lib/error.hpp"
#include "lib/meta/util.hpp"

#include <memory>


namespace lib {
  
  using std::shared_ptr;
  using std::weak_ptr;
  
  /**
   * Customised refcounting smart pointer template, built upon
   * std::shared_ptr, but forwarding type relationships and
   * ordering operators to the pointee objects.
   * @param TAR the visible pointee type
   * @param BASE the shared-ptr type used as implementation
   * @note if the BASE smart-ptr type used as implementation
   *       implies another pointer type than the one used on
   *       the interface (=type TAR), then every access to the 
   *       pointee causes an dynamic cast. Thus the pointee types
   *       need to support RTTI.
   */
  template<class TAR, class BASE =shared_ptr<TAR>>
  class P
    : public BASE
    {
    public:
                                 P ( )                      : BASE() {}
      template<class Y> explicit P (Y* p)                   : BASE(p) {}
      template<class Y, class D> P (Y* p, D d)              : BASE(p,d){}
      
                                 P (P const& r)             : BASE(r)  {}
                                 P (P const&& rr)           : BASE(rr) {}
      template<class Y>          P (shared_ptr<Y> const& r) : BASE(r)  {}
      template<class Y> explicit P (weak_ptr<Y> const& wr)  : BASE(wr) {}
      template<class Y> explicit P (std::unique_ptr<Y>&& u) : BASE(u.release()) {}
      
      
      P& operator= (P const& r)                               { BASE::operator= (r);  return *this; }
      P& operator= (P const&& rr)                             { BASE::operator= (rr); return *this; }
      template<class Y> P& operator=(shared_ptr<Y> const& sr) { BASE::operator= (sr); return *this; }
      
      TAR* get() const        { return dynamic_cast<TAR*> (BASE::get()); }
      TAR& operator*() const  { return *get(); }
      TAR* operator->() const { return get();  }
      
      void swap(P& b)         { BASE::swap (b);}
      
      operator std::string()  const noexcept;
      
      
    private: /* === friend operators injected into enclosing namespace for ADL === */
                                                                                          //////////////////TICKET #932 Clang is unable to fill in the default template argument. Resolved in newer versions of Clang. Temporary workaround: add second parameter B
      template<typename _O_,typename B>
      friend inline bool
      operator== (P const& p, P<_O_, B> const& q) { return (p && q)? (*p == *q) : (!p && !q); }
      
      template<typename _O_,typename B>
      friend inline bool
      operator!= (P const& p, P<_O_, B> const& q) { return (p && q)? (*p != *q) : !(!p && !q); }
      
      template<typename _O_,typename B>
      friend inline bool
      operator<  (P const& p, P<_O_, B> const& q) { REQUIRE (p && q); return *p < *q; }   ///< @note deliberately not allowing comparison on NIL ////TICKET #307  : problem with equality test in associative containers, where equal(a,b) := !(a < b) && !(b < a)
      
      template<typename _O_,typename B>
      friend inline bool
      operator>  (P const& p, P<_O_, B> const& q) { REQUIRE (p && q); return *q < *p; }
      
      template<typename _O_,typename B>
      friend inline bool
      operator<= (P const& p, P<_O_, B> const& q) { REQUIRE (p && q); return *p <= *q;}
      
      template<typename _O_,typename B>
      friend inline bool
      operator>= (P const& p, P<_O_, B> const& q) { REQUIRE (p && q); return *p >= *q;}
      
    };
  
  
  
  
  /** Helper to create and manage by lib::P
   * @tparam X the type of the new object to create on the heap
   * @param ctorArgs arbitrary arguments to pass to ctor of `X`
   * @return managing smart-ptr of type P<X>, holding onto the
   *    object just created on the heap.
   */
  template<typename X, typename...ARGS>
  inline P<X>
  makeP (ARGS&&... ctorArgs)
  {
    return P<X>{new X {std::forward<ARGS>(ctorArgs)...}};
  }
  
  
  
  /**
   * use custom string conversion on pointee, if applicable,
   * otherwise fall back to a human readable type string.S
   */
  template<class TAR, class BASE>
  inline
  P<TAR,BASE>::operator std::string()  const noexcept
  try {
    if (BASE::get())
      return util::StringConv<TAR>::invoke (this->operator*());
    else
      return "⟂ P<"+meta::typeStr<TAR>()+">";
  }
  catch(...)
  { return meta::FAILURE_INDICATOR; }
  
  
  
} // namespace lib
#endif
