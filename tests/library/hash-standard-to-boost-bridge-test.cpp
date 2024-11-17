/*
  HashStandardToBoostBridge(Test)  -  verify a workaround to combine both systems of hash function definition

   Copyright (C)
     2014,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file hash-standard-to-boost-bridge-test.cpp
 ** unit test \ref HashStandardToBoostBridge_test
 */


#include "lib/test/run.hpp"
#include "lib/hash-standard.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"

#include <boost/functional/hash.hpp>
#include <unordered_set>
#include <string>
#include <vector>

using util::contains;
using std::unordered_set;
using std::vector;
using std::string;


namespace lib {
namespace test{
  
  /* ==  custom data items, each with its own private hash function == */
  
  /**
   * Custom-Datatype 1:
   * uses std::hash extension point
   */
  class S
    {
      string s_;
      
      friend std::hash<S>;
      
    public:
      S(string ss ="")
        : s_(ss)
        { }
      
      bool
      operator== (S const& os)  const
        {
          return s_ == os.s_;
        }
    };
  
}}
// this is ugly: we have to close our own namespace and open namespace std
// for the sole purpose of placing our private hash function into the official extension point
  
namespace std {
  
  using lib::test::S;
  
  template<>
  struct hash<S>
    {
      size_t
      operator() (S const& val)  const noexcept
        {
          hash<string> string_hasher;
          return string_hasher(val.s_);
        }
    };
  
} // and back again into our own namespace...
namespace lib {
namespace test{
  
  
  
  
  /**
   * Custom-Datatype 2:
   * uses boost::hash extension point
   */
  class V
    {
      vector<string> v_;
      
    public:
      V(string ss ="")
        {
          v_.push_back(ss);
        }
      
      bool
      operator== (V const& ov)  const
        {
          return v_ == ov.v_;
        }
      
      friend size_t
      hash_value (V const& v)
      {
        return boost::hash_value(v.v_);
      }
    };
  
  
  
  
  /***********************************************************************//**
   * @test document and verify an automatic bridge to integrate boost-style
   *       hash functions with the new \c std::hash template.
   *       Currently (as of 2014) this requires us to "hijack" the standard
   *       library definition to defeat a static assertion, which otherwise
   *       would defeat any attempts based on SFINAE
   * 
   * This test builds two custom types, one with a std::hash compliant
   * hash function extension point, the other one with a boost::hash style
   * custom hash function, to be picked up by ADL. By virtue of the automatic
   * bridging template, we're able to use the boost-style definition at places
   * where the std::hash is expected -- most notably in STL hashtable containers.
   * 
   * @see hash-standard.hpp
   * @see lib::HashIndexed
   */
  class HashStandardToBoostBridge_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          checkHashFunctionInvocation();
          checkHashtableUsage();
        }
      
      
      void
      checkHashFunctionInvocation ()
        {
          string p("Путин"), pp(p);
          S s(p), ss(pp);
          V v(p), vv(pp);
          
          std::hash<string>   std_stringHasher;
          boost::hash<string> boo_stringHasher;
          
          std::hash<S>   std_customHasher;
          boost::hash<V> boo_customHasher;
          
          CHECK(0 < std_stringHasher(p));
          CHECK(0 < boo_stringHasher(p));
          CHECK(0 < std_customHasher(p));
          CHECK(0 < boo_customHasher(p));
          
          CHECK(std_stringHasher(p) == std_stringHasher(pp));
          CHECK(boo_stringHasher(p) == boo_stringHasher(pp));
          CHECK(std_customHasher(s) == std_customHasher(ss));
          CHECK(boo_customHasher(v) == boo_customHasher(vv));
          
          // should be equal too, since the custom impl delegates to the standard string hasher
          CHECK(std_stringHasher(p) == std_customHasher(s));
          
          
          // verify the trait used to build the automatic bridge
          CHECK(! lib::meta::provides_BoostHashFunction<S>::value);
          CHECK(  lib::meta::provides_BoostHashFunction<V>::value);
          
          //verify the automatic bridge
          std::hash<V> boo2std_crossHar;
          CHECK(0 < boo2std_crossHar(v));
          
          // the std hash function delegates to the boost function...
          CHECK(boo2std_crossHar(v) == boo_customHasher(v));
          
//  Note: does not compile,
//        since there is not automatic bridge to use std::hash from boost::hash
//  
//        boost::hash<S>()(s);
        }
      
      
      void
      checkHashtableUsage ()
        {
          string o1{randStr(5)},
                 o2{randStr(6)},
                 o3{randStr(7)},
                 o4{randStr(8)};
          S s1{o1}, s2{o2}, s3{o3}, s4{o4}, s5{o2}, s6{o1};
          V v1{o1}, v2{o2}, v3{o3}, v4{o4}, v5{o3}, v6{o2};
          
          unordered_set<S> us{s1,s2,s3,s4,s5,s6};
          unordered_set<V> uv{v1,v2,v3,v1,v5,v6};
          
          CHECK(4 == us.size());
          CHECK(3 == uv.size());
          
          CHECK(contains (us, s1));
          CHECK(contains (us, s2));
          CHECK(contains (us, s3));
          CHECK(contains (us, s4));
          CHECK(contains (us, s5));
          CHECK(contains (us, s6));
          
          CHECK( contains (uv, v1));
          CHECK( contains (uv, v2));
          CHECK( contains (uv, v3));
          CHECK(!contains (uv, v4));
          CHECK( contains (uv, v5));
          CHECK( contains (uv, v6));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (HashStandardToBoostBridge_test, "unit common");
  
  
}} // namespace lib::test
