/*
  HashGenerator(Test)  -  hash value generation details

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "lib/util.hpp"

#include <boost/functional/hash.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>
#include <map>

using boost::lexical_cast;
using util::contains;
using std::string;
using std::cout;
using std::endl;


namespace lib {
namespace test{
  
  
  
  
  
  /***************************************************************************
   * @test cover various detail aspects regarding hash value generation
   *       - weakness of boost::hash
   *       
   * @see HashIndexed_test
   * @see EntryID_test 
   */
  class HashGenerator_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          demonstrate_boost_hash_weakness();
        }
      
      
      /** @test demonstrate a serious weakness of boost::hash for strings.
       * When hashing just the plain string representation of integers,
       * we get collisions already with small numbers below 100000.
       * This is counter-intuitive, as the generated hash values
       * are 17 digits long and could span much wider scale.
       * 
       * This problem is especially dangerous when storing objects keyed
       * by a string-id, which is generated from running numbers.
       */
      void
      demonstrate_boost_hash_weakness ()
        {
          typedef boost::hash<string> BoostStringHasher;
          typedef std::map<size_t, string> StringsTable;
          
          BoostStringHasher hashFunction;
          StringsTable hashValues;
          string prefix = "Entry.";
          uint collisions(0);
          for (uint i=0; i<100000; ++i)
            {
              string candidate = prefix + lexical_cast<string> (i);
              size_t hashVal = hashFunction(candidate);
              
              if (contains (hashValues, hashVal))
                {
                  ++collisions;
                  string other = hashValues[hashVal];
                  cout << "Duplicate at "<< i    << endl;
                  cout << "existing--->" << other  << endl;
                  cout << "new-------->" << candidate << endl;
                  
                  size_t exHash = hashFunction(other);
                  size_t newHash = hashFunction(candidate);
                  cout << "hash-ex---->" << exHash << endl;
                  cout << "hash_new--->" << newHash << endl;
                }
              hashValues[hashVal] = candidate;
            }
          CHECK (0 < collisions, "boost::hash for strings is expected to produce collisions");
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (HashGenerator_test, "unit common");
  
  
}} // namespace lib::test
