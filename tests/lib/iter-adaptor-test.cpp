/*
  IterAdaptor(Test)  -  building simple iterators for a given container
 
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
#include "lib/util.hpp"

//#include "lib/scoped-ptrvect.hpp"
//#include "testdummy.hpp"


namespace lib {
namespace test{
  
  using ::Test;
  using util::isnil;
  
  
  
  /********************************************************************
   *  @test create an iterator element for a given container and
   *        verify its behaviour in accordance to the concept
   *        "lumiera forward iterator"
   * @todo see Ticket #182
   */
  class IterAdaptor_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          UNIMPLEMENTED ("build a simple lumiera forward iterator");
          simpleUsage();
//        iterating();
//        detaching();
        }
      
      
      
      void
      simpleUsage()
        {
        }
      
    };
  
  LAUNCHER (IterAdaptor_test, "unit common");
  
  
}} // namespace lib::test

