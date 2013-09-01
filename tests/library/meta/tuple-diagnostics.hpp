/*
  TUPLE-DIAGNOSTICS  -  helper for diagnostics of type tuples

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


/** @file tuple-diagnostics.hpp
 ** an extension to typelist-diagnostics.hpp, allowing to dump the \em contents
 ** of a Tuple datatype. Any type defining an operator string() may be printed
 ** when used as Tuple member type; special formatting is provided for the
 ** Num<int> test types, which makes typelist and tuples of these types 
 ** a good candidate for unit tests.
 **
 ** @see type-tuple-test.cpp
 ** @see typelist-manip-test.cpp
 **
 */
#ifndef META_TUPLE_DIAGNOSTICS_H
#define META_TUPLE_DIAGNOSTICS_H


#include "meta/typelist-diagnostics.hpp"
#include "lib/format-string.hpp"
#include "lib/meta/tuple.hpp"

#include <boost/utility/enable_if.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <string>

using std::string;
using boost::enable_if;
using boost::lexical_cast;
using util::unConst;


namespace lib  {
namespace meta {
namespace test {
  
  
  namespace { // Diagnostics
    
    
    
    template<int i>
    string
    showTupElement(Num<i> o) 
    {
      static util::_Fmt   constElm("(%i)");
      static util::_Fmt changedElm("{%i}");
      
      return string( (o.o_==i? constElm:changedElm) % int(o.o_));
    }
    
    string
    showTupElement(int i)
    {
      return lexical_cast<string>(i);
    }
    
    template<typename T>
    string
    showTupElement(T x)
    {
      return string(x);
    }
    
    
    /**
     * Helper template which acts as an "accessor".
     * Using the BuildTupleAccessor, we create a linear chain
     * of such TupleElementDisplayers as subclass of a given tuple type.
     * Here this technique is just used for dumping the tuples data fields,
     * but e.g. the control::Closure uses the same principle for manipulating
     * the individual datafields of an function argument tuple.  
     */
    template
      < typename TY
      , class BASE
      , class TUP
      , uint idx
      >
    class TupleElementDisplayer
      : public BASE
      {
        TY      & element()        { return BASE::template getAt<idx>(); }
        TY const& element()  const { return unConst(this)->template getAt<idx>(); }
        
      public:
        TupleElementDisplayer(TUP const& tuple) : BASE(tuple) {}
        
        string
        dump (string const& prefix ="(")  const
          {
            return BASE::dump (prefix+showTupElement(element())+",");
          }
      };
    
    template<class TUP>
    class TupleElementDisplayer<NullType, TUP, TUP, 0>
      : public TUP
      {
      public:
        TupleElementDisplayer(TUP const& tuple) : TUP(tuple) {}
        
        string
        dump (string const& prefix ="(")  const
          { 
            if (1 < prefix.length())
              // removing the trailing comma
              return prefix.substr (0, prefix.length()-1) +")";
            else
              return prefix+")";
          }
      };
    
  } // (END) Diagnostics Helper
  
  
  
  /* ===== printing Tuple types and contents ===== */ 
    
  template<typename TYPES>
  typename enable_if< is_TuplePlain<Tuple<TYPES> >,
    string          >::type
  showDump (Tuple<TYPES> const& tuple)
  {
    typedef BuildTupleAccessor<TYPES,TupleElementDisplayer> BuildAccessor;
    typedef typename BuildAccessor::Accessor Displayer;
    
    return "...Tuple" + Displayer(tuple).dump();
  }
  
  template<typename TYPES>
  typename enable_if< is_TupleListType<Tuple<TYPES> >,
    string          >::type
  showDump (Tuple<TYPES> const& tuple)
  {
    typedef typename Tuple<TYPES>::Type TypeSeq;
    Tuple<TypeSeq> plainTuple (tuple);
    
    typedef BuildTupleAccessor<TypeSeq, TupleElementDisplayer> BuildAccessor;
    typedef typename BuildAccessor::Accessor Displayer;
    
    return "...Tuple" + Displayer(plainTuple).dump();
  }
  
  template<typename TUP>
  typename enable_if< is_TuplePlain<TUP>,
    string          >::type
  showType ()
  {
    typedef InstantiateChained<typename TUP::ArgList, Printer, NullP>  DumpPrinter;
    return "TYPES-<>"
         + DumpPrinter::print();
  }
  
  template<typename TUP>
  typename enable_if< is_TupleListType<TUP>,
    string          >::type
  showType ()
  {
    typedef InstantiateChained<typename TUP::ArgList, Printer, NullP>  DumpPrinter;
    return "TYPES-[]"
         + DumpPrinter::print();
  }
  
  // see the macros DISPLAY and DUMPVAL defined in typelist-diagnostics.hpp
  
  
  
  
}}} // namespace lib::meta::test
#endif
