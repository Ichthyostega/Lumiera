/*
  TestHelperVariadic(Test)  -  verify variadic template diagnostics helper

  Copyright (C)         Lumiera.org
    2014,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file test-helper-variadic-test.cpp
 ** unit test \ref TestHelperVariadic_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"


#include <iostream>
#include <utility>
#include <string>
#include <cmath>

using std::string;
using std::cout;


namespace lib {
namespace test{
namespace test{
  
  namespace { // test fixture...
    
    class Interface
      {
      public:
        Interface(){}
        Interface(Interface const& o) { cout << "COPY.CT from "<<&o<<" !!!\n"; }
        Interface(Interface const&& o) { cout << "MOVE.CT from "<<&o<<" !!!\n"; }
        
        Interface& operator= (Interface const& o) { cout << "COPY= from "<<&o<<" !!!\n"; return *this; }
        Interface& operator= (Interface const&& o) { cout << "MOVE= from "<<&o<<" !!!\n"; return *this; }
        
        virtual ~Interface() { }
      };
    
    class Impl
      : public Interface
      {
        string s_;
        
      public:
        Impl(string ss ="ZOMG") : s_(ss) { }
      };
    
    
    inline double
    makeRvalue()
      {
        return atan2(0,-0.0);
      }
    
    
  }//(End) test fixture
  
  
  
  
  
  
  /****************************************************************************//**
   * @test document usage of the diagnostics helper for variadic templates.
   *       Errors in variadic template instantiations are sometimes hard to spot,
   *       due to the intricacies of template argument matching and the
   *       reference collapsing rules. Our diagnostics facility is itself a
   *       variadic function template, which, when supplied with an argument pack,
   *       will build a diagnostic string describing the arguments.
   * @warning care has to be taken to pass the template arguments properly,
   *       since template argument matching might mess up the reference kind
   *       (rvalue, lvalue) of the passed types. Thus, either use std::forward,
   *       or spell out the template argument(s) explicitly on invocation
   *       
   * @see showVariadicTypes()
   * @see TestHelper_test
   */
  class TestHelperVariadic_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          double d = makeRvalue();
          double const& cr = d;
          
          Impl obj;
          Interface const& ref = obj;
          
          cout << "--no-arg--\n"    << showVariadicTypes() <<"\n";
          cout << "--reference--\n" << showVariadicTypes(d) <<"\n";
          cout << "--value--\n"     << showVariadicTypes(makeRvalue()) <<"\n";
          
          forwardFunction("two values", "foo", 42L);         // displayed as char [4] const&, long &&
          forwardFunction("references", d,cr,std::move(d));  // displayed as double&, double const&, double &&
          
          forwardFunction("baseclass", ref);                 // displayed as Interface const&
        }
      
      
      /** this dummy simulates a typical variadic call
       *  which takes all arguments as '&&' for the purpose of "perfect forwarding"
       */
      template<typename... ARGS>
      void
      forwardFunction (string id, ARGS&&... args)
        {
          cout << "--"<<id<<"--\n"
               << showVariadicTypes (std::forward<ARGS>(args)...)
               << "\n"
               ;
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (TestHelperVariadic_test, "unit common");
  
  
}}} // namespace lib::test::test
