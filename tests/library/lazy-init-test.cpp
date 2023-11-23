/*
  LazyInit(Test)  -  verify a mechanism to install a self-initialising functor

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file lazy-init-test.cpp
 ** unit test \ref LazyInit_test
 */



#include "lib/test/run.hpp"
#include "lib/lazy-init.hpp"
//#include "lib/format-string.hpp"
//#include "lib/test/test-helper.hpp"
#include "lib/test/diagnostic-output.hpp" /////////////////////TODO TODOH

//#include <array>



namespace lib {
namespace test{
  
//  using util::_Fmt;
//  using lib::meta::_FunRet;
//  using err::LUMIERA_ERROR_LIFECYCLE;
  
  
  
  namespace { // policy and configuration for test...

    //
  }//(End) Test config


  
  
  
  /***********************************************************************************//**
   * @test Verify a mix-in to allow for lazy initialisation of complex infrastructure
   *       tied to a std::function; the intention is to have a »trap« hidden in the
   *       function itself to trigger on first use and perform the one-time
   *       initialisation, then finally lock the object in place.
   * @see lazy-init.hpp
   * @see lib::RandomDraw
   */
  class LazyInit_test
    : public Test
    {
      
      void
      run (Arg)
        {
          simpleUse();
          
          verify_inlineFunctorStorage();
//          verify_numerics();
//          verify_adaptMapping();
//          verify_dynamicChange();
        }
      
      
      
      /** @test demonstrate a basic usage scenario
       */
      void
      simpleUse()
        {
        }
      
      
      
      /** @test verify that std::function indeed stores a simple functor inline
       */
      void
      verify_inlineFunctorStorage()
        {
        }
      
      
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (LazyInit_test, "unit common");
  
  
}} // namespace lib::test
