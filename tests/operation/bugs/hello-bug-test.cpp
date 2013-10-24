/*
  errortest.c  -  executable for running bug regression tests

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

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

#include <iostream>

using std::cerr;
using std::endl;


namespace lumiera{
namespace test   {
  
  
  /**************************************//**
   * @todo come up with some more bugs
   * @test cover known regressions
   */
  class HelloBug_test
    : public Test
    {
      virtual void
      run(Arg) 
      {
        std::cerr << "hello sunshine, no bugs whatsoever" <<endl;
      } 
    };
  
  
  
  
  
  /** Register this test class... */
  LAUNCHER (HelloBug_test, "function regression");
  
  
  
}} // namespace lumiera::test
