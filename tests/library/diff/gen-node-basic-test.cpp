/*
  GenNodeBasic(Test)  -  fundamental properties of a generic tree node element

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
#include "lib/diff/gen-node.hpp"
#include "lib/diff/record.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/util.hpp"

//#include <utility>
//#include <string>
//#include <vector>
#include <cmath>

using util::isnil;
using util::contains;
using util::isSameObject;
using lib::hash::LuidH;
using lib::time::FSecs;
using lib::time::Time;
using lib::time::TimeSpan;
//using std::string;
//using std::vector;
//using std::swap;
using std::fabs;


namespace lib {
namespace diff{
namespace test{
  
//  using lumiera::error::LUMIERA_ERROR_LOGIC;
  using error::LUMIERA_ERROR_WRONG_TYPE;
  using error::LUMIERA_ERROR_BOTTOM_VALUE;
  
  namespace {//Test fixture....
    
    
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /*****************************************************************************//**
   * @test Verify properties of a special collection type shaped for
   *       external representation of object-like data.
   *       - GenNode elements can be created "right away", picking up
   *         the given type, assumed that the payload is one of the
   *         supported basic types.
   *       - optionally, GenNode elements can be named
   *       - unnamed elements get a marker ID plus unique number extension
   *       - object-like elements can be represented by using a diff:Record<GenNode>
   *         as payload. Obviously, the resulting data structure type is recursive.
   *       - a shortcut is provided to simplify defining empty baseline objects
   *       - there is a special notation to create "id references", which can
   *         can be used to stand-in for an "object" (Record). This shortcut
   *         notation is relevant for the tree diff language -- used within
   *         Lumiera as "External Tree Description" of object networks.
   *       
   * @see IndexTable
   * @see DiffListApplication_test
   */
  class GenNodeBasic_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          objectShortcut();
          symbolReference();
          sequenceIteration();
          copy_and_move();
        }
      
      
      void
      simpleUsage()
        {
          // can build from the supported value types
          GenNode n1(42);
          CHECK (42 == n1.data.get<int>());
          CHECK (!n1.isNamed());
          CHECK (contains (n1.idi.getSym(), "_CHILD_"));
          CHECK (contains (name(n1), "_CHILD_"));
          
          // can optionally be named
          GenNode n2("π", 3.14159265358979323846264338328);
          CHECK (fabs (3.14159265 - n2.data.get<double>()) < 1e-5 );
          CHECK (n2.isNamed());
          CHECK ("π" == n2.idi.getSym());
          CHECK ("π" == name(n2));
          
          // is a copyable value
          GenNode n11(n1);
          CHECK (n1 == n11);
          CHECK (42 == n11.data.get<int>());
          
          // is assignable with compatible payload value
          n11.data = 24;
          CHECK (n1 != n11);
          CHECK (24 == n11.data.get<int>());
          CHECK (42 == n1.data.get<int>());
          
          // is assignable within the same kind of value
          n1 = n11;
          CHECK (n1 == n11);
          
          // but assignment may not alter payload type
          VERIFY_ERROR (WRONG_TYPE, n1 = n2 );
          
          // can build recursive data structures
          GenNode n3(Rec({GenNode("type", "spam"), GenNode("ham", "eggs")}));
          CHECK ("spam" == n3.data.get<Rec>().getType());
          CHECK ("eggs" == n3.data.get<Rec>().get("ham").data.get<string>());
          CHECK ("ham"  == n3.data.get<Rec>().get("ham").idi.getSym());
          CHECK (n3.data.get<Rec>().get("ham").isNamed());
          CHECK (!n3.isNamed());
        }
      
      
      void
      objectShortcut()
        {
          auto o0 = MakeRec().genNode();
          auto o1 = MakeRec().genNode("νόμος");
          auto o2 = MakeRec().type("spam").genNode();
          auto o3 = MakeRec().attrib("Ψ", int64_t(42), "π", 3.14159265358979323846264338328).genNode("λόγος");
          
          CHECK (!o0.isNamed());
          CHECK (isnil(o0.data.get<Rec>()));
          CHECK ("NIL" == o0.data.get<Rec>().getType());
          
          CHECK (o1.isNamed());
          CHECK ("νόμος" == o1.idi.getSym());
          CHECK (isnil(o1.data.get<Rec>()));
          
          CHECK (!o2.isNamed());
          CHECK ("spam" == o0.data.get<Rec>().getType());
          CHECK (isnil(o2.data.get<Rec>()));
          
          CHECK (o3.isNamed());
          CHECK ("λόγος" == o3.idi.getSym());
          CHECK ("NIL" == o3.data.get<Rec>().getType());
          CHECK (GenNode("Ψ", int64_t(42)) == o3.data.get<Rec>().get("Ψ"));
          CHECK (42L == o3.data.get<Rec>().get("Ψ").data.get<int64_t>());
          CHECK (1e-7 > fabs (3.14159265 - o3.data.get<Rec>().get("π").data.get<double>()));
          
          LuidH luid;
          //Demonstration: object builder is based on the mutator mechanism for Records...
          auto o4 = Rec::Mutator(o2.data.get<Rec>())                         // ...use GenNode o2 as starting point
                       .appendChild(GenNode("τ", Time(1,2,3,4)))             // a named node with Time value
                       .scope('*'                                            // a char node
                             ,"★"                                            // a string node
                             ,luid                                           // a hash value (LUID)
                             ,TimeSpan(Time::ZERO, FSecs(23,25))             // a time span
                             ,MakeRec().type("ham").scope("eggs").genNode()) // a spam object
                       .genNode("baked beans");                              // ---> finish into named node
          
          CHECK (o4.isNamed());
          CHECK ("baked beans" == o4.idi.getSym());
          CHECK ("spam" == o4.data.get<Rec>().getType());  // this was "inherited" from o2
          
          auto scope = o4.data.get<Rec>().scope();
          CHECK (!isnil(scope));
          CHECK (GenNode("τ", Time(1,2,3,4)) == *scope);
          ++scope;
          CHECK (GenNode(char('*')) == *scope);
          ++scope;
          CHECK ("★" == scope->data.get<string>());
          ++scope;
          CHECK (luid == scope->data.get<LuidH>());
          ++scope;
          CHECK (Time(0.92,0) == scope->data.get<TimeSpan>().end());
          ++scope;
          auto spam = *scope;
          CHECK (!++scope);
          CHECK ("ham" == spam.data.get<Rec>().getType());
          CHECK (spam.contains (GenNode("eggs")));
          
          // but while o4 was based on o2,
          // adding all the additional contents didn't mutate o2
          CHECK (isnil(o2.data.get<Rec>()));
        }
      
      
      
      void
      symbolReference()
        {
          GenNode ham = MakeRec().type("spam").attrib("τ", Time(23,42)).genNode("egg bacon sausage and spam");

          GenNode::ID hamID(ham);
          CHECK (hamID == ham.idi);
          CHECK (hamID.getSym() == ham.idi.getSym());
          CHECK (hamID.getHash() == ham.idi.getHash());
          CHECK (contains (string(hamID), "spam")); // Lovely spam!
          
          GenNode ref1 = Ref("egg bacon sausage and spam");
          GenNode ref2 = Ref(ham);
          
          CHECK (ref1.idi == ham.idi);
          CHECK (ref2.idi == ham.idi);
          
          // can stand-in for the original Record...
          CHECK (isSameObject (ham, ref2.data.get<Rec>()));
          VERIFY_ERROR (BOTTOM_VALUE, ref1.data.get<Rec>());
          
          RecordRef rr1 = ref1.data.get<RecordRef>();
          RecordRef rr2 = ref2.data.get<RecordRef>();
          
          CHECK ( isnil(rr1));
          CHECK (!isnil(rr2));
          Rec& ham_ref = rr2;
          CHECK (isSameObject(ham, ham_ref));
          CHECK (isSameObject(&ham, rr2.get()));
          
          // pre-defined special ref-tokens
          CHECK ("_END_" == name(Ref::END));
          CHECK ("_THIS_" == name(Ref::THIS));
          CHECK ("_CHILD_" == name(Ref::CHILD));
          CHECK ("_ATTRIBS_" == name(Ref::ATTRIBS));
          
          CHECK (isnil (Ref::END.data.get<RecordRef>()));
          CHECK (isnil (Ref::THIS.data.get<RecordRef>()));
          CHECK (isnil (Ref::CHILD.data.get<RecordRef>()));
          CHECK (isnil (Ref::ATTRIBS.data.get<RecordRef>()));
        }
      
      
      void
      sequenceIteration()
        {
          UNIMPLEMENTED ("wtf");
        }
      
      
      void
      copy_and_move()
        {
          UNIMPLEMENTED ("need to check that?");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (GenNodeBasic_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
