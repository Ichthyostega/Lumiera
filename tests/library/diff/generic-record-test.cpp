/*
  GenericRecord(Test)  -  introspective representation of object-like data

   Copyright (C)
     2015,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file generic-record-test.cpp
 ** unit test \ref GenericRecord_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"
#include "lib/diff/record.hpp"
#include "lib/itertools.hpp"

#include <string>
#include <vector>

using std::string;
using util::isSameObject;
using util::isnil;
using util::join;
using std::vector;
using std::swap;


namespace lib {
namespace diff{
namespace test{
  
  using LERR_(INVALID);
  using LERR_(INDEX_BOUNDS);
  using LERR_(BOTTOM_VALUE);
  
  namespace {//Test fixture....
    
    using Seq  = vector<string>;
    using RecS = Record<string>;
    
    template<class IT>
    inline Seq
    contents (IT const& it)
    {
      Seq collected;
      append_all (it, collected);
      return collected;
    }
    
    inline Seq
    contents (RecS const& rec_of_strings)
    {
      return contents (rec_of_strings.begin());
    }
    
    template<class X>
    inline Seq
    strings (std::initializer_list<X> const& con)
    {
      Seq collected;
      for (auto elm : con)
        collected.push_back(elm);
      return collected;
    }
    
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  /*************************************************************************************//**
   * @test Verify properties of a special collection type meant for external representation
   *       of object-like data, especially for symbolic representation in diff messages.
   *       - there is a type meta attribute
   *       - a Record can have attributes (by key) and contents (ordered list of values)
   *       - various kinds of iterators are provided
   *       - besides the regular constructor, which explicitly takes a type, a collection
   *         of attributes, and a collection of contents, there is a convenience constructor
   *         especially for literal notation and data definition. This one figures out the
   *         break between attributes and contents automatically; a type meta attribute
   *         is recognised and the first element without a given key or ID ends the
   *         attributes and starts the content scope
   *       - Record elements are conceived as values and equality is defined in terms
   *         of their contents, including the order (no normalisation, no sorting)
   *       - they are \em immutable after construction. But we provide a Mutator
   *         for remoulding a given element, enabling object builder notation.
   *       - a reference wrapper for handling of large structures is provided.
   * @remarks this test uses the specialisation \c Record<string> solely, to cover the
   *       basic properties and behaviour, while leaving out the complexities of specific
   *       payload data types. For the actual use case, the symbolic description of
   *       data structure differences, we use a specific "value" within Record,
   *       the diff::GenNode, which is a limited typesafe Variant element, and in
   *       turn allows Record<GenNode> as embedded payload. Effectively this creates
   *       a "recursive data type", which is key to typesafe functional processing of
   *       unlimited data structures. The design of diff::Record only makes sense with
   *       this use case in mind; most notably, we have the keys (attribute names)
   *       embedded within the value payload, which turns attributes into just another
   *       content scope with special access operations. This also explains, why we
   *       do not normalise the content in any way; content is meant to reflect
   *       other data structures, which are normalised and maintained by their owner. 
   *       
   * @see GenNode_test
   * @see tree-diff.cpp
   */
  class GenericRecord_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          verifyCreation();
          verifyMutations();
          copy_and_move();
          equality();
          wrapRef();
        }
      
      
      void
      simpleUsage()
        {
          RecS enterprise("starship"
                         , strings ({"Name = USS Enterprise"
                                    ,"Registry = NCC-1701-D"
                                    ,"Class = Galaxy"
                                    ,"Owner = United Federation of Planets"
                                    ,"Operator= Starfleet"
                                    ,"built=2363"
                                   })
                         , strings ({"Picard", "Riker", "Data", "Troi", "Worf", "Crusher", "La Forge"})
                         );
          
          CHECK (enterprise.getType() == "starship");
          CHECK (enterprise.get("Registry") == "NCC-1701-D");
          CHECK (enterprise.child(0) == "Picard");
          CHECK (enterprise.child(2) == "Data");
          
          CHECK (enterprise.hasAttribute("Owner"));
          CHECK (!enterprise.hasAttribute("owner"));
          CHECK (!enterprise.hasAttribute("Owner ")); // no normalisation
          
          CHECK (enterprise.contains("Data"));
          CHECK (!enterprise.contains("Woof"));    // it is /Worf/, madam
          CHECK (util::contains (enterprise, "Worf"));
          
          VERIFY_ERROR (INVALID, enterprise.get("warp10"));
          VERIFY_ERROR (INDEX_BOUNDS, enterprise.child(12));
          
          cout << "enterprise = "
               << enterprise <<endl;
          for (string elm : enterprise)
            cout << elm <<endl;
          cout << "--Attributes--"<<endl;
          for (string att : enterprise.attribs())
            cout << att <<endl;
          cout << "--Keys--->" << join (enterprise.keys(), "<->")<<endl;
          cout << "--Vals--->" << join (enterprise.vals(), "<->")<<endl;
          cout << "--Crew--->" << join (enterprise.scope()," | ")<<endl;
        }
      
      
      void
      verifyCreation()
        {
          RecS nil;
          CHECK (isnil(nil));
          CHECK ("NIL" == nil.getType());
          CHECK (RecS::TYPE_NIL == nil.getType());
          
          CHECK (!nil.begin());
          CHECK (nil.begin() == nil.end());
          
          
          RecS untyped({"x"});
          CHECK (!isnil(untyped));
          CHECK ("NIL" == untyped.getType());
          CHECK (Seq{"x"} == contents(untyped));
          CHECK (Seq{"x"} == contents(untyped.scope()));
          CHECK (isnil (untyped.attribs()));
          
          RecS untyped2({"x=y", "z"});
          CHECK (!isnil(untyped2));
          CHECK ("NIL" == untyped2.getType());
          CHECK (Seq({"x=y", "z"}) == contents(untyped2));
          CHECK (Seq{"x"} == contents (untyped2.keys()));
          CHECK (Seq{"y"} == contents (untyped2.vals()));
          CHECK (Seq{"z"} == contents (untyped2.scope()));
          
          
          RecS something({"a=1", "type=thing", "b=2", "c", "d"});
          CHECK (!isnil(something));
          CHECK ("thing" == something.getType());
          CHECK (Seq({"a=1", "b=2", "c", "d"}) == contents(something));
          CHECK (Seq({"a", "b"}) == contents (something.keys()));
          CHECK (Seq({"1", "2"}) == contents (something.vals()));
          CHECK (Seq({"c", "d"}) == contents (something.scope()));
        }
      
      
      void
      copy_and_move()
        {
          RecS a({"a=1", "b=2", "c", "d"});
          RecS b(a);
          CHECK (a.getType() == b.getType());
          CHECK (contents(a) == contents(b));
          CHECK (contents(a.attribs()) == contents(b.attribs()));
          
          CHECK (!isSameObject (a.get("a"), b.get("a")));
          CHECK (!isSameObject (*a.scope(), *b.scope()));
          
          string const& c = *b.scope();
          CHECK ("c" == c);
          
          RecS bb;
          CHECK (isnil(bb));
          bb = move(b);
          CHECK ("2" == bb.get("b"));
          CHECK (isSameObject(c, *bb.scope()));
          
          swap (a, bb);
          CHECK (!isSameObject(c, *bb.scope()));
          CHECK ( isSameObject(c, *a.scope()));
          
          CHECK (isnil (b));
          b = bb;
          CHECK (!isnil (b));
          CHECK (!isSameObject(b.get("a"), bb.get("a")));
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
          RecS az({"a =a","a"});
          
          CHECK (a != aa);   CHECK (aa != a);
          CHECK (aa != aaa); CHECK (aaa != aa);
          CHECK (a != aaa);  CHECK (aaa != a);
          CHECK (a != ax);   CHECK (ax != a);
          CHECK (a != ay);   CHECK (ay != a);
          CHECK (ax != ay);  CHECK (ay != ax);
          CHECK (aaa != ay); CHECK (ay != aaa);
          CHECK (ay != az);  CHECK (az != ay);   // NOTE: attributes are *not* normalised,
                                                //        rather, they are used as-is,
                                               //         thus "a=a" != "a =a"
          RecS a2({"a","aa"});
          CHECK (aa == a2);  CHECK (a2 == aa);
          
          RecS o1("oo", strings({"a=α", "b=β"}), strings({"γ", "δ", "ε"}));
          RecS o2({"type=oo", "a=α", "b=β", "γ", "δ", "ε"});
          RecS o3({"type=oO", "a=α", "b=β", "γ", "δ", "ε"});
          RecS o4({"type=oo", "a=α", "b=β", "c=γ", "δ", "ε"});
          RecS o5({"type=oo", "a=α", "b=β", "γ", "ε", "δ"});
          RecS o6({"type=oo", "a=α", "b=β", "γ", "δ"});
          
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
          CHECK (Seq({"a = 1", "a"}) == contents(aa));
          CHECK (Seq({"a"}) == contents (aa.keys()));
          CHECK (Seq({"1"}) == contents (aa.vals()));
          CHECK (Seq({"a"}) == contents (aa.scope()));
          
          CHECK (mut == aa);
          
          mut.prependChild("⟂");
          mut.set("b", "β");
          mut.set("a", "α");
          
          CHECK (mut != aa);
          
          mut.swap (a);
          CHECK (isnil (mut));
          CHECK (Seq({"a = α", "b = β", "⟂", "a"}) == contents(a));
          CHECK (Seq({"a = 1", "a"}) == contents(aa));
        }
      
      
      void
      wrapRef()
        {
          RecS oo({"type = 🌰", "☿ = mercury", "♀ = venus", "♁ = earth", "♂ = mars", "♃ = jupiter", "♄ = saturn"});
          
          RecordRef<string> empty;
          CHECK (bool(empty) == false);
          CHECK (nullptr == empty.get());
          VERIFY_ERROR (BOTTOM_VALUE, empty.operator RecS&() );
          
          RecordRef<string> ref(oo);
          CHECK (ref);
          CHECK (ref.get() == &oo);
          
          RecS& oor = ref;
          CHECK ("🌰" == oor.getType());
          CHECK (oor.get("♄") == "saturn");
          
          // are copyable but not reassignable
          RecordRef<string> r2 = ref;
          CHECK (r2);
          CHECK (r2.get() == ref.get());
          CHECK (!isSameObject (r2, ref));
          
          // but references are move-assignable
          empty = std::move(r2);
          CHECK (empty);
          CHECK (!r2);
          CHECK (nullptr == r2.get());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (GenericRecord_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
