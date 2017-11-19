/*
  DuckDetector(Test)  -  detecting properties of a type at compile time

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file duck-detector-test.cpp
 ** unit test \ref DuckDetector_test:
 ** duck typing through metaprogramming
 */


#include "lib/test/run.hpp"
#include "lib/meta/duck-detector.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"


namespace lib {
namespace meta{
namespace test{
  
  
  namespace { // some test ducks....
    
    
    struct PropperGander
      {
        class Core;
        
        PropperGander& honk (long,long);
      };
    
    
    struct Propaganda
      {
        void honk(float);
      };
    
    
  }//(End) with test ducks
  
#define SHOW_CHECK(_EXPR_) cout << STRINGIFY(_EXPR_) << "\t : " << (_EXPR_::value? "Yes":"No") << endl;
  
  
  
  /*******************************************************************************//**
   * @test verify building predicates to detect properties of a type at compile time.
   *       Especially, this allows us to detect if a type in question
   *       - has a nested type with a specific name
   *       - has a member with a specific name
   *       - defines a function with a specific signature
   */
  class DuckDetector_test : public Test
    {
    
      META_DETECT_NESTED(Core);
      META_DETECT_MEMBER(honk);
      META_DETECT_FUNCTION(PropperGander&, honk, (long,long));
    
    
      void
      run (Arg) 
        {
          SHOW_CHECK( HasNested_Core<PropperGander> );
          SHOW_CHECK( HasNested_Core<Propaganda>    );
          
          SHOW_CHECK( HasMember_honk<PropperGander> );
          SHOW_CHECK( HasMember_honk<Propaganda>    );
          
          SHOW_CHECK( HasFunSig_honk<PropperGander> );
          SHOW_CHECK( HasFunSig_honk<Propaganda>    );
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (DuckDetector_test, "unit meta");
  
  
  
}}} // namespace lib::meta::test
