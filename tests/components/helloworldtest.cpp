/*
  HelloWorld(Test)  -  how to use this test framework...
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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


#include <iostream>
#include "common/test/run.hpp"


namespace cinelerra
  {
  namespace test
    {

    /******************************************
     * Helooooooo to the world of TDD
     * @test demo of using the test framework
     */
    class HelloWorld_test : public Test
      {
        virtual void run(Arg arg) 
        {
          greeting();
        } 
        
        void greeting() 
        { 
          std::cout << "This is how the world ends...\n"; 
        }
      };
    
      

    /** Register this test class to be invoked in some test groups (suites) */
    Launch<HelloWorld_test> run_HelloWorld_test("HelloWorld_test","unit common");
    
    // NOTE: you may use the Macro "LAUNCHER" in run.hpp to simplify this Registration
    
  } // namespace test

} // namespace cinelerra
