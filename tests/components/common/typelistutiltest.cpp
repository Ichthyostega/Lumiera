/*
  TypeListUtil(Test)  -  check the typelist helpers
 
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


/** @file typelistutiltest.cpp
 ** \par what are we doing here??
 **
 ** the following test composes both an interface and the corresponding implementation
 ** by instantiating "building block" templates over a collection of types. The resulting
 ** class ends up inheriting a \e virtual function instantiated for each of the types 
 ** in the list. (remember: normally the number and signature of all virtual functions
 ** need to be absolutely fixed in the class definition)
 **
 ** @see typelisttest.cpp
 ** @see typelistutil.hpp
 ** @see lumiera::query::ConfigRules  a real world usage example
 **
 */


#include "common/test/run.hpp"
#include "common/typelistutil.hpp"

#include <boost/format.hpp>
#include <iostream>

using std::string;
using std::cout;


namespace lumiera
  {
  namespace typelist
    {
    namespace test
      {
      
      /** template for generating lots of different test types */
      template<int I>
      struct Block    
        { 
          static string name;
          string talk()   { return name+"::eat(..)"; }  
        };
      
      
      boost::format fmt ("Block<%2i>");
        
      template<int I>
      string Block<I>::name = str (fmt % I);
      
      
      
      /** Use this building block for assembling an abstract interface */
      template<class X>
      class TakeIt
        {
        public:
          virtual void eat (X& x) = 0;
          virtual ~TakeIt() { }
        };
      
      /** Use this building block for chaining corresponding implementation classes. */
      template<class X, class BASE>
      class DoIt
        : public BASE
        {
        protected:
          DoIt ()         { cout << "ctor DoIt<"<< X::name << " >\n";}
          virtual ~DoIt() { cout << "dtor DoIt<"<< X::name << " >\n";}
        public:
          void eat (X& x) { cout << x.talk() << "\n";}
          using BASE::eat; // prevent shaddowing
        };
      
      typedef Types< Block<1>
                   , Block<2>
                   , Block<3>
                   , Block<5>
                   , Block<8>
                   , Block<13>
                   >::List TheTypes;
      
      typedef InstantiateForEach<TheTypes,TakeIt>  TheInterface;
      
      
      struct BaseImpl : public TheInterface
        {
          void eat() { cout << "gulp!\n"; }
        };
      
      typedef InstantiateChained<TheTypes,DoIt, BaseImpl>  NumberBabbler;
      
      
      /*************************************************************************
       * @test check the helpers for dealing with lists-of-types.
       *       <ul><li>build an interface and an implementation class 
       *               by inheriting template instantiations 
       *               for a collection of classes</li>
       *       </ul>
       */
      class TypeListUtil_test : public Test
        {
          virtual void run(Arg arg) 
            {
              NumberBabbler me_can_has_more_numberz;
              
              ASSERT (INSTANCEOF (TheInterface, &me_can_has_more_numberz));  
              
              TheTypes::Tail::Head                          b2;   //  Block<2>
              TheTypes::Tail::Tail::Tail::Head              b5;   //  Block<5>
              TheTypes::Tail::Tail::Tail::Tail::Tail::Head  b13;  //  Block<13>

              me_can_has_more_numberz.eat (b2);
              me_can_has_more_numberz.eat (b5);
              
              TakeIt<Block<13> >& subInterface = me_can_has_more_numberz;
              
              subInterface.eat (b13);
              me_can_has_more_numberz.eat();
              
              INFO (test, "SizeOf = %i", sizeof(me_can_has_more_numberz));
            } 
        };
      
      
      /** Register this test class... */
      LAUNCHER (TypeListUtil_test, "unit common");
      
      
      
    } // namespace test
    
  } // namespace typelist

} // namespace lumiera
