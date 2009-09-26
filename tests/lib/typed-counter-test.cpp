/*
  TypedCounter(Test)  -  managing a set of type based contexts
 
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
#include "lib/test/test-helper.hpp"
//#include "proc/control/command.hpp"
#include "proc/control/typed-allocation-manager.hpp"
//#include "proc/control/command-def.hpp"
//#include "lib/lumitime.hpp"
//#include "lib/symbol.hpp"
#include "lib/util.hpp"


#include <tr1/memory>
//#include <boost/ref.hpp>
//#include <boost/format.hpp>
//#include <iostream>
#include <cstdlib>
//#include <string>


namespace lib {
namespace test{


//  using boost::format;
//  using boost::str;
  //using lumiera::Time;
  //using util::contains;
  using std::tr1::shared_ptr;
//  using std::tr1::bind;
//  using std::string;
  using std::rand;
  //using std::cout;
  //using std::endl;
//  using lib::test::showSizeof;
  using util::isSameObject;
//  using util::contains;

//  using session::test::TestClip;
//  using lib::Symbol;
//  using lumiera::P;
  
  
  //using lumiera::typelist::BuildTupleAccessor;
//  using lumiera::error::LUMIERA_ERROR_EXTERNAL;
  
  namespace { // test data and helpers...
  
    long checksum_ = 0;
    
    /**
     * Yet-another fammily of dummy types....
     */
    template<uint siz>
    class DummyType
      {
        char crap_[siz];
      };
  }
  
  
  
  /***************************************************************************************
   * @test cover the basic implementation of a custom allocator, delegating to mpool.
   *       TypedAllocationManager is a base class, used e.g. to build the CommandRegistry.
   * 
   * @todo planned but not implemented as of 8/09   see also Ticket #217
   *        
   * @see CommandRegistry
   * @see command-registry-test.cpp
   * @see allocationclustertest.cpp
   */
  class TypedCounter_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          UNIMPLEMENTED ("type based contexts");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TypedCounter_test, "unit common");
  
  
}} // namespace lib::test
