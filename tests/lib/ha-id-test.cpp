/*
  HaID(Test)  -  proof-of-concept test for a hash based and typed ID
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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

#include "lib/ha-id.hpp"

#include <boost/format.hpp>
#include <iostream>

using boost::format;
//using std::string;
using std::cout;


namespace lib {
namespace test{
  
  /* ==  a hierarchy of test-dummy objects to use the HaID == */
  
  struct Base
    {
      int ii_;
    };
  
  struct TestA : Base, HaIndexed<TestA>
    {
    };
  struct TestBA : TestA {};
  struct TestBB : TestA {};
  
  
  
  
  /***************************************************************************
   * @test proof-of-concept test for a generic hash based and typed ID struct.
   * @see  lib::HaID
   */
  class HaID_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          format fmt ("sizeof( %s ) = %d\n");
          
          /////////////////////////////////TODO
          HaID<TestBB,TestA> hahaBB1;
          
          TestBA bab;
          bab.resetID(hahaBB1);
          
          HaID<TestBA,TestA> hahaBA1 (bab);
          
          cout << fmt % "TestBA"     % sizeof(bab);
          cout << fmt % "ID<TestBA>" % sizeof(hahaBA1);
          cout << fmt % "ID<TestBB>" % sizeof(hahaBB1);
          
          ASSERT (hahaBA1.dummy_ == hahaBB1.dummy_);
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (HaID_test, "unit common");
      
      
}} // namespace lib::test
