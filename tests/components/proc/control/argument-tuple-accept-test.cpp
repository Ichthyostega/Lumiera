/*
  ArgumentTupleAccept(Test)  -  verify synthesising a bind(...) function
 
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
//#include "proc/asset/media.hpp"
//#include "proc/mobject/session.hpp"
//#include "proc/mobject/session/edl.hpp"
//#include "proc/mobject/session/testclip.hpp"
//#include "proc/mobject/test-dummy-mobject.hpp"
//#include "lib/p.hpp"
//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/placement-index.hpp"
//#include "proc/mobject/explicitplacement.hpp"
#include "proc/control/argument-tuple-accept.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/tuple.hpp"

//#include "lib/scoped-ptrvect.hpp"
//#include "lib/lumitime-fmt.hpp"
//#include "lib/meta/typelist.hpp"
//#include "lib/meta/tuple.hpp"
//#include "lib/util.hpp"

#include <tr1/functional>
//#include <boost/format.hpp>
#include <iostream>
//#include <strstream>
//#include <cstdlib>
//#include <string>

//using std::tr1::bind;
//using std::tr1::placeholders::_1;
//using std::tr1::placeholders::_2;
using std::tr1::function;
//using util::isnil;
//using util::and_all;
//using boost::format;
//using lumiera::Time;
//using util::contains;
//using std::string;
//using std::rand;
//using std::ostream;
//using std::ostrstream;
using std::cout;
using std::endl;


namespace control {
namespace test    {
  
  using lib::test::showSizeof;
  
  using lumiera::typelist::FunctionSignature;
  using lumiera::typelist::Tuple;
//  using session::test::TestClip;
//  using lumiera::P;
//  using namespace lumiera::typelist;
//  using lumiera::typelist::Tuple;
  
//  using control::CmdClosure;
  
  
  
  
  
  
  namespace { // build a test dummy class....
    
    template<typename SIG>
    struct Tup
      {
        typedef typename FunctionSignature< function<SIG> >::Args Args;
        typedef Tuple<Args> Ty;
      };
    
    
    template<typename SIG>
    class TestClass
      : public ArgumentTupleAccept< SIG                     // to derive the desired signature
                                  , TestClass               // the target class providing the implementation
                                  , typename Tup<SIG>::Ty   // base class to inherit from
                                  >
      {
        void
        bind (typename Tup<SIG>::Ty const& tuple)
          {
            *this = tuple;
          }
      };
    
    
    Time
    randTime ()
      {
        UNIMPLEMENTED ("create a random but not insane Time value");
      }
    
    
  } // test-helper implementation
  
  
  
  
  
  typedef lib::ScopedPtrVect<CmdClosure> ArgTuples;
  
  /***************************************************************
   * @test Build test object, which accepts a bind(...) call with 
   *       specifically typed arguments.
   *       
   * @see  control::CommandArgumentHolder
   */
  class ArgumentTupleAccept_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          TestClass<void()> testVoid;
          TestClass<int(Time,int)> testTime;
          
          testVoid.bind();
          testTime.bind(randTime(),23);
          
          cout << showSizeof(testVoid) << endl;
          cout << showSizeof(testITim) << endl;
          
          cout << testITim.getHead() << endl;
          ASSERT (23 == testITim.getTail().getHead());
        }
      
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (ArgumentTupleAccept_test, "unit controller");
  
  
}} // namespace control::test
