/*
  GenNode(Test)  -  fundamental properties of a generic tree node element

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

/** @file gen-node-test.cpp
 ** unit test \ref GenNode_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-cout.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/diff/record.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/util-quant.hpp"
#include "lib/util.hpp"

#include <string>

using util::isnil;
using util::contains;
using util::isSameObject;
using util::almostEqual;
using lib::hash::LuidH;
using lib::time::FSecs;
using lib::time::Time;
using lib::time::TimeSpan;
using lib::test::randTime;
using std::string;


namespace lib {
namespace diff{
namespace test{
  
  using LERR_(WRONG_TYPE);
  using LERR_(BOTTOM_VALUE);
  
  namespace {//Test fixture....
    
    const double PI = 3.14159265358979323846264338328;
    
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
   *       - a special notation is provided to create "id references", which
   *         can be used to stand-in for an "object" (Record). This shortcut
   *         notation is relevant for the tree diff language -- used within
   *         Lumiera as "External Tree Description" of object networks.
   *       
   * @see IndexTable
   * @see DiffListApplication_test
   */
  class GenNode_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          equalityMatch();
          objectShortcut();
          symbolReference();
          sequenceIteration();
          convenienceRecAccess();
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
          GenNode n2("π", PI);
          CHECK (almostEqual (PI, n2.data.get<double>()));
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
          
          cout << n3 <<endl; // diagnostic spam
        }
      
      
      void
      objectShortcut()
        {
          auto o0 = MakeRec().genNode();
          auto o1 = MakeRec().genNode("νόμος");
          auto o2 = MakeRec().type("spam").genNode();
          auto o3 = MakeRec().attrib("Ψ", int64_t(42), "π", 3.14159265358979323846264338328).genNode("μάθησις");
          
          CHECK (!o0.isNamed());
          CHECK (isnil(o0.data.get<Rec>()));
          CHECK ("NIL" == o0.data.get<Rec>().getType());
          
          CHECK (o1.isNamed());
          CHECK ("νόμος" == o1.idi.getSym());
          CHECK (isnil(o1.data.get<Rec>()));
          
          CHECK (!o2.isNamed());
          CHECK ("spam" == o2.data.get<Rec>().getType());
          CHECK (isnil(o2.data.get<Rec>()));
          
          CHECK (o3.isNamed());
          CHECK ("μάθησις" == o3.idi.getSym());
          CHECK ("NIL" == o3.data.get<Rec>().getType());
          CHECK (GenNode("Ψ", int64_t(42)) == o3.data.get<Rec>().get("Ψ"));
          CHECK (42L == o3.data.get<Rec>().get("Ψ").data.get<int64_t>());
          CHECK (almostEqual (PI, o3.data.get<Rec>().get("π").data.get<double>()));
          
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
          CHECK (char('*') == scope->data.get<char>());
          ++scope;
          CHECK ("★" == scope->data.get<string>());
          ++scope;
          CHECK (luid == scope->data.get<LuidH>());
          ++scope;
          CHECK (Time(920,0) == scope->data.get<TimeSpan>().end());
          ++scope;
          auto spam = *scope;
          CHECK (!++scope);
          CHECK ("ham" == spam.data.get<Rec>().getType());
          CHECK (spam.contains ("eggs"));
          
          // but while o4 was based on o2,
          // adding all the additional contents didn't mutate o2
          CHECK (isnil(o2.data.get<Rec>()));
          
          // special case: can create an (Attribute) GenNode with specifically crafted ID
          idi::EntryID<uint8_t> veryspecialID{"quasi niente"};
          auto o5 = MakeRec().genNode(veryspecialID);
          CHECK (o5 != MakeRec().genNode());
          CHECK (o5 != MakeRec().genNode("quasi niente"));
          CHECK (o5 == MakeRec().genNode(veryspecialID));
          CHECK (name(o5) == "quasi_niente");             // Note: EntryID sanitised the string
          CHECK (o5.idi == veryspecialID);
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
          
          Ref ref1("egg bacon sausage and spam"); // empty placeholder
          Ref ref2(ham);
          
          CHECK (ref1.idi == ham.idi);
          CHECK (ref2.idi == ham.idi);
          
          // can stand-in for the original Record...
          Rec& hamRef = ham.data.get<Rec>();
          CHECK (isSameObject (hamRef, ref2.data.get<Rec>()));
          VERIFY_ERROR (BOTTOM_VALUE, ref1.data.get<Rec>());
          
          RecRef rr1 = ref1.data.get<RecRef>();
          RecRef rr2 = ref2.data.get<RecRef>();
          
          CHECK ( isnil(rr1));
          CHECK (!isnil(rr2));
          Rec& ham_ref = rr2;
          CHECK (isSameObject(hamRef, ham_ref));
          CHECK (isSameObject(hamRef, *rr2.get()));
          
          // pre-defined special ref-tokens
          CHECK ("_END_" == name(Ref::END));
          CHECK ("_THIS_" == name(Ref::THIS));
          CHECK ("_CHILD_" == name(Ref::CHILD));
          CHECK ("_ATTRIBS_" == name(Ref::ATTRIBS));
          
          CHECK (isnil (Ref::END.data.get<RecRef>()));
          CHECK (isnil (Ref::THIS.data.get<RecRef>()));
          CHECK (isnil (Ref::CHILD.data.get<RecRef>()));
          CHECK (isnil (Ref::ATTRIBS.data.get<RecRef>()));
        }
      
      
      void
      sequenceIteration()
        {
          GenNode n = MakeRec()
                         .scope('*'                                      // a char node
                               ,"★"                                      // a string node
                               , PI                                      // a double value
                               ,MakeRec().type("ham")
                                         .scope("eggs","spam","spam")
                                         .genNode("spam")                // a spam object
                               ,TimeSpan(Time::ZERO, FSecs(23,25))       // a time span
                               ,int64_t(42))                             // long value
                         .attrib("hasSpam", true) // boolean Attribute
                         .genNode("baked beans"); // build Node from named Record
          
          
          cout << "--spam--"<<endl;
          for (auto & elm : n)
            cout << elm <<endl;
          
          
          auto iter = n.begin();
          CHECK (!isnil (iter));
          CHECK (1 == iter.level());
          CHECK ("baked beans" == iter->idi.getSym());                   // initially the Record itself is exposed
          CHECK (Rec::TYPE_NIL == iter->data.get<Rec>().getType());
          
              ++iter;
              CHECK (2 == iter.level());                                 // delve into the contents,
              CHECK ("hasSpam" == iter->idi.getSym());                   // ...starting with the attribute(s)
              CHECK (true      == iter->data.get<bool>());
              CHECK ("GenNode-ID(\"hasSpam\")-DataCap|«bool»|true" == string(*iter));
              
              ++iter;
              CHECK (!iter->isNamed());                                  // contents of the object's scope
              CHECK ('*' == iter->data.get<char>());
              
              ++iter;
              CHECK (!iter->isNamed());
              CHECK ("★" == iter->data.get<string>());
              
              ++iter;
              CHECK (!iter->isNamed());
              CHECK (almostEqual (PI, iter->data.get<double>()));
              
              ++iter;
              CHECK ("spam" == iter->idi.getSym());                      // the nested object is first exposed as a whole
              CHECK ("ham"  == iter->data.get<Rec>().getType());
              
                  ++iter;
                  CHECK (3 == iter.level());
                  CHECK ("eggs" == iter->data.get<string>());            // contents of the nested ("spam") object's scope
                  
                  ++iter;
                  CHECK ("spam" == iter->data.get<string>());
                  
                  ++iter;
                  CHECK ("spam" == iter->data.get<string>());
                  CHECK (3 == iter.level());
              
              ++iter;
              CHECK (2 == iter.level());                                 // decreasing level indicates we left nested scope
              CHECK (!iter->isNamed());                                  // next item in the enclosing scope
              CHECK ("0:00:00.000≺920ms≻" == string(iter->data.get<TimeSpan>()));
              ++iter;
              CHECK (!iter->isNamed());
              CHECK (42 == iter->data.get<int64_t>());
              CHECK (2 == iter.level());
          
          ++iter;                  // nothing more on top level beyond the initial Record
          CHECK (0 == iter.level());
          CHECK (isnil (iter));
          
          
          
          // another kind of iteration: shallow child data sequence
          // note: exposing the DataCap of each child
          auto child = childData(n);
          CHECK (!isnil (child));
          CHECK ('*' == child->get<char>());
          
          ++child;
          CHECK ("★" == child->get<string>());
          
          ++child;
          CHECK (almostEqual (PI, child->get<double>()));
          
          ++child;
          CHECK ("ham"  == child->get<Rec>().getType());
          CHECK ("eggs" == child->get<Rec>().child(0).data.get<string>());
          
          ++child;
          CHECK (TimeSpan(Time::ZERO, FSecs(23,25)) == child->get<TimeSpan>());
          
          ++child;
          CHECK (42 == child->get<int64_t>());
          
          ++child;
          CHECK (isnil (child));
          
          CHECK (n.hasChildren());
          CHECK (not GenNode{42}.hasChildren());
        }
      
      
      void
      equalityMatch()
        {
          int     i1 = 64;                        GenNode ni1(i1);
          int     i2 = 126;                       GenNode ni2(i2);
          int64_t l1 = 64;                        GenNode nl1(l1);
          int64_t l2 = 126;                       GenNode nl2(l2);
          short   r1 = 64;                        GenNode nr1(r1);
          short   r2 = 126;                       GenNode nr2(r2);
          double  d1 = 64;                        GenNode nd1(d1);
          double  d2 = 126;                       GenNode nd2(d2);
          char    c1 = '@';                       GenNode nc1(c1);
          char    c2 = '~';                       GenNode nc2(c2);
          bool    b1 = true;                      GenNode nb1(b1);
          bool    b2 = false;                     GenNode nb2(b2);
          string  s1 = "";                        GenNode ns1(s1);
          string  s2 = "↯";                       GenNode ns2(s2);
          
          time::Time t1 = randTime();             GenNode nt1(t1);
          time::Time t2(-t1);                     GenNode nt2(t2);
          time::Offset to1(t1);                   GenNode nto1(to1);
          time::Offset to2(t2);                   GenNode nto2(to2);
          time::Duration td1(to2);                GenNode ntd1(td1);
          time::Duration td2(to2*2);              GenNode ntd2(td2);
          time::TimeSpan ts1(t1, td1);            GenNode nts1(ts1);
          time::TimeSpan ts2(t2, td2);            GenNode nts2(ts2);
          hash::LuidH h1;                         GenNode nh1(h1);
          hash::LuidH h2;                         GenNode nh2(h2);
          
          Rec spam1({GenNode("ham", "eggs")});    GenNode rec1(spam1);
          Rec spam2(MakeRec(spam1).type("spam")); GenNode rec2(spam2);
          
          RecRef recRef1(spam1);                  Ref ref1(rec1);
          RecRef recRef2(spam2);                  Ref ref2(rec2);
                                                   // NOTE: same ID as referee
          CHECK (ni1 == ni1);
          CHECK (ni2 == ni2);
          CHECK (nl1 == nl1);
          CHECK (nl2 == nl2);
          CHECK (nr1 == nr1);
          CHECK (nr2 == nr2);
          CHECK (nd1 == nd1);
          CHECK (nd2 == nd2);
          CHECK (nc1 == nc1);
          CHECK (nc2 == nc2);
          CHECK (nb1 == nb1);
          CHECK (nb2 == nb2);
          CHECK (ns1 == ns1);
          CHECK (ns2 == ns2);
          CHECK (nt1  == nt1 );
          CHECK (nt2  == nt2 );
          CHECK (nto1 == nto1);
          CHECK (nto2 == nto2);
          CHECK (ntd1 == ntd1);
          CHECK (ntd2 == ntd2);
          CHECK (nts1 == nts1);
          CHECK (nts2 == nts2);
          CHECK (nh1  == nh1 );
          CHECK (nh2  == nh2 );
          CHECK (rec1 == rec1);
          CHECK (rec2 == rec2);
          CHECK (ref1 == ref1);
          CHECK (ref2 == ref2);
          
          CHECK (ni1 != ni2);      CHECK (ni2 != ni1);
          CHECK (nl1 != nl2);      CHECK (nl2 != nl1);
          CHECK (nr1 != nr2);      CHECK (nr2 != nr1);
          CHECK (nd1 != nd2);      CHECK (nd2 != nd1);
          CHECK (nc1 != nc2);      CHECK (nc2 != nc1);
          CHECK (nb1 != nb2);      CHECK (nb2 != nb1);
          CHECK (ns1 != ns2);      CHECK (ns2 != ns1);
          CHECK (nt1  != nt2 );    CHECK (nt2  != nt1 );
          CHECK (nto1 != nto2);    CHECK (nto2 != nto1);
          CHECK (ntd1 != ntd2);    CHECK (ntd2 != ntd1);
          CHECK (nts1 != nts2);    CHECK (nts2 != nts1);
          CHECK (nh1  != nh2 );    CHECK (nh2  != nh1 );
          CHECK (rec1 != rec2);    CHECK (rec2 != rec1);
          CHECK (ref1 != ref2);    CHECK (ref2 != ref1);
          
          CHECK (ni1 != ni2);      CHECK (ni2  != ni1);
          CHECK (ni1 != nl1);      CHECK (nl1  != ni1);
          CHECK (ni1 != nl2);      CHECK (nl2  != ni1);
          CHECK (ni1 != nr1);      CHECK (nr1  != ni1);
          CHECK (ni1 != nr2);      CHECK (nr2  != ni1);
          CHECK (ni1 != nd1);      CHECK (nd1  != ni1);
          CHECK (ni1 != nd2);      CHECK (nd2  != ni1);
          CHECK (ni1 != nc1);      CHECK (nc1  != ni1);
          CHECK (ni1 != nc2);      CHECK (nc2  != ni1);
          CHECK (ni1 != nb1);      CHECK (nb1  != ni1);
          CHECK (ni1 != nb2);      CHECK (nb2  != ni1);
          CHECK (ni1 != ns1);      CHECK (ns1  != ni1);
          CHECK (ni1 != ns2);      CHECK (ns2  != ni1);
          CHECK (ni1 != nt1 );     CHECK (nt1  != ni1);
          CHECK (ni1 != nt2 );     CHECK (nt2  != ni1);
          CHECK (ni1 != nto1);     CHECK (nto1 != ni1);
          CHECK (ni1 != nto2);     CHECK (nto2 != ni1);
          CHECK (ni1 != ntd1);     CHECK (ntd1 != ni1);
          CHECK (ni1 != ntd2);     CHECK (ntd2 != ni1);
          CHECK (ni1 != nts1);     CHECK (nts1 != ni1);
          CHECK (ni1 != nts2);     CHECK (nts2 != ni1);
          CHECK (ni1 != nh1 );     CHECK (nh1  != ni1);
          CHECK (ni1 != nh2 );     CHECK (nh2  != ni1);
          CHECK (ni1 != rec1);     CHECK (rec1 != ni1);
          CHECK (ni1 != rec2);     CHECK (rec2 != ni1);
          CHECK (ni1 != ref1);     CHECK (ref1 != ni1);
          CHECK (ni1 != ref2);     CHECK (ref2 != ni1);
          
          CHECK (ni2 != nl1);      CHECK (nl1  != ni2);
          CHECK (ni2 != nl2);      CHECK (nl2  != ni2);
          CHECK (ni2 != nr1);      CHECK (nr1  != ni2);
          CHECK (ni2 != nr2);      CHECK (nr2  != ni2);
          CHECK (ni2 != nd1);      CHECK (nd1  != ni2);
          CHECK (ni2 != nd2);      CHECK (nd2  != ni2);
          CHECK (ni2 != nc1);      CHECK (nc1  != ni2);
          CHECK (ni2 != nc2);      CHECK (nc2  != ni2);
          CHECK (ni2 != nb1);      CHECK (nb1  != ni2);
          CHECK (ni2 != nb2);      CHECK (nb2  != ni2);
          CHECK (ni2 != ns1);      CHECK (ns1  != ni2);
          CHECK (ni2 != ns2);      CHECK (ns2  != ni2);
          CHECK (ni2 != nt1 );     CHECK (nt1  != ni2);
          CHECK (ni2 != nt2 );     CHECK (nt2  != ni2);
          CHECK (ni2 != nto1);     CHECK (nto1 != ni2);
          CHECK (ni2 != nto2);     CHECK (nto2 != ni2);
          CHECK (ni2 != ntd1);     CHECK (ntd1 != ni2);
          CHECK (ni2 != ntd2);     CHECK (ntd2 != ni2);
          CHECK (ni2 != nts1);     CHECK (nts1 != ni2);
          CHECK (ni2 != nts2);     CHECK (nts2 != ni2);
          CHECK (ni2 != nh1 );     CHECK (nh1  != ni2);
          CHECK (ni2 != nh2 );     CHECK (nh2  != ni2);
          CHECK (ni2 != rec1);     CHECK (rec1 != ni2);
          CHECK (ni2 != rec2);     CHECK (rec2 != ni2);
          CHECK (ni2 != ref1);     CHECK (ref1 != ni2);
          CHECK (ni2 != ref2);     CHECK (ref2 != ni2);
          
          CHECK (nl1 != nl2);      CHECK (nl2  != nl1);
          CHECK (nl1 != nr1);      CHECK (nr1  != nl1);
          CHECK (nl1 != nr2);      CHECK (nr2  != nl1);
          CHECK (nl1 != nd1);      CHECK (nd1  != nl1);
          CHECK (nl1 != nd2);      CHECK (nd2  != nl1);
          CHECK (nl1 != nc1);      CHECK (nc1  != nl1);
          CHECK (nl1 != nc2);      CHECK (nc2  != nl1);
          CHECK (nl1 != nb1);      CHECK (nb1  != nl1);
          CHECK (nl1 != nb2);      CHECK (nb2  != nl1);
          CHECK (nl1 != ns1);      CHECK (ns1  != nl1);
          CHECK (nl1 != ns2);      CHECK (ns2  != nl1);
          CHECK (nl1 != nt1 );     CHECK (nt1  != nl1);
          CHECK (nl1 != nt2 );     CHECK (nt2  != nl1);
          CHECK (nl1 != nto1);     CHECK (nto1 != nl1);
          CHECK (nl1 != nto2);     CHECK (nto2 != nl1);
          CHECK (nl1 != ntd1);     CHECK (ntd1 != nl1);
          CHECK (nl1 != ntd2);     CHECK (ntd2 != nl1);
          CHECK (nl1 != nts1);     CHECK (nts1 != nl1);
          CHECK (nl1 != nts2);     CHECK (nts2 != nl1);
          CHECK (nl1 != nh1 );     CHECK (nh1  != nl1);
          CHECK (nl1 != nh2 );     CHECK (nh2  != nl1);
          CHECK (nl1 != rec1);     CHECK (rec1 != nl1);
          CHECK (nl1 != rec2);     CHECK (rec2 != nl1);
          CHECK (nl1 != ref1);     CHECK (ref1 != nl1);
          CHECK (nl1 != ref2);     CHECK (ref2 != nl1);
          
          CHECK (nl2 != nr1);      CHECK (nr1  != nl2);
          CHECK (nl2 != nr2);      CHECK (nr2  != nl2);
          CHECK (nl2 != nd1);      CHECK (nd1  != nl2);
          CHECK (nl2 != nd2);      CHECK (nd2  != nl2);
          CHECK (nl2 != nc1);      CHECK (nc1  != nl2);
          CHECK (nl2 != nc2);      CHECK (nc2  != nl2);
          CHECK (nl2 != nb1);      CHECK (nb1  != nl2);
          CHECK (nl2 != nb2);      CHECK (nb2  != nl2);
          CHECK (nl2 != ns1);      CHECK (ns1  != nl2);
          CHECK (nl2 != ns2);      CHECK (ns2  != nl2);
          CHECK (nl2 != nt1 );     CHECK (nt1  != nl2);
          CHECK (nl2 != nt2 );     CHECK (nt2  != nl2);
          CHECK (nl2 != nto1);     CHECK (nto1 != nl2);
          CHECK (nl2 != nto2);     CHECK (nto2 != nl2);
          CHECK (nl2 != ntd1);     CHECK (ntd1 != nl2);
          CHECK (nl2 != ntd2);     CHECK (ntd2 != nl2);
          CHECK (nl2 != nts1);     CHECK (nts1 != nl2);
          CHECK (nl2 != nts2);     CHECK (nts2 != nl2);
          CHECK (nl2 != nh1 );     CHECK (nh1  != nl2);
          CHECK (nl2 != nh2 );     CHECK (nh2  != nl2);
          CHECK (nl2 != rec1);     CHECK (rec1 != nl2);
          CHECK (nl2 != rec2);     CHECK (rec2 != nl2);
          CHECK (nl2 != ref1);     CHECK (ref1 != nl2);
          CHECK (nl2 != ref2);     CHECK (ref2 != nl2);
          
          CHECK (nr1 != nr2);      CHECK (nr2  != nr1);
          CHECK (nr1 != nd1);      CHECK (nd1  != nr1);
          CHECK (nr1 != nd2);      CHECK (nd2  != nr1);
          CHECK (nr1 != nc1);      CHECK (nc1  != nr1);
          CHECK (nr1 != nc2);      CHECK (nc2  != nr1);
          CHECK (nr1 != nb1);      CHECK (nb1  != nr1);
          CHECK (nr1 != nb2);      CHECK (nb2  != nr1);
          CHECK (nr1 != ns1);      CHECK (ns1  != nr1);
          CHECK (nr1 != ns2);      CHECK (ns2  != nr1);
          CHECK (nr1 != nt1 );     CHECK (nt1  != nr1);
          CHECK (nr1 != nt2 );     CHECK (nt2  != nr1);
          CHECK (nr1 != nto1);     CHECK (nto1 != nr1);
          CHECK (nr1 != nto2);     CHECK (nto2 != nr1);
          CHECK (nr1 != ntd1);     CHECK (ntd1 != nr1);
          CHECK (nr1 != ntd2);     CHECK (ntd2 != nr1);
          CHECK (nr1 != nts1);     CHECK (nts1 != nr1);
          CHECK (nr1 != nts2);     CHECK (nts2 != nr1);
          CHECK (nr1 != nh1 );     CHECK (nh1  != nr1);
          CHECK (nr1 != nh2 );     CHECK (nh2  != nr1);
          CHECK (nr1 != rec1);     CHECK (rec1 != nr1);
          CHECK (nr1 != rec2);     CHECK (rec2 != nr1);
          CHECK (nr1 != ref1);     CHECK (ref1 != nr1);
          CHECK (nr1 != ref2);     CHECK (ref2 != nr1);
          
          CHECK (nr2 != nd1);      CHECK (nd1  != nr2);
          CHECK (nr2 != nd2);      CHECK (nd2  != nr2);
          CHECK (nr2 != nc1);      CHECK (nc1  != nr2);
          CHECK (nr2 != nc2);      CHECK (nc2  != nr2);
          CHECK (nr2 != nb1);      CHECK (nb1  != nr2);
          CHECK (nr2 != nb2);      CHECK (nb2  != nr2);
          CHECK (nr2 != ns1);      CHECK (ns1  != nr2);
          CHECK (nr2 != ns2);      CHECK (ns2  != nr2);
          CHECK (nr2 != nt1 );     CHECK (nt1  != nr2);
          CHECK (nr2 != nt2 );     CHECK (nt2  != nr2);
          CHECK (nr2 != nto1);     CHECK (nto1 != nr2);
          CHECK (nr2 != nto2);     CHECK (nto2 != nr2);
          CHECK (nr2 != ntd1);     CHECK (ntd1 != nr2);
          CHECK (nr2 != ntd2);     CHECK (ntd2 != nr2);
          CHECK (nr2 != nts1);     CHECK (nts1 != nr2);
          CHECK (nr2 != nts2);     CHECK (nts2 != nr2);
          CHECK (nr2 != nh1 );     CHECK (nh1  != nr2);
          CHECK (nr2 != nh2 );     CHECK (nh2  != nr2);
          CHECK (nr2 != rec1);     CHECK (rec1 != nr2);
          CHECK (nr2 != rec2);     CHECK (rec2 != nr2);
          CHECK (nr2 != ref1);     CHECK (ref1 != nr2);
          CHECK (nr2 != ref2);     CHECK (ref2 != nr2);
          
          CHECK (nd1 != nd2);      CHECK (nd2  != nd1);
          CHECK (nd1 != nc1);      CHECK (nc1  != nd1);
          CHECK (nd1 != nc2);      CHECK (nc2  != nd1);
          CHECK (nd1 != nb1);      CHECK (nb1  != nd1);
          CHECK (nd1 != nb2);      CHECK (nb2  != nd1);
          CHECK (nd1 != ns1);      CHECK (ns1  != nd1);
          CHECK (nd1 != ns2);      CHECK (ns2  != nd1);
          CHECK (nd1 != nt1 );     CHECK (nt1  != nd1);
          CHECK (nd1 != nt2 );     CHECK (nt2  != nd1);
          CHECK (nd1 != nto1);     CHECK (nto1 != nd1);
          CHECK (nd1 != nto2);     CHECK (nto2 != nd1);
          CHECK (nd1 != ntd1);     CHECK (ntd1 != nd1);
          CHECK (nd1 != ntd2);     CHECK (ntd2 != nd1);
          CHECK (nd1 != nts1);     CHECK (nts1 != nd1);
          CHECK (nd1 != nts2);     CHECK (nts2 != nd1);
          CHECK (nd1 != nh1 );     CHECK (nh1  != nd1);
          CHECK (nd1 != nh2 );     CHECK (nh2  != nd1);
          CHECK (nd1 != rec1);     CHECK (rec1 != nd1);
          CHECK (nd1 != rec2);     CHECK (rec2 != nd1);
          CHECK (nd1 != ref1);     CHECK (ref1 != nd1);
          CHECK (nd1 != ref2);     CHECK (ref2 != nd1);
          
          CHECK (nd2 != nc1);      CHECK (nc1  != nd2);
          CHECK (nd2 != nc2);      CHECK (nc2  != nd2);
          CHECK (nd2 != nb1);      CHECK (nb1  != nd2);
          CHECK (nd2 != nb2);      CHECK (nb2  != nd2);
          CHECK (nd2 != ns1);      CHECK (ns1  != nd2);
          CHECK (nd2 != ns2);      CHECK (ns2  != nd2);
          CHECK (nd2 != nt1 );     CHECK (nt1  != nd2);
          CHECK (nd2 != nt2 );     CHECK (nt2  != nd2);
          CHECK (nd2 != nto1);     CHECK (nto1 != nd2);
          CHECK (nd2 != nto2);     CHECK (nto2 != nd2);
          CHECK (nd2 != ntd1);     CHECK (ntd1 != nd2);
          CHECK (nd2 != ntd2);     CHECK (ntd2 != nd2);
          CHECK (nd2 != nts1);     CHECK (nts1 != nd2);
          CHECK (nd2 != nts2);     CHECK (nts2 != nd2);
          CHECK (nd2 != nh1 );     CHECK (nh1  != nd2);
          CHECK (nd2 != nh2 );     CHECK (nh2  != nd2);
          CHECK (nd2 != rec1);     CHECK (rec1 != nd2);
          CHECK (nd2 != rec2);     CHECK (rec2 != nd2);
          CHECK (nd2 != ref1);     CHECK (ref1 != nd2);
          CHECK (nd2 != ref2);     CHECK (ref2 != nd2);
          
          CHECK (nc1 != nc2);      CHECK (nc2  != nc1);
          CHECK (nc1 != nb1);      CHECK (nb1  != nc1);
          CHECK (nc1 != nb2);      CHECK (nb2  != nc1);
          CHECK (nc1 != ns1);      CHECK (ns1  != nc1);
          CHECK (nc1 != ns2);      CHECK (ns2  != nc1);
          CHECK (nc1 != nt1 );     CHECK (nt1  != nc1);
          CHECK (nc1 != nt2 );     CHECK (nt2  != nc1);
          CHECK (nc1 != nto1);     CHECK (nto1 != nc1);
          CHECK (nc1 != nto2);     CHECK (nto2 != nc1);
          CHECK (nc1 != ntd1);     CHECK (ntd1 != nc1);
          CHECK (nc1 != ntd2);     CHECK (ntd2 != nc1);
          CHECK (nc1 != nts1);     CHECK (nts1 != nc1);
          CHECK (nc1 != nts2);     CHECK (nts2 != nc1);
          CHECK (nc1 != nh1 );     CHECK (nh1  != nc1);
          CHECK (nc1 != nh2 );     CHECK (nh2  != nc1);
          CHECK (nc1 != rec1);     CHECK (rec1 != nc1);
          CHECK (nc1 != rec2);     CHECK (rec2 != nc1);
          CHECK (nc1 != ref1);     CHECK (ref1 != nc1);
          CHECK (nc1 != ref2);     CHECK (ref2 != nc1);
          
          CHECK (nc2 != nb1);      CHECK (nb1  != nc2);
          CHECK (nc2 != nb2);      CHECK (nb2  != nc2);
          CHECK (nc2 != ns1);      CHECK (ns1  != nc2);
          CHECK (nc2 != ns2);      CHECK (ns2  != nc2);
          CHECK (nc2 != nt1 );     CHECK (nt1  != nc2);
          CHECK (nc2 != nt2 );     CHECK (nt2  != nc2);
          CHECK (nc2 != nto1);     CHECK (nto1 != nc2);
          CHECK (nc2 != nto2);     CHECK (nto2 != nc2);
          CHECK (nc2 != ntd1);     CHECK (ntd1 != nc2);
          CHECK (nc2 != ntd2);     CHECK (ntd2 != nc2);
          CHECK (nc2 != nts1);     CHECK (nts1 != nc2);
          CHECK (nc2 != nts2);     CHECK (nts2 != nc2);
          CHECK (nc2 != nh1 );     CHECK (nh1  != nc2);
          CHECK (nc2 != nh2 );     CHECK (nh2  != nc2);
          CHECK (nc2 != rec1);     CHECK (rec1 != nc2);
          CHECK (nc2 != rec2);     CHECK (rec2 != nc2);
          CHECK (nc2 != ref1);     CHECK (ref1 != nc2);
          CHECK (nc2 != ref2);     CHECK (ref2 != nc2);
          
          CHECK (nb1 != nb2);      CHECK (nb2  != nb1);
          CHECK (nb1 != ns1);      CHECK (ns1  != nb1);
          CHECK (nb1 != ns2);      CHECK (ns2  != nb1);
          CHECK (nb1 != nt1 );     CHECK (nt1  != nb1);
          CHECK (nb1 != nt2 );     CHECK (nt2  != nb1);
          CHECK (nb1 != nto1);     CHECK (nto1 != nb1);
          CHECK (nb1 != nto2);     CHECK (nto2 != nb1);
          CHECK (nb1 != ntd1);     CHECK (ntd1 != nb1);
          CHECK (nb1 != ntd2);     CHECK (ntd2 != nb1);
          CHECK (nb1 != nts1);     CHECK (nts1 != nb1);
          CHECK (nb1 != nts2);     CHECK (nts2 != nb1);
          CHECK (nb1 != nh1 );     CHECK (nh1  != nb1);
          CHECK (nb1 != nh2 );     CHECK (nh2  != nb1);
          CHECK (nb1 != rec1);     CHECK (rec1 != nb1);
          CHECK (nb1 != rec2);     CHECK (rec2 != nb1);
          CHECK (nb1 != ref1);     CHECK (ref1 != nb1);
          CHECK (nb1 != ref2);     CHECK (ref2 != nb1);
          
          CHECK (nb2 != ns1);      CHECK (ns1  != nb2);
          CHECK (nb2 != ns2);      CHECK (ns2  != nb2);
          CHECK (nb2 != nt1 );     CHECK (nt1  != nb2);
          CHECK (nb2 != nt2 );     CHECK (nt2  != nb2);
          CHECK (nb2 != nto1);     CHECK (nto1 != nb2);
          CHECK (nb2 != nto2);     CHECK (nto2 != nb2);
          CHECK (nb2 != ntd1);     CHECK (ntd1 != nb2);
          CHECK (nb2 != ntd2);     CHECK (ntd2 != nb2);
          CHECK (nb2 != nts1);     CHECK (nts1 != nb2);
          CHECK (nb2 != nts2);     CHECK (nts2 != nb2);
          CHECK (nb2 != nh1 );     CHECK (nh1  != nb2);
          CHECK (nb2 != nh2 );     CHECK (nh2  != nb2);
          CHECK (nb2 != rec1);     CHECK (rec1 != nb2);
          CHECK (nb2 != rec2);     CHECK (rec2 != nb2);
          CHECK (nb2 != ref1);     CHECK (ref1 != nb2);
          CHECK (nb2 != ref2);     CHECK (ref2 != nb2);
          
          CHECK (ns1 != nt2 );     CHECK (nt2  != ns1);
          CHECK (ns1 != nto1);     CHECK (nto1 != ns1);
          CHECK (ns1 != nto2);     CHECK (nto2 != ns1);
          CHECK (ns1 != ntd1);     CHECK (ntd1 != ns1);
          CHECK (ns1 != ntd2);     CHECK (ntd2 != ns1);
          CHECK (ns1 != nts1);     CHECK (nts1 != ns1);
          CHECK (ns1 != nts2);     CHECK (nts2 != ns1);
          CHECK (ns1 != nh1 );     CHECK (nh1  != ns1);
          CHECK (ns1 != nh2 );     CHECK (nh2  != ns1);
          CHECK (ns1 != rec1);     CHECK (rec1 != ns1);
          CHECK (ns1 != rec2);     CHECK (rec2 != ns1);
          CHECK (ns1 != ref1);     CHECK (ref1 != ns1);
          CHECK (ns1 != ref2);     CHECK (ref2 != ns1);
          
          CHECK (ns2 != nt1 );     CHECK (nt1  != ns2);
          CHECK (ns2 != nt2 );     CHECK (nt2  != ns2);
          CHECK (ns2 != nto1);     CHECK (nto1 != ns2);
          CHECK (ns2 != nto2);     CHECK (nto2 != ns2);
          CHECK (ns2 != ntd1);     CHECK (ntd1 != ns2);
          CHECK (ns2 != ntd2);     CHECK (ntd2 != ns2);
          CHECK (ns2 != nts1);     CHECK (nts1 != ns2);
          CHECK (ns2 != nts2);     CHECK (nts2 != ns2);
          CHECK (ns2 != nh1 );     CHECK (nh1  != ns2);
          CHECK (ns2 != nh2 );     CHECK (nh2  != ns2);
          CHECK (ns2 != rec1);     CHECK (rec1 != ns2);
          CHECK (ns2 != rec2);     CHECK (rec2 != ns2);
          CHECK (ns2 != ref1);     CHECK (ref1 != ns2);
          CHECK (ns2 != ref2);     CHECK (ref2 != ns2);
          
          CHECK (nt1 != nt2 );     CHECK (nt2  != nt1);
          CHECK (nt1 != nto1);     CHECK (nto1 != nt1);
          CHECK (nt1 != nto2);     CHECK (nto2 != nt1);
          CHECK (nt1 != ntd1);     CHECK (ntd1 != nt1);
          CHECK (nt1 != ntd2);     CHECK (ntd2 != nt1);
          CHECK (nt1 != nts1);     CHECK (nts1 != nt1);
          CHECK (nt1 != nts2);     CHECK (nts2 != nt1);
          CHECK (nt1 != nh1 );     CHECK (nh1  != nt1);
          CHECK (nt1 != nh2 );     CHECK (nh2  != nt1);
          CHECK (nt1 != rec1);     CHECK (rec1 != nt1);
          CHECK (nt1 != rec2);     CHECK (rec2 != nt1);
          CHECK (nt1 != ref1);     CHECK (ref1 != nt1);
          CHECK (nt1 != ref2);     CHECK (ref2 != nt1);
          
          CHECK (nt2 != nto1);     CHECK (nto1 != nt2);
          CHECK (nt2 != nto2);     CHECK (nto2 != nt2);
          CHECK (nt2 != ntd1);     CHECK (ntd1 != nt2);
          CHECK (nt2 != ntd2);     CHECK (ntd2 != nt2);
          CHECK (nt2 != nts1);     CHECK (nts1 != nt2);
          CHECK (nt2 != nts2);     CHECK (nts2 != nt2);
          CHECK (nt2 != nh1 );     CHECK (nh1  != nt2);
          CHECK (nt2 != nh2 );     CHECK (nh2  != nt2);
          CHECK (nt2 != rec1);     CHECK (rec1 != nt2);
          CHECK (nt2 != rec2);     CHECK (rec2 != nt2);
          CHECK (nt2 != ref1);     CHECK (ref1 != nt2);
          CHECK (nt2 != ref2);     CHECK (ref2 != nt2);
          
          CHECK (nto1 != nto2);    CHECK (nto2 != nto1);
          CHECK (nto1 != ntd1);    CHECK (ntd1 != nto1);
          CHECK (nto1 != ntd2);    CHECK (ntd2 != nto1);
          CHECK (nto1 != nts1);    CHECK (nts1 != nto1);
          CHECK (nto1 != nts2);    CHECK (nts2 != nto1);
          CHECK (nto1 != nh1 );    CHECK (nh1  != nto1);
          CHECK (nto1 != nh2 );    CHECK (nh2  != nto1);
          CHECK (nto1 != rec1);    CHECK (rec1 != nto1);
          CHECK (nto1 != rec2);    CHECK (rec2 != nto1);
          CHECK (nto1 != ref1);    CHECK (ref1 != nto1);
          CHECK (nto1 != ref2);    CHECK (ref2 != nto1);
          
          CHECK (nto2 != ntd1);    CHECK (ntd1 != nto2);
          CHECK (nto2 != ntd2);    CHECK (ntd2 != nto2);
          CHECK (nto2 != nts1);    CHECK (nts1 != nto2);
          CHECK (nto2 != nts2);    CHECK (nts2 != nto2);
          CHECK (nto2 != nh1 );    CHECK (nh1  != nto2);
          CHECK (nto2 != nh2 );    CHECK (nh2  != nto2);
          CHECK (nto2 != rec1);    CHECK (rec1 != nto2);
          CHECK (nto2 != rec2);    CHECK (rec2 != nto2);
          CHECK (nto2 != ref1);    CHECK (ref1 != nto2);
          CHECK (nto2 != ref2);    CHECK (ref2 != nto2);
          
          CHECK (ntd1 != ntd2);    CHECK (ntd2 != ntd1);
          CHECK (ntd1 != nts1);    CHECK (nts1 != ntd1);
          CHECK (ntd1 != nts2);    CHECK (nts2 != ntd1);
          CHECK (ntd1 != nh1 );    CHECK (nh1  != ntd1);
          CHECK (ntd1 != nh2 );    CHECK (nh2  != ntd1);
          CHECK (ntd1 != rec1);    CHECK (rec1 != ntd1);
          CHECK (ntd1 != rec2);    CHECK (rec2 != ntd1);
          CHECK (ntd1 != ref1);    CHECK (ref1 != ntd1);
          CHECK (ntd1 != ref2);    CHECK (ref2 != ntd1);
          
          CHECK (ntd2 != nts1);    CHECK (nts1 != ntd2);
          CHECK (ntd2 != nts2);    CHECK (nts2 != ntd2);
          CHECK (ntd2 != nh1 );    CHECK (nh1  != ntd2);
          CHECK (ntd2 != nh2 );    CHECK (nh2  != ntd2);
          CHECK (ntd2 != rec1);    CHECK (rec1 != ntd2);
          CHECK (ntd2 != rec2);    CHECK (rec2 != ntd2);
          CHECK (ntd2 != ref1);    CHECK (ref1 != ntd2);
          CHECK (ntd2 != ref2);    CHECK (ref2 != ntd2);
          
          CHECK (nts1 != nts2);    CHECK (nts2 != nts1);
          CHECK (nts1 != nh1 );    CHECK (nh1  != nts1);
          CHECK (nts1 != nh2 );    CHECK (nh2  != nts1);
          CHECK (nts1 != rec1);    CHECK (rec1 != nts1);
          CHECK (nts1 != rec2);    CHECK (rec2 != nts1);
          CHECK (nts1 != ref1);    CHECK (ref1 != nts1);
          CHECK (nts1 != ref2);    CHECK (ref2 != nts1);
          
          CHECK (nts2 != nh1 );    CHECK (nh1  != nts2);
          CHECK (nts2 != nh2 );    CHECK (nh2  != nts2);
          CHECK (nts2 != rec1);    CHECK (rec1 != nts2);
          CHECK (nts2 != rec2);    CHECK (rec2 != nts2);
          CHECK (nts2 != ref1);    CHECK (ref1 != nts2);
          CHECK (nts2 != ref2);    CHECK (ref2 != nts2);
          
          CHECK (nh1 != nh2 );     CHECK (nh2  != nh1);
          CHECK (nh1 != rec1);     CHECK (rec1 != nh1);
          CHECK (nh1 != rec2);     CHECK (rec2 != nh1);
          CHECK (nh1 != ref1);     CHECK (ref1 != nh1);
          CHECK (nh1 != ref2);     CHECK (ref2 != nh1);
                                                       
          CHECK (nh2 != rec1);     CHECK (rec1 != nh2);
          CHECK (nh2 != rec2);     CHECK (rec2 != nh2);
          CHECK (nh2 != ref1);     CHECK (ref1 != nh2);
          CHECK (nh2 != ref2);     CHECK (ref2 != nh2);
          
          CHECK (rec1 != rec2);    CHECK (rec2 != rec1);
          CHECK (ref1 != ref2);    CHECK (ref2 != ref1);
          
          // NOTE: special handling for record references…
          CHECK (rec1 == ref1);    CHECK (ref1 == rec1);
          CHECK (rec1 != ref2);    CHECK (ref2 != rec1);
          
          CHECK (rec2 != ref1);    CHECK (ref1 != rec2);
          CHECK (rec2 == ref2);    CHECK (ref2 == rec2);
        
          
          
          /* ----- equivalence match ----- */
          
          // equivalence as object       // equivalence on ID match         // contained value equality
          CHECK (ni1 .matches(ni1 ));    CHECK (ni1 .matches(ni1 .idi));    CHECK (ni1 .matches(i1 ));
          CHECK (ni2 .matches(ni2 ));    CHECK (ni2 .matches(ni2 .idi));    CHECK (ni2 .matches(i2 ));
          CHECK (nl1 .matches(nl1 ));    CHECK (nl1 .matches(nl1 .idi));    CHECK (nl1 .matches(l1 ));
          CHECK (nl2 .matches(nl2 ));    CHECK (nl2 .matches(nl2 .idi));    CHECK (nl2 .matches(l2 ));
          CHECK (nr1 .matches(nr1 ));    CHECK (nr1 .matches(nr1 .idi));    CHECK (nr1 .matches(r1 ));
          CHECK (nr2 .matches(nr2 ));    CHECK (nr2 .matches(nr2 .idi));    CHECK (nr2 .matches(r2 ));
          CHECK (nd1 .matches(nd1 ));    CHECK (nd1 .matches(nd1 .idi));    CHECK (nd1 .matches(d1 ));
          CHECK (nd2 .matches(nd2 ));    CHECK (nd2 .matches(nd2 .idi));    CHECK (nd2 .matches(d2 ));
          CHECK (nc1 .matches(nc1 ));    CHECK (nc1 .matches(nc1 .idi));    CHECK (nc1 .matches(c1 ));
          CHECK (nc2 .matches(nc2 ));    CHECK (nc2 .matches(nc2 .idi));    CHECK (nc2 .matches(c2 ));
          CHECK (nb1 .matches(nb1 ));    CHECK (nb1 .matches(nb1 .idi));    CHECK (nb1 .matches(b1 ));
          CHECK (nb2 .matches(nb2 ));    CHECK (nb2 .matches(nb2 .idi));    CHECK (nb2 .matches(b2 ));
          CHECK (ns1 .matches(ns1 ));    CHECK (ns1 .matches(ns1 .idi));    CHECK (ns1 .matches(s1 ));
          CHECK (ns2 .matches(ns2 ));    CHECK (ns2 .matches(ns2 .idi));    CHECK (ns2 .matches(s2 ));
          CHECK (nt1 .matches(nt1 ));    CHECK (nt1 .matches(nt1 .idi));    CHECK (nt1 .matches(t1 ));
          CHECK (nt2 .matches(nt2 ));    CHECK (nt2 .matches(nt2 .idi));    CHECK (nt2 .matches(t2 ));
          CHECK (nto1.matches(nto1));    CHECK (nto1.matches(nto1.idi));    CHECK (nto1.matches(to1));
          CHECK (nto2.matches(nto2));    CHECK (nto2.matches(nto2.idi));    CHECK (nto2.matches(to2));
          CHECK (ntd1.matches(ntd1));    CHECK (ntd1.matches(ntd1.idi));    CHECK (ntd1.matches(td1));
          CHECK (ntd2.matches(ntd2));    CHECK (ntd2.matches(ntd2.idi));    CHECK (ntd2.matches(td2));
          CHECK (nts1.matches(nts1));    CHECK (nts1.matches(nts1.idi));    CHECK (nts1.matches(ts1));
          CHECK (nts2.matches(nts2));    CHECK (nts2.matches(nts2.idi));    CHECK (nts2.matches(ts2));
          CHECK (nh1 .matches(nh1 ));    CHECK (nh1 .matches(nh1 .idi));    CHECK (nh1 .matches(h1 ));
          CHECK (nh2 .matches(nh2 ));    CHECK (nh2 .matches(nh2 .idi));    CHECK (nh2 .matches(h2 ));
          CHECK (rec1.matches(rec1));    CHECK (rec1.matches(rec1.idi));    CHECK (rec1.matches(spam1));
          CHECK (rec2.matches(rec2));    CHECK (rec2.matches(rec2.idi));    CHECK (rec2.matches(spam2));
          CHECK (ref1.matches(ref1));    CHECK (ref1.matches(ref1.idi));    CHECK (ref1.matches(recRef1));
          CHECK (ref2.matches(ref2));    CHECK (ref2.matches(ref2.idi));    CHECK (ref2.matches(recRef2));
          
          // cross-match on equivalent payload data --------
                                     CHECK (nl1.matches(i1));   CHECK (nr1.matches(i1));  CHECK (nd1.matches(i1));  CHECK (nc1.matches(i1));
          CHECK (ni1.matches(l1));                              CHECK (nr1.matches(l1));  CHECK (nd1.matches(l1));  CHECK (nc1.matches(l1));
          CHECK (ni1.matches(r1));   CHECK (nl1.matches(r1));                             CHECK (nd1.matches(r1));  CHECK (nc1.matches(r1));
          CHECK (ni1.matches(d1));   CHECK (nl1.matches(d1));   CHECK (nr1.matches(d1));                            CHECK (nc1.matches(d1));
          CHECK (ni1.matches(c1));   CHECK (nl1.matches(c1));   CHECK (nr1.matches(c1));  CHECK (nd1.matches(c1));
          
                                     CHECK (nl2.matches(i2));   CHECK (nr2.matches(i2));  CHECK (nd2.matches(i2));  CHECK (nc2.matches(i2));
          CHECK (ni2.matches(l2));                              CHECK (nr2.matches(l2));  CHECK (nd2.matches(l2));  CHECK (nc2.matches(l2));
          CHECK (ni2.matches(r2));   CHECK (nl2.matches(r2));                             CHECK (nd2.matches(r2));  CHECK (nc2.matches(r2));
          CHECK (ni2.matches(d2));   CHECK (nl2.matches(d2));   CHECK (nr2.matches(d2));                            CHECK (nc2.matches(d2));
          CHECK (ni2.matches(c2));   CHECK (nl2.matches(c2));   CHECK (nr2.matches(c2));  CHECK (nd2.matches(c2));
          
                                     CHECK (nto1.matches(t1 )); CHECK (nts1.matches(t1 ));
          CHECK (nt1.matches(to1));                             CHECK (nts1.matches(to1));
          CHECK (nt1.matches(ts1));  CHECK (nto1.matches(ts1));
          
                                     CHECK (nto2.matches(t2 )); CHECK (nts2.matches(t2 ));
          CHECK (nt2.matches(to2));                             CHECK (nts2.matches(to2));
          CHECK (nt2.matches(ts2));  CHECK (nto2.matches(ts2));
          
          CHECK (ns1.matches(""));
          CHECK (ns2.matches("↯"));
          CHECK (nc1.matches("@"));
          CHECK (nc2.matches("~"));
          
          // match due to references sharing the target's ID
          CHECK (rec1.matches(ref1.idi));
          CHECK (ref1.matches(rec1.idi));
          CHECK (rec2.matches(ref2.idi));
          CHECK (ref2.matches(rec2.idi));
          
          // some negative cases...
          CHECK (!ni1.matches(i2));  CHECK (!ni2.matches(i1));
          CHECK (!ni1.matches(l2));  CHECK (!ni2.matches(l1));
          CHECK (!ni1.matches(r2));  CHECK (!ni2.matches(r1));
          CHECK (!ni1.matches(d2));  CHECK (!ni2.matches(d1));
          CHECK (!ni1.matches(c2));  CHECK (!ni2.matches(c1));
          
          CHECK (!nd1.matches(i2));  CHECK (!nd2.matches(i1));
          CHECK (!nd1.matches(l2));  CHECK (!nd2.matches(l1));
          CHECK (!nd1.matches(r2));  CHECK (!nd2.matches(r1));
          CHECK (!nd1.matches(d2));  CHECK (!nd2.matches(d1));
          CHECK (!nd1.matches(c2));  CHECK (!nd2.matches(c1));
          
          // string match is literal
          CHECK (!ns1.matches(" "));
          CHECK (!ns2.matches("↯ "));
          
          GenNode copy(ni1);
          CHECK (copy == ni1);
          
          copy.data = 2*i1;
          CHECK (copy != ni1);
          CHECK (copy.idi  == ni1.idi);
          CHECK (not copy.data.matchData(ni1.data));
          
          // NOTE: "match" operation is shallow on records
          CHECK (copy.matches(ni1)); CHECK (ni1.matches(copy));
        }
      
      
      /** @test simplified notation for access to nested record properties.
       * This is a somewhat questionable shorthand, insofar it allows to "probe"
       * the contents of a GenNode to some limited extent. Generally speaking,
       * we'd prefer if the code using GenNode operates based on precise
       * structural knowledge, instead of peeking into the data.
       */
      void
      convenienceRecAccess()
        {
          GenNode n1(42);
          GenNode n2 = MakeRec().type("spam").genNode("eggs");
          GenNode n3 = MakeRec().attrib("Ψ", Time(3,2,1)).genNode();
          
          CHECK (not n1.isNamed());
          CHECK (    n2.isNamed());
          CHECK (not n3.isNamed());
          
          CHECK (not n1.isNested());
          CHECK (    n2.isNested());
          CHECK (    n3.isNested());
          
          CHECK (n1.data.recordType() == util::BOTTOM_INDICATOR);
          CHECK (n2.data.recordType() == "spam"                );
          CHECK (n3.data.recordType() == Rec::TYPE_NIL         );
          
          CHECK (not n1.hasAttribute("baked beans"));
          CHECK (not n2.hasAttribute("baked beans"));
          CHECK (not n3.hasAttribute("baked beans"));
          
          CHECK (not n1.hasAttribute("Ψ"));
          CHECK (not n2.hasAttribute("Ψ"));
          CHECK (    n3.hasAttribute("Ψ"));
          
          CHECK (not n1.retrieveAttribute<float>("Ψ"));
          CHECK (not n2.retrieveAttribute<float>("Ψ"));
          CHECK (not n3.retrieveAttribute<float>("Ψ"));
          
          CHECK (not n1.retrieveAttribute<Time>("Ψ"));
          CHECK (not n2.retrieveAttribute<Time>("Ψ"));
          CHECK (    n3.retrieveAttribute<Time>("Ψ"));
          
          CHECK (Time(3,2,1) == *n3.retrieveAttribute<Time>("Ψ"));
          CHECK (std::nullopt == n2.retrieveAttribute<Time>("Ψ"));
          
          CHECK (not n1.hasChildren());   // a simple value GenNode is not nested and thus can not have children
          CHECK (not n2.hasChildren());   // n2 is nested (holds a Rec), but has an empty scope
          CHECK (not n3.hasChildren());   // n3 is likewise nested, but holds only attributes, no children
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (GenNode_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
