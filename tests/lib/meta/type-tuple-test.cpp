/*
  TypeTuple(Test)  -  checking type tuples and records based on them
 
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
 
* *****************************************************/


/** @file type-tuple-test.cpp
 ** Interconnection of typelists, type tuples and simple record
 ** data types build on top of them.  
 ** @todo define function-closure-test
 **
 ** @see lumiera::typelist::Tuple
 ** @see tuple.hpp
 ** @see function-closure.hpp
 ** @see control::CmdClosure real world usage example
 **
 */


#include "lib/test/run.hpp"
#include "lib/meta/typelist.hpp"   ////////////TODO really?
#include "lib/meta/tuple.hpp"
#include "meta/typelist-diagnostics.hpp"
#include "lib/util.hpp"

#include <boost/utility/enable_if.hpp>
#include <boost/format.hpp>
#include <iostream>

using ::test::Test;
  
using boost::enable_if;
using boost::format;
using boost::str;
using util::unConst;
using std::string;
using std::cout;
using std::endl;


namespace lumiera {
namespace typelist{
namespace test {
      
  
      
      namespace { // test data
        
        
        
        typedef Types< Num<1>
                     , Num<3>
                     , Num<5>
                     >          Types1;
        typedef Types< Num<2>
                     , Num<4>
                     >          Types2;
        typedef Types< Num<7> > Types3;
        
        
        
      } // (End) test data
  
  
  namespace { // Diagnostics
    
    
    
    template<int i>
    string
    showTupElement(Num<i> o) 
    {
      static format   constElm("(%i)");
      static format changedElm("{%i}");
      
      return str ( (o.o_==i? constElm:changedElm) % int(o.o_));
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
        dump (string const& prefix = "(")  const
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
        
        
      protected:
        
        string
        dump (string const& prefix)  const
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
  string
  showDump (Tuple<TYPES> const& tuple)
  {
    typedef BuildTupleAccessor<TYPES,TupleElementDisplayer> BuildAccessor;
    typedef typename BuildAccessor::Accessor Displayer;
    
    return " Tup" + Displayer(tuple).dump();
  }
  
  template<typename TUP>
  typename enable_if< is_Tuple<TUP>,
    string          >::type
  showType ()
  {
    typedef InstantiateChained<typename TUP::ArgList, Printer, NullP>  DumpPrinter;
    return "TYPES-<>"
         + DumpPrinter::print();
  }
  
  // see the macros DISPLAY and DUMPVAL defined in typelist-diagnostics.hpp
  
  
  
  
  
  /*************************************************************************
   * @test //////////////////////////////////////////
   *       - building combinations and permutations
   */
  class TypeTuple_test : public Test
    {
      virtual void
      run (Arg) 
        {
          check_diagnostics();
          
          UNIMPLEMENTED ("verify type tuples");
        }
      
      
      /** verify the test input data
       *  @see TypeListManipl_test#check_diagnostics()
       *       for an explanation of the DISPLAY macro
       */
      void
      check_diagnostics ()
        {
          typedef Types1::List L1;
          typedef Types2::List L2;
          typedef Types3::List L3;
          
          DISPLAY (L1);
          DISPLAY (L2);
          DISPLAY (L3);
          
          typedef Tuple<Types3> Tup3;
          
          DISPLAY (Tup3);
          DUMPVAL (Tup3());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TypeTuple_test, "unit common");
  
  
  
}}} // namespace lumiera::typelist::test
