/*
  TYPELIST-DIAGNOSTICS  -  helper for testing the typelist based utilities

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file typelist-diagnostics.hpp
 ** Support for writing metaprogramming unit-tests dealing with typelists and flags.  
 ** a Printer template usable for debugging the structure of a typelist built
 ** upon some simple debugging-style types. Examples being a Num<int> template,
 ** or the Flag type. A Printer type generated from this template provides
 ** a static \c print() function. The string returned from this function
 ** visualises the structure of the typelist provided as parameter
 ** to the Printer template.
 ** 
 ** @see typelist-manip-test.cpp
 ** @see config-flags-test.cpp
 **
 */

#ifndef META_TYPELIST_DIAGNOSTICS_H
#define META_TYPELIST_DIAGNOSTICS_H


#include "lib/meta/typelist.hpp"
#include "lib/meta/generator.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/meta/util.hpp"

#include <string>


using std::string;


namespace lib  {
namespace meta {
  
  /** dummy interface / baseclass for diagnostics */
  struct Numz
    {
      uint o_;
      Numz (uint x =0) : o_(x) { }
      operator uint ()  const { return o_; }
    };
  
  
  /** constant-wrapper type for debugging purposes,
   *  usable for generating lists of distinguishable types
   */
  template<int I>
  struct Num : Numz
    {
      enum{ VAL=I };
      
      Num (uint x = uint(I)) : Numz(x) { }
    };
  
  
  
  /* some forwards used by config-flags-test.cpp */
  template<uint bit> struct Flag;
  template< uint f1
          , uint f2
          , uint f3
          , uint f4
          , uint f5
          >
  struct Config;
  
  
  
  /** helper for generating test lists */      
  template<class X> struct CountDown         { typedef NullType List; };
  template<>        struct CountDown<Num<0>> { typedef Node<Num<0>, NullType> List; };
  template<int I>   struct CountDown<Num<I>> { typedef Node<Num<I>, typename CountDown<Num<I-1>>::List> List; };
  
  
  
  
  namespace test{ //  unit tests covering typelist manipulating templates
    namespace  { // internals to support diagnostics in unit tests....
      
      
      using util::_Fmt;
      
      struct NullP
        {
          static string print () { return "-"; }
        };
      
      /** debugging template, 
       *  printing the "number" used for instantiation on ctor call
       */
      template<class T=NullType, class BASE=NullP>
      struct Printer
        : BASE
        {
          static string print () { return _Fmt("-<%s>%s") % typeStr<T>() % BASE::print(); }
        };
      
      template<class BASE>
      struct Printer<NullType, BASE>
        : BASE
        {
          static string print () { return _Fmt("-<%u>%s") % "·" % BASE::print(); }
        };
      
      template<class BASE, int I>
      struct Printer<Num<I>, BASE>    ///< display the presence of a Num instance in the typelist
        : BASE
        {
          static string print () { return _Fmt("-<%u>%s") % uint(Num<I>::VAL) % BASE::print(); }
        };
      
      template<class BASE, uint Fl>
      struct Printer<Flag<Fl>, BASE>  ///< display the presence of a Flag in the typelist
        : BASE
        {
          static string print () { return _Fmt("-<%u>%s") % uint(Fl) % BASE::print(); }
        };
      
      template<class BASE>
      struct Printer<int, BASE>  ///< display the presence of a plain int in the typelist
        : BASE
        {
          static string print () { return _Fmt("-<%u>%s") % 'i' % BASE::print(); }
        };
      
      
      
      /** call the debug-print for a typelist
       *  utilising the Printer template */
      template<class L>
      string
      printSublist ()
      {
        typedef InstantiateChained<L, Printer, NullP> SubList;
        return SubList::print();
      }
      
      /** Specialisation for debug-printing of a nested sublist */
      template<class TY, class TYPES, class BASE>
      struct Printer<Node<TY,TYPES>, BASE>
        : BASE
        {
          static string print () 
            {
              typedef Node<TY,TYPES> List;
              return string("\n\t+--") + printSublist<List>()+"+"
                   + BASE::print(); 
            }
        };
      
      template<uint f1, uint f2, uint f3, uint f4, uint f5, class BASE>
      struct Printer<Config<f1,f2,f3,f4,f5>, BASE>
        : BASE
        {
          static string print () 
            {
              typedef typename Config<f1,f2,f3,f4,f5>::Flags FlagList;
              return string("\n\t+-Conf-[") + printSublist<FlagList>()+"]"
                   + BASE::print(); 
            }
        };
      
    } // (End) internal defs
    
    
    
    /* ===== printing types and contents ===== */ 
    
    template<typename TYPES>
    inline                  enable_if< is_Typelist<TYPES>,
    string                  >
    showType ()
    {
      typedef InstantiateChained<typename TYPES::List, Printer, NullP>  DumpPrinter;
      return DumpPrinter::print();
    }
    
    //  Note: we define overloads of this function for other types, especially Tuples
    
    
#define DISPLAY(_IT_)  \
        cout << STRINGIFY(_IT_) << "\t:" << showType<_IT_>() << endl;
    
#define DUMPVAL(_IT_)  \
        cout << STRINGIFY(_IT_) << "\t:" << util::toString(_IT_) << endl;
    
    
    
    
    
}}} // namespace lib::meta::test
#endif
