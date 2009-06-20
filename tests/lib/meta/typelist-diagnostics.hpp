/*
  TYPELIST-DIAGNOSTICS  -  helper for testing the typelist based utilities
 
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
 
* *****************************************************/


/** @file typelist-diagnostics.hpp
 ** a Printer template usable for debugging the structure of a typelist built
 ** upon some simple debugging-style types. Examples being a Num<int> template,
 ** or the Flag type. A Printer type generated from this template provides
 ** a static \c print() function returning a string visualising the structure
 ** of the typelist provided as parameter to the Printer template.
 **
 ** @see typelist-manip-test.cpp
 ** @see config-flags-test.cpp
 **
 */
#ifndef META_TYPELIST_DIAGNOSTICS_H
#define META_TYPELIST_DIAGNOSTICS_H


#include "lib/meta/typelist.hpp"
#include "lib/meta/generator.hpp"

#include <boost/format.hpp>

using std::string;
using boost::format;


namespace lumiera {
namespace typelist{
  
  /** dummy interface / baseclass for diagnostics */
  struct Numz
    {
      char o_;
      Numz (char x =0) : o_(x) { }
    };


  /** constant-wrapper type for debugging purposes,
   *  usable for generating lists of distinguishable types
   */
  template<int I>
  struct Num : Numz
    {
      enum{ VAL=I };
      
      Num (char x = char(I)) : Numz(x) { }
    };
  
  
  
  /* some forwards used by config-flags-test.cpp */
  template<char bit> struct Flag;
  template< char f1
          , char f2
          , char f3
          , char f4
          , char f5
          >
  struct Config;
  
  
        
  /** helper for generating test lists */      
  template<class X> struct CountDown          { typedef NullType List; };
  template<>        struct CountDown<Num<0> > { typedef Node<Num<0>, NullType> List; };
  template<int I>   struct CountDown<Num<I> > { typedef Node<Num<I>, typename CountDown<Num<I-1> >::List> List; };
  
  
  
  namespace test { //< unit tests covering typelist manipulating templates
    namespace {   // hidden internals for diagnostics....
      
      using boost::format;
      
      format fmt ("-<%u>%s");
      
      struct NullP
        {
          static string print () { return "-"; }
        };
      
      /** debugging template, 
       *  printing the "number" used for instantiation on ctor call
       */
      template<class NUM=NullType, class BASE=NullP>
      struct Printer;
      
      template<class BASE>
      struct Printer<NullType, BASE>
        : BASE
        {
          static string print () { return str( fmt % "·" % BASE::print()); }
        };
      
      template<class BASE, int I>
      struct Printer<Num<I>, BASE>    ///< display the presence of a Num instance in the typelist
        : BASE
        {
          static string print () { return str( fmt % uint(Num<I>::VAL) % BASE::print()); }
        };
      
      template<class BASE, char Fl>
      struct Printer<Flag<Fl>, BASE>  ///< display the presence of a Flag in the typelist
        : BASE
        {
          static string print () { return str( fmt % uint(Fl) % BASE::print()); }
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
      
      template<char f1, char f2, char f3, char f4, char f5, class BASE>
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
      
      
#define DIAGNOSE(LIST) \
        typedef InstantiateChained<LIST::List, Printer, NullP>  Contents_##LIST;
                     
#define DISPLAY(NAME)  \
        DIAGNOSE(NAME); cout << STRINGIFY(NAME) << "\t:" << Contents_##NAME::print() << "\n";
      
      
      
    } // (End) internal defs
    
    
    
}}} // namespace lumiera::typelist::test
#endif
