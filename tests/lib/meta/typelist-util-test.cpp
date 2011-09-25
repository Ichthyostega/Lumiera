/*
  TypeListUtil(Test)  -  check the simple typelist metaprogramming helpers 

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


#include "lib/test/run.hpp"
#include "lib/meta/typelist-util.hpp"



namespace lumiera {
namespace typelist{
namespace test {
  
  
  
  typedef Types< int
               , uint
               , int64_t
               , uint64_t
               >::List     TheList;
  
  typedef Types<  >::List  EmptyList;
  
  
  
  /*************************************************************************
   * @test verify the simple helpers for working with lists-of-types.
   *       These are simple metafunctions to count the number of elements,
   *       calculate the maximum size or check for inclusion.
   *       
   *       Because these metafunctions will be computed during compilation,
   *       mostly the function is already verified when it passes compilation.
   *       All we can do here, at runtime, is to verify some of the (expected)
   *       constant results.
   */
  class TypeListUtil_test : public Test
    {
      void
      run (Arg) 
        {
          CHECK (4 == count<TheList>::value);
          CHECK (0 == count<EmptyList>::value);
          
          CHECK (sizeof(int64_t) == maxSize<TheList>::value);
          CHECK (0               == maxSize<EmptyList>::value);
          
          CHECK ( bool(IsInList<int     , TheList>::value));
          CHECK ( bool(IsInList<uint    , TheList>::value));
          CHECK ( bool(IsInList<int64_t , TheList>::value));
          CHECK ( bool(IsInList<uint64_t, TheList>::value));
          
          CHECK (!bool(IsInList<double  , TheList>::value));
          CHECK (!bool(IsInList<int     , EmptyList>::value));
          CHECK (!bool(IsInList<int     , int>::value));         // Note: not-a-typelist yields false too
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (TypeListUtil_test, "unit common");
  
  
  
}}} // namespace lumiera::typelist::test
