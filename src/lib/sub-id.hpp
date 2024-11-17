/*
  SUB-ID.hpp  -  extensible symbolic identifier

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file sub-id.hpp
 ** Extensible symbolic ID type. 
 ** @remark this is a design sketch (from 9/2009) an not used anywhere as of 3/2017...
 ** 
 ** My observation is that, during design, I did run again and again into a specific
 ** situation, which I then needed to circumvent in lack of a first class solution.
 ** Learning from that experiences, I start building this structured ID template.
 ** - it shall be an \em symbolic identifier, not an artificial ID
 ** - the basic value set should be limited and statically type safe.
 ** - possibility of structured adornments and variations
 ** - optionally concealing these extensions from the interface level
 ** 
 ** The first attempt to build such an entity is based on standard techniques,
 ** disregarding performance and memory footprint considerations.
 ** 
 ** @todo 1/2016 this idea seems very reasonable, and we should just make it
 **       robust and usable, along the lines pointed out by that draft
 **       - use std::tuple as storage
 **       - remove the `format-util` dependency (!)           ////////////////////////////////TICKET #985
 **       - provide a hash implementation for real
 **       - extend to arbitrary number of sub-dimensions (variadic)
 **       - implement comparisons as you'd do for any algebraic type
 **       - provide a metaprogramming facility to get an id tuple
 ** 
 ** @see SubID_test
 ** @see MultiFact
 **
 */



#ifndef LIB_SUB_ID_H
#define LIB_SUB_ID_H

#include "lib/format-util.hpp"

//#include <functional>
#include <boost/functional/hash.hpp>      /////TODO better push the hash implementation into a cpp file (and btw, do it more seriously!)

#include <string>


namespace lib {

  using boost::hash_value;
  using std::string;
  
  
  
  /**
   * 
   */
  template<typename I>
  class SubId;
  
  class SubID
    {
    public:
      virtual ~SubID()  { }
      
      virtual operator string()  const =0;
    };
  
  
  inline size_t
  hash_value (SubID const& sID)
  {
    return hash_value (string (sID));
  }
  
  inline bool
  operator== (SubID const& id1, SubID const& id2)
  {
    return (string (id1) == string (id2));
  }
  
  ////////TODO a serious implementation should descend recursively, instead of relying on the string representation
  
  
  
  
  
  
  template<typename I>
  class SubId
    : public SubID
    {
      I baseID_;
      
    public:
      SubId (I id)
        : baseID_(id)
        { }
      
      operator string()  const
        {
          using util::toString;
          return toString (baseID_);  // note: extension point
        }
    };
  
  
  template<typename I, class SUZ>
  class ExtendedSubId
    : public SubId<I>
    {
      typedef SubId<I> _baID;
      
      SUZ extID_;
      
    public:
      ExtendedSubId (I i, SUZ const& chain)
        : _baID(i)
        , extID_(chain)
        { }
      
      operator string()  const
        {
          return _baID::operator string()
               + '.'
               + string (extID_);   
        }
    };
  
  
  
  
} // namespace lib
#endif
