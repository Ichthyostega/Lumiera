/*
  SUB-ID.hpp  -  extensible symbolic identifier
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file sub-id.hpp
 ** Extensible symbolic ID type. 
 ** @todo (9/09) WIP-WIP-WIP just some vague emerging ideas here...
 ** 
 ** My observation is that, during design, I did run again and again into a specific
 ** situation, which I then needed to circumvent due to a lacking primary solution.
 ** Learning from that experiences, I start building this structured ID template.
 ** - it shall be an \em symbolic identifier, not an artificial ID
 ** - the basic value set should be limited and statically type safe.
 ** - possibility of structured adornments and variations
 ** - optionally concealing these extensions from the interface level
 ** 
 ** The first attempt to build such an entity is based on standard techniques,
 ** disregarding performance and memory footprint considerations.
 ** 
 ** @see SubID_test
 ** @see MultiFact
 **
 */



#ifndef LIB_SUB_ID_H
#define LIB_SUB_ID_H

#include "lib/format.hpp"

//#include <functional>
#include <iostream>
#include <string>


namespace lib {

  using std::string;
  using std::ostream;
  
  
  
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

      
  ostream&
  operator<< (ostream& os, SubID const& sID)
    {
      return os << string(sID);
    }
  

  
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
          return util::str (baseID_);   
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
