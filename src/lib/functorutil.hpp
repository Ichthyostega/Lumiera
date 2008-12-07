/*
  FUNCTORUTIL.hpp  -  collection of helpers for dealing with functors and signals
 
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


#ifndef FUNCTORUTIL_H_
#define FUNCTORUTIL_H_

#include <tr1/functional>



namespace util { ////////////TODO: refactor it. But probably not directly into namespace lib. Needs some more consideration though
  
  using std::tr1::function;
  using std::tr1::bind;
  using std::tr1::placeholders::_1;
  
  
  /** "Combiner" which calls two functions one after another
   *  returning the result of the second invocation.  */
  template<typename SIG>
  class CombineSequenced;
  
  template<typename ARG, typename RET>
  struct CombineSequenced<RET(ARG)>
    {
      typedef function<RET(ARG)> Func;
      
      static RET 
      dispatch (Func first, Func second, ARG arg)
        {
          first (arg);
          return second (arg);
        }
    };
  
  
  template< typename SIG 
          , class COM = CombineSequenced<SIG> 
          >
  class Dispatch
    : public function<SIG>
    {
      typedef typename COM::Func Func;
      
    public:
      Dispatch (Func const& f1,
                Func const& f2)
        : Func (bind (&COM::dispatch, f1, f2, _1))
        { }
    };
  
  
  /** convenience shortcut to call two functors in sequence. 
   *  @return a Dispatch functor object which incorporates the
   *          functors as copy and on invocation calls the first 
   *          function and then returns the result of the second */
  template<typename SIG>
  Dispatch<SIG>
  dispatchSequenced (function<SIG> const& f1,
                     function<SIG> const& f2)
  {
    return Dispatch<SIG> (f1, f2);
  }
  
  
} // namespace util

#endif /*UTIL_HPP_*/
