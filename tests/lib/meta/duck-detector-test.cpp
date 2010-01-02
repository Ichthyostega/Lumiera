/*
  DuckDetector(Test)  -  detecting properties of a type at compile time
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "lib/test/run.hpp"
#include "lib/meta/duck-detector.hpp"
#include "lib/util.hpp"  /////////////TODO

#include <iostream>


namespace lib {
namespace meta{
namespace test{

  //using std::string;
  using std::cout;
  using std::endl;
  
  
  namespace { // some test ducks....
    
    struct PropperGander
      {
        class Core;
        
        PropperGander& honk();
      };
    
    struct Propaganda
      {
        void honk();
      };

#define DO_CHECK(_EXPR_) cout << STRINGIFY(_EXPR_) << "\t = " << _EXPR_::value << endl;    
    
  }//(End) with test ducks
  
  
  /***********************************************************************************
   * @test verify building predicates to detect properties of a type at compile time.
   *       Especially, this allows us to detect if a type in question
   *       - has a nested type with a specific name
   *       - has a member with a specific name
   *       - defines a function with a specific signature
   */
  class DuckDetector_test : public Test
    {
      void
      run (Arg) 
        {
          DO_CHECK( DetectNested<PropperGander> );
          DO_CHECK( DetectNested<Propaganda>    );
          
          DO_CHECK( DetectMember<PropperGander> );
          DO_CHECK( DetectMember<Propaganda>    );
          
          DO_CHECK( DetectFunSig<PropperGander> );
          DO_CHECK( DetectFunSig<Propaganda>    );
          
          UNIMPLEMENTED ("detect the propaganda");
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (DuckDetector_test, "unit common");
  
  
  
}}} // namespace lib::meta::test
