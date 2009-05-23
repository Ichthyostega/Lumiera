/*
  HashIndexed(Test)  -  proof-of-concept test for a hash based and typed ID
 
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

#include "lib/hash-indexed.hpp"

//#include <boost/format.hpp>
#include <iostream>

//using boost::format;
//using std::string;
using std::cout;


namespace lib {
namespace test{
  
  /* ==  a hierarchy of test-dummy objects to use the HashIndexed::ID == */
  
  struct Base
    {
      long ii_;
    };
  
  struct TestB : Base, HashIndexed<TestB,LuidH> ///< Base class to mix in the hash ID facility
    {
      TestB () {}
      TestB (ID const& refID) : HashIndexed<TestB,LuidH>(refID) {}
    };
  struct TestDA : TestB {};
  struct TestDB : TestB {};
  
  
  
  
  /***************************************************************************
   * @test proof-of-concept test for a generic hash based and typed ID struct.
   * @see  lib::HashIndexed::Id
   */
  class HashIndexed_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          TestB::Id<TestDA> idDA;
          
          TestB bb (idDA);
          
          TestB::Id<TestDB> idDB1 ;
          TestB::Id<TestDB> idDB2 (idDB1);
          
          ASSERT (sizeof (idDB1)     == sizeof (idDA) );
          ASSERT (sizeof (TestB::ID) == sizeof (LuidH));
          ASSERT (sizeof (TestDA)    == sizeof (LuidH) + sizeof (Base));
          
          ASSERT (idDA  == bb.getID() );
          ASSERT (idDB1 == idDB2 );            // equality handled by the hash impl (here LuidH)
          
          TestDA d1;
          TestDA d2;
          ASSERT (d1.getID() != d2.getID());   // should be different because LUIDs are random
          
          d2 = d1; 
          ASSERT (d1.getID() == d2.getID());   // default assignment operator works as expected
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (HashIndexed_test, "unit common");
      
      
}} // namespace lib::test
