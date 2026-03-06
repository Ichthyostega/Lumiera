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
 ** a static `show()` function. The string returned from this function
 ** visualises the structure of the typelist provided as parameter
 ** to the Printer template.
 ** 
 ** @see typelist-manip-test.cpp
 ** @see config-flags-test.cpp
 **
 */

#ifndef META_TYPELIST_DIAGNOSTICS_H
#define META_TYPELIST_DIAGNOSTICS_H


#include "test/test-helper.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/generator.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/meta/util.hpp"



namespace lib  {
namespace meta {
  
  using std::string;
  
  
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
  template<class X> struct CountDown         { using List = Nil; };
  template<>        struct CountDown<Num<0>> { using List = Node<Num<0>, Nil>; };
  template<int I>   struct CountDown<Num<I>> { using List = Node<Num<I>, typename CountDown<Num<I-1>>::List>; };
  
  
  
  
  namespace test{ //  unit tests covering typelist manipulating templates
    namespace  { // internals to support diagnostics in unit tests....
      
      
      using util::_Fmt;
      
      struct NullP
        {
          static string show() { return "-"; }
        };
      
      /** debugging template,
       *  printing the "number" used for instantiation on ctor call
       */
      template<class T=Nil, class BASE=NullP>
      struct Printer
        : BASE
        {
          static string show() { return _Fmt("-<%s>%s") % typeStr<T>() % BASE::show(); }
        };
      
      template<class BASE>
      struct Printer<Nil, BASE>
        : BASE
        {
          static string show() { return _Fmt("-<%s>%s") % "·" % BASE::show(); }
        };
      
      template<class BASE, int I>
      struct Printer<Num<I>, BASE>    ///< display the presence of a Num instance in the typelist
        : BASE
        {
          static string show() { return _Fmt("-<%u>%s") % uint(Num<I>::VAL) % BASE::show(); }
        };
      
      template<class BASE, uint Fl>
      struct Printer<Flag<Fl>, BASE>  ///< display the presence of a Flag in the typelist
        : BASE
        {
          static string show() { return _Fmt("-<%u>%s") % uint(Fl) % BASE::show(); }
        };
      
      template<class BASE>
      struct Printer<int, BASE>  ///< display the presence of a plain int in the typelist
        : BASE
        {
          static string show() { return _Fmt("-<%s>%s") % 'i' % BASE::show(); }
        };
      
      
      
      /** call the debug-print for a typelist
       *  utilising the Printer template */
      template<class LIST>
      string
      printSublist ()
      {
        using PrinterChain = InstantiateChained<LIST, Printer, NullP>;
        return PrinterChain::show();
      }
      
      /** Specialisation for debug-printing of a nested sublist */
      template<class TY, class TYPES, class BASE>
      struct Printer<Node<TY,TYPES>, BASE>
        : BASE
        {
          static string show()
            {
              typedef Node<TY,TYPES> List;
              return string("\n\t+--") + printSublist<List>()+"+"
                   + BASE::show();
            }
        };
      
      template<uint f1, uint f2, uint f3, uint f4, uint f5, class BASE>
      struct Printer<Config<f1,f2,f3,f4,f5>, BASE>
        : BASE
        {
          static string show()
            {
              using FlagList = Config<f1,f2,f3,f4,f5>::Flags;
              return string("\n\t+-Conf-[") + printSublist<FlagList>()+"]"
                   + BASE::show(); 
            }
        };
      
    } // (End) internal defs
    
    
    
    /* ===== printing types and contents ===== */
    
    template<typename TYPES>
    inline                  enable_if< is_Typelist<TYPES>,
    string                  >
    renderSeq()
    {
      using TypeList = TYPES::List;
      return printSublist<TypeList>();
    }
    
    //  Note: we define further overloads of this function for other types, especially Tuples
    
    
    
    
    
    /* ================= convenience macro notation ================= */
    
#define DISPLAY(_IT_)  \
        cout << STRINGIFY(_IT_) << "\t:" << renderSeq<_IT_>() << endl;
    
#define DUMPVAL(_IT_)  \
        cout << STRINGIFY(_IT_) << "\t:" << util::toString(_IT_) << endl;
    
#define EXPECT(_TY_, RENDERED_STRUCTURE )  \
        CHECK (renderSeq<_TY_>() == RENDERED_STRUCTURE ## _expect)
    
    
}}} // namespace lib::meta::test
#endif
