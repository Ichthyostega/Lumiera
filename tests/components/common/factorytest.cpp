/*
  Factory(Test)  -  unittest for the object creating factory
 
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


#include "common/test/run.hpp"


namespace cinelerra
  {
  namespace test
    {
    
    /**
     * Target object to be created by the Test-Factory
     */
    class TargetObj
      {
      public:
      };
      
    /**********************************************************
     * @test the various object creation factories
     * @see  cinelerra::Factory
     * @todo still to be written...
     */
    class Factory_test : public Test
      {
        virtual void run(Arg arg) 
          {
          } 
      };
    
    /** Register this test class to be invoked in some test groups (suites) */
    Launch<Factory_test> run_Factory_test("Factory_test","unit common");
    
    
    
  } // namespace test

} // namespace cinelerra
