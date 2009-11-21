/*
  SCOPE-PATH.hpp  -  logical access path down from Session root
 
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


#ifndef MOBJECT_SESSION_SCOPE_PATH_H
#define MOBJECT_SESSION_SCOPE_PATH_H

//#include "proc/mobject/mobject.hpp"
//#include "proc/mobject/placement-ref.hpp"
#include "proc/mobject/session/scope.hpp"
#include "lib/iter-adapter.hpp"

#include <vector>
//#include <string>

using std::vector;
//using std::string;

namespace mobject {
namespace session {
  
  
  
  /**
   * TODO type comment
   */
  class ScopePath
    {
      vector<Scope> path_;
      
      typedef vector<Scope>                  _VType;
      typedef _VType::const_reverse_iterator _VIter;
      typedef lib::RangeIter<_VIter>      _IterType;
      
    public:
      ScopePath();
      
      /// Iteration is always ascending from leaf to root
      typedef _IterType iterator;
      iterator begin()  const;
      iterator end()    const;
      
      Scope getLeaf()  const;
      
      void clear();
      
    };
///////////////////////////TODO currently just fleshing the API
  
  
  inline ScopePath::iterator
  ScopePath::begin()  const
  {
    return iterator (path_.rbegin(), path_.rend());
  }
  
  inline ScopePath::iterator
  ScopePath::end()    const
  {
    return iterator();
  }
  
  
}} // namespace mobject::session
#endif
