/*
  FUNCTOR-UTIL.hpp  -  collection of helpers for dealing with functors and signals

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


/** @file functor-util.hpp
 ** Collection of small helpers and utilities related to function objects.
 ** 
 ** @todo combine with meta/function-closure.hpp and reorganise
 ** @todo 2017 find out to what extent we still need that (looks obsolete)
 **  
 ** @see GuiSubsysDescriptor#start (guifacade.cpp)
 ** @see MementoTie
 ** @see functor-util-test.cpp
 **
 */



#ifndef FUNCTOR_UTIL_H
#define FUNCTOR_UTIL_H

#include "lib/hash-value.h"

#include <functional>
#include <boost/functional/hash.hpp>



namespace util { ////////////TODO: refactor namespace. But probably not directly into namespace lib. Needs some more consideration though
  
  using std::function;
  using std::bind;
  using std::placeholders::_1;
  
  namespace { // hiding some nasty details...
    
    using lib::HashVal;
    using boost::hash_combine;
  
    /**
     * This Class is used to bypass the access protection
     * and break into the tr1::function implementation.
     * Thus we can implement a raw comparison function,
     * as a replacement for the missing functor comparison
     * facility. (TR1 requires an operator==, but boost
     * seemingly doesn't provide it, because it can't
     * be done correctly/properly in all cases. See
     * the section "FAQ" in the documentation of 
     * the boost/function library)
     * 
     * The layout of this class is chosen to mimic that
     * of the boost function implementation, without all
     * the generic type decoration. For the comparison
     * we use a conservative approach, by requiring
     * the concrete invoker, the storage manager and
     * the actual function and argument data pointers
     * to be the same.
     * @todo 2017 is this still necessary today?
     */
    class HijackedFunction 
      : std::_Function_base
      {
        typedef void (*DummyInvoker) (void);
        DummyInvoker invoker_;
        
      public:
        
        friend bool
        operator== (HijackedFunction const& f1,
                    HijackedFunction const& f2)
          {
            return (f1.invoker_ == f2.invoker_)
                && (f1._M_manager == f2._M_manager)
                && (f1._M_functor._M_unused._M_const_object ==
                    f2._M_functor._M_unused._M_const_object );
          }                      // note: we don't cover any member pointer offset
        
        friend HashVal
        hash_value (HijackedFunction const& fun)
          {
            HashVal hash(0);
            hash_combine (hash, fun.invoker_);
            hash_combine (hash, fun._M_manager);
            hash_combine (hash, fun._M_functor._M_unused._M_const_object);
            return hash;        // note: member pointer offset part uncovered
          }
      };
    
  }
  
  
  /** temporary workaround: tr1/functional should define
   *  public comparison operators for functor objects, but
   *  in the implementation provided by boost 1.34 it doesn't.
   *  To get at least \em some comparison capability, we do a
   *  brute force comparison of the functor's internal data.
   *  @note use with caution. This implementation relies on
   *        internal details of boost/function; but it is
   *        rather conservative and might deem functors
   *        "different" erroneously, due to garbage in 
   *        the internal functor data's storage */
  template<typename SIG>
  inline bool
  rawComparison (function<SIG> const& f1,
                 function<SIG> const& f2)
  {
    typedef HijackedFunction const& Hij;
    
    return reinterpret_cast<Hij> (f1)
        == reinterpret_cast<Hij> (f2);
  }
  
  /** catch-all for the comparison: functors with
   *  different base type are always "different" */
  template<typename SIG1, typename SIG2>
  inline bool
  rawComparison (function<SIG1> const&,
                 function<SIG2> const&)
  {
    return false;
  }
  
  /** variant with unchecked access */
  inline bool
  rawComparison (void* f1, void* f2)
  {
    typedef HijackedFunction * HijP;
    
    return (!f1 && !f2)
        || *reinterpret_cast<HijP> (f1)
        == *reinterpret_cast<HijP> (f2);
  }
  
  
  /** workaround to calculate a hash value for a given function object. 
   * @note use with caution. This implementation relies on internal details
   *       of boost/function; it can be expected to be rather conservative,
   *       i.e. yielding different hash values for objects, which actually
   *       are semantically equivalent.
   * @warning especially function objects bound to member functions aren't
   *       fully supported. It \em may happen that we miss differences on the
   *       offset part and only hash the "this" pointer on some platform.
   */
  template<typename SIG>
  inline HashVal
  rawHashValue (function<SIG> const& fun)
  {
    typedef HijackedFunction const& Hij;
    
    return hash_value (reinterpret_cast<Hij> (fun));
  }
  
} // namespace util

namespace std {
  
  /** inject into std::tr1 to be picked up by ADL:
   * @return hash value of given functor
   * @note   use with caution. Hash is calculated
   *         relying on undocumented boost internals.
   */
//  template<typename SIG>
//  inline lib::HashVal
//  hash_value (function<SIG> const& fun)
//  {
//    return util::rawHashValue (fun);
//  }
  
}

#endif /*FUNCTOR_UTIL_H*/
