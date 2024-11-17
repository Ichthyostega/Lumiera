/*
  HashIndexed(Test)  -  proof-of-concept test for a hash based and typed ID

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file hash-indexed-test.cpp
 ** unit test \ref HashIndexed_test
 */


#include "lib/test/run.hpp"

#include "lib/hash-indexed.hpp"
#include "lib/util.hpp"

#include <unordered_map>

using util::isSameObject;


namespace lib {
namespace test{
  
  /* ==  a hierarchy of test-dummy objects to use the HashIndexed::ID == */
  
  struct DummyAncestor
    {
      long xyz_;
    };
  
  struct TestB;                                  ///< Base class to mix in the hash ID facility
  typedef HashIndexed<TestB, hash::LuidH> Mixin; ///< actual configuration of the mixin
  
  struct TestB : DummyAncestor, Mixin
    {
      TestB () {}
      TestB (ID const& refID) : Mixin (refID) {}
      
      bool operator== (TestB const& o)  const { return this->getID() == o.getID(); }
    };
  struct TestDA : TestB {};
  struct TestDB : TestB {};
  
  
  
  
  /***********************************************************************//**
   * @test proof-of-concept test for a generic hash based and typed ID struct.
   *       - check the various ctors 
   *       - check default assignment works properly
   *       - check assumptions about memory layout
   *       - check equality comparison
   *       - extract LUID and then cast LUID back into ID
   *       - use the embedded hash ID (LUID) as hashtable key
   *       
   * @see lib::HashIndexed::Id
   * @see mobject::Placement real world usage example 
   */
  class HashIndexed_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          checkBasicProperties();
          checkLUID_passing();
          
          //            ---key-type-------+-value-+-hash-function--- 
          buildHashtable<TestB::Id<TestDB>, TestDB, TestB::UseHashID> ();
          buildHashtable<TestDB,            TestDB, TestB::UseEmbeddedHash>();
        }
      
      
      void
      checkBasicProperties ()
        {
          TestB::Id<TestDA> idDA;
          
          TestB bb (idDA);
          
          TestB::Id<TestDB> idDB1 ;
          TestB::Id<TestDB> idDB2 (idDB1);
          
          CHECK (sizeof (idDB1)     == sizeof (idDA) );
          CHECK (sizeof (TestB::ID) == sizeof (hash::LuidH));
          CHECK (sizeof (TestDA)    == sizeof (hash::LuidH) + sizeof (DummyAncestor));
          
          CHECK (idDA  == bb.getID() );
          CHECK (idDB1 == idDB2 );            // equality handled by the hash impl (here LuidH)
          
          TestDA d1;
          TestDA d2;
          CHECK (d1.getID() != d2.getID());   // should be different because LUIDs are random
          
          d2 = d1; 
          CHECK (d1.getID() == d2.getID());   // default assignment operator works as expected
        }
      
      
      void
      checkLUID_passing ()
        {
          TestB::Id<TestDA> idOrig;
          
          lumiera_uid plainLUID;
          lumiera_uid_copy (&plainLUID, idOrig.get());
          
          // now, maybe after passing it through a Layer barrier...
          TestB::ID const& idCopy = reinterpret_cast<TestB::ID & > (plainLUID);
          
          CHECK (idOrig == idCopy);
        }
      
      
      template<class KEY, class VAL, class HashFunc>
      void
      buildHashtable ()
        {
          typedef std::unordered_map<KEY, VAL, HashFunc> Hashtable;
          
          Hashtable tab;
          
          VAL o1;  KEY key1 (o1);
          VAL o2;  KEY key2 (o2);
          VAL o3;  KEY key3 (o3);
          
          tab[key1] = o1;                             // store copy into hashtable
          tab[key2] = o2;
          tab[key3] = o3;
          
          CHECK (!isSameObject (o1, tab[key1]));     // indeed a copy...
          CHECK (!isSameObject (o2, tab[key2]));
          CHECK (!isSameObject (o3, tab[key3]));
          
          CHECK (o1.getID() == tab[key1].getID());   // but "equal" by ID
          CHECK (o2.getID() == tab[key2].getID());
          CHECK (o3.getID() == tab[key3].getID());
          
          CHECK (o1.getID() != tab[key2].getID());
          CHECK (o1.getID() != tab[key3].getID());
          CHECK (o2.getID() != tab[key3].getID());
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (HashIndexed_test, "unit common");
  
  
}} // namespace lib::test
