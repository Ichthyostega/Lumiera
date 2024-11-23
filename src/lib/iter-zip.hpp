/*
  ITER-ZIP.hpp  -  join several iterators into a iterator-of-tuples

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file iter-zip.hpp
 ** Iterator builder to combine several iterables into a tuple sequence.
 ** 
 ** @see IterZip_test
 ** @see iter-explorer.hpp
 **
 */


#ifndef LIB_ITER_ZIP_H
#define LIB_ITER_ZIP_H


#include "lib/iter-adapter.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/meta/tuple-helper.hpp"

#include <utility>


namespace lib {
  
  namespace iter {
    
    /** construction-helper: apply IterExplorer builder packaged tuple */
    template<typename...ITS>
    auto
    buildIterTuple (ITS&& ...iters)
      {
        return std::make_tuple (lib::explore (std::forward<ITS> (iters)) ...);
      }
    
    template<class ITUP>
    class ProductCore
      {
        ITUP iters_;
        
      public:
        ProductCore(ITUP&& iterTup)
          : iters_{move (iterTup)}
          { }
        
        ProductCore()  = default;
        // default copy acceptable
        
        friend bool
        operator== (ProductCore const& cor1, ProductCore const& cor2)
        {
          return cor1.iters_ == cor2.iters_;
        }
        
        
        /* === »state core« protocol API === */
        
        bool
        checkPoint()  const
          {                                        //note: short-circuit
            return std::apply ([](auto&... its) { return (bool(its) and ...); }
                              , iters_);
          }
        
        ITUP&
        yield()  const      ///< exposing the iterator-tuple itself as »product«
          {
            return util::unConst(iters_);
          }
        
        void
        iterNext()
          {
            meta::forEach (iters_
                          ,[](auto& it){ ++it; });
          }
      };
  } // namespace lib::iter
  
  
  
  
  /**
   * Build a tuple-combining iterator builder
   * @param iters an arbitrary sequence of _iterable entities_
   * @return an IterExplorer to yield result tuples on iteration
   * @remark IterExplorer is both a »Lumiera Forward Iterator« and a _Pipeline Builder_
   *       - as Lumiera iterator, it can be used directly in _for-each_ and _while_ loops
   *       - result components can be picked up conveniently through _structural bindings_ for tuples
   *       - using the builder API, results can be postprocessed (apply a function), filtered, searched, reduced...
   */
  template<class...ITS>
  inline auto
  zip (ITS&& ...iters)
  {
    auto access_result = [](auto& it)->decltype(auto){ return *it; };               // Note: pass-through result type (maybe reference)
    auto tuple_results = [&](auto& it){ return meta::mapEach (*it, access_result); };
    //
    auto core = iter::ProductCore{iter::buildIterTuple (std::forward<ITS> (iters)...)};
    //
    return explore (std::move(core))
              .transform (tuple_results);
  }
  
  /** tuple-combining iterator prefixed by index sequence */
  template<class...ITS>
  inline auto
  izip (ITS&& ...iters)
  {
    return zip (eachNum<size_t>(), std::forward<ITS>(iters)...);
  }
  
  
  
} // namespace lib
#endif /*LIB_ITER_ZIP_H*/
