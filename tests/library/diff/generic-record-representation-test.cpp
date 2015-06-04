/*
  GenericRecordRepresentation(Test)  -  introspective representation of object-like data

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "lib/test/test-helper.hpp"
#include "lib/diff/record.hpp"
#include "lib/util.hpp"       //////TODO necessary?

#include <iostream>
//#include <utility>
//#include <string>
#include <vector>

//using std::string;
using util::isSameObject;
using std::vector;
//using std::swap;
using std::cout;
using std::endl;


namespace lib {
namespace diff{
namespace test{
  
//  using lumiera::error::LUMIERA_ERROR_LOGIC;
  
  namespace {//Test fixture....
    
    
    
  }//(End)Test fixture
  
  using Seq  = vector<string>;
  using RecS = Record<String>;
  
  
  
  
  
  
  
  /*************************************************************************************//**
   * @test Verify properties of a special collection type meant for external representation
   *       of object-like data.
   *       
   * @see IndexTable
   * @see DiffListApplication_test
   */
  class GenericRecordRepresentation_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          verifyCreation();
          verifyMutations();
          copy_and_move();
          equality();
        }
      
      
      void
      simpleUsage()
        {
          RecS enterprise("starship"
                         , {"Name = USS Enterprise"
                           ,"Registry = NCC-1701-D"
                           ,"Class = Galaxy"
                           ,"Owner = United Federation of Planets"
                           ,"built=2363"
                           }
                         , {"Picard", "Riker", "Data", "Worf", "Troi", "Crusher", "La Forge"}
                         );
          
          CHECK (enterprise.getType() == "starship");
          CHECK (enterprise.get("Registry") == "NCC-1701-D");
          
          CHECK (enterprise.hasAttribute("Owner"));
          CHECK (!enterprise.hasAttribute("owner"));
          CHECK (!enterprise.hasAttribute("Owner "));
          
          CHECK (enterprise.contains("Data"));
          CHECK (!contains (enterprise, "Woof"));
          CHECK (util::contains (enterprise, "Worf"));
          
          VERIFY_ERROR (INVALID, enterprise.get("warp10"));
          
          cout << "enterprise = " << string(enterprise)<<endl;
          for (string elm : enterprise)             cout << elm<<endl;
          for (string mbr : enterprise.scope())     cout << mbr<<endl;
          for (auto attr : enterprise.attributes()) cout << attr<<endl;
        }
      
      
      void
      verifyCreation()
        {
          RecS nil;
          CHECK (isnil(nil));
          CHECK ("NIL" == nil.getType());
          
          CHECK (!nil.begin());
          CHECK (nil.begin() == nil.end());
          
          
          RecS untyped({"x"});
          CHECK (!isnil(untyped));
          CHECK ("NIL" == untyped.getType());
          CHECK (Seq{"x"} == contents(untyped));
          CHECK (Seq{"x"} == contents(untyped.scope()));
          CHECK (isnil (untyped.attributes()));
          
          RecS untyped2({"x=y", "z"});
          CHECK (!isnil(untyped2));
          CHECK ("NIL" == untyped2.getType());
          CHECK (Seq({"x=y", "z"}) == contents(untyped2));
          CHECK (Seq{"x"} == contents (untyped2.keys()));
          CHECK (Seq{"y"} == contents (untyped2.values()));
          CHECK (Seq{"z"} == contents (untyped2.scope()));
          
          
          RecS something({"type=thing", "a=1", "b=2", "c", "d"});
          CHECK (!isnil(something));
          CHECK ("thing" == something.getType());
          CHECK (Seq({"type=thing", "a=1", "b=2", "c", "d"}) == contents(something));
          CHECK (Seq({"a", "b"}) == contents (something.keys()));
          CHECK (Seq({"1", "2"}) == contents (something.values()));
          CHECK (Seq({"c", "d"}) == contents (something.scope()));
        }
      
      
      void
      copy_and_move()
        {
          RecS a({"type=thing", "a=1", "b=2", "c", "d"});
          RecS b(a);
          CHECK (a.getType() == b.getType());
          CHECK (contents(a) == contents(b));
          CHECK (contents(a.attributes()) == contents(b.attributes()));
          
          CHECK (!isSameOject (a.get("a"), b.get("a")));
          CHECK (!isSameOject (*a.scope(), *b.scope()));
          
          string& c = *b.scope();
          CHECK ("c" == c);
          
          RecS bb;
          CHECK (isnil(bb));
          bb = move(b);
          CHECK ("b" == bb.get("b"));
          CHECK (isSameObject(c, *bb.scope()));
          
          swap (a, bb);
          CHECK (!isSameObject(c, *bb.scope()));
          CHECK ( isSameObject(c, *a.scope()));
          
          CHECK (isnil (b));
          b = bb;
          CHECK (!isnil (b));
          CHECK (!isSameObject(*b.get("a"), *bb.get("a")));
          CHECK (!isSameObject(*b.scope(), *bb.scope()));
        }
      
      
      void
      equality()
        {
          RecS a({"a"});
          RecS aa({"a","aa"});
          RecS aaa({"a","a"});
          RecS ax({"type=a","a"});
          RecS ay({"a=a","a"});
          
          CHECK (a != aa);   CHECK (aa != a);
          CHECK (aa != aaa); CHECK (aaa != aa);
          CHECK (a != aaa);  CHECK (aaa != a);
          CHECK (a != ax);   CHECK (ax != a);
          CHECK (a != ay);   CHECK (ay != a);
          CHECK (ax != ay);  CHECK (ay != ax);
          CHECK (aaa != ay); CHECK (ay != aaa);
          
          RecS a2({"a","aa"});
          CHECK (aa == a2);  CHECK (a2 == aa);
          
          RecS o1("oo", {"a=α", "b=β"}, {"γ", "δ", "ε"});
          RecS o2({"type=oo", "a = α", "b = β", "γ", "δ", "ε"});
          RecS o3({"type=oO", "a = α", "b = β", "γ", "δ", "ε"});
          RecS o4({"type=oo", "a = α", "b = β", "c=γ", "δ", "ε"});
          RecS o5({"type=oo", "a = α", "b = β", "γ", "ε", "δ"});
          RecS o6({"type=oo", "a = α", "b = β", "γ", "δ"});
          
          CHECK (o1 == o2);  CHECK (o2 == o1);
          CHECK (o2 != o3);  CHECK (o3 != o2);
          CHECK (o3 != o4);  CHECK (o4 != o3);
          CHECK (o4 != o5);  CHECK (o5 != o4);
          CHECK (o5 != o6);  CHECK (o6 != o5);
          CHECK (o1 != o3);  CHECK (o3 != o1);
          CHECK (o1 != o4);  CHECK (o4 != o1);
          CHECK (o1 != o5);  CHECK (o5 != o1);
          CHECK (o1 != o6);  CHECK (o6 != o1);
          CHECK (o2 != o4);  CHECK (o4 != o2);
          CHECK (o2 != o5);  CHECK (o5 != o2);
          CHECK (o2 != o6);  CHECK (o6 != o2);
          CHECK (o3 != o5);  CHECK (o5 != o3);
          CHECK (o3 != o6);  CHECK (o6 != o3);
          CHECK (o4 != o6);  CHECK (o6 != o4);
          
          RecS o7({"type=oo", "b = β", "a = α", "γ", "δ", "ε"});
          CHECK (o2 != o7);  CHECK (o7 != o2);
          // ideally, they would be equal, but this would require
          // a way more expensive implementation
        }
      
      
      void
      verifyMutations()
        {
          RecS a;
          CHECK (isnil (a));
          CHECK ("NIL" == a.getType());
          
          RecS::Mutator mut(a);
          mut.setType("u");
          mut.appendChild("a");
          mut.set("a", "1");
          
          RecS aa(mut);
          CHECK (a != aa);
          CHECK ("u" == aa.getType());
          CHECK (Seq({"type=u", "a=1", "a"}) == contents(aa));
          CHECK (Seq({"a"}) == contents (aa.keys()));
          CHECK (Seq({"1"}) == contents (aa.values()));
          CHECK (Seq({"a"}) == contents (aa.scope()));
          
          CHECK (mut == aa);
          
          mut.prependChild("⟂");
          mut.set("b", "β");
          mut.set("a", "α");
          
          CHECK (mut != aa);
          
          mut.replace(a);
          CHECK (isnil (mut));
          CHECK (Seq({"type=u", "a=α", "a=β", "⟂", "a"}) == contents(a));
          CHECK (Seq({"type=u", "a=1", "a"}) == contents(aa));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (GenericRecordRepresentation_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
