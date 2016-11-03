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

/** @file §§§
 ** unit test TODO §§§
 */


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
  
  
  
  
  
  /***********************************************************************//**
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
          verify_Knuth_workaround();
        }
      
      
      typedef boost::hash<string> BoostStringHasher;
      typedef std::map<size_t, string> StringsTable;
      
      
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
      
      
      
      /** @test verify a well-known pragmatic trick to help with unevenly spaced hash values.
       * The boost::hash function is known to perform poorly on strings with common prefix
       * plus running number. The mentioned trick (attributed to Donald Knuth) is spread the
       * input numbers by something below the full domain, best close to the golden ratio;
       * bonus points if this number is also a prime. An additional factor of 2 does not hurt
       * (so in case of 64bit platform).
       * 
       * In our case, it is sufficient to apply this trick to the trailing four digits;
       * without this trick, we get the first collisions after about 20000 running numbers.
       * @note on x86_64, even just spreading the trailing two digits seem to be sufficient
       *      to remove any collisions from the first 100000 numbers.
       * @see BareEntryID
       */
      void
      verify_Knuth_workaround()
        {
          StringsTable hashValues;
          string prefix = "Entry.";
          const size_t seed = rand();
          
          const size_t KNUTH_MAGIC = 2654435761;
          
          uint collisions(0);
          for (uint i=0; i<20000; ++i)
            {
              string candidate = prefix + lexical_cast<string> (i);
              size_t l = candidate.length();
              size_t hashVal = seed;
              
              boost::hash_combine(hashVal, KNUTH_MAGIC * candidate[l-1]);
              boost::hash_combine(hashVal, KNUTH_MAGIC * candidate[l-2]);
              boost::hash_combine(hashVal, KNUTH_MAGIC * candidate[l-3]);
              boost::hash_combine(hashVal, KNUTH_MAGIC * candidate[l-4]);
              boost::hash_combine(hashVal, candidate);
              
              if (contains (hashValues, hashVal))
                {
                  ++collisions;
                  string other = hashValues[hashVal];
                  cout << "Hash collision between " << i << " and " << other <<endl;
                }
              hashValues[hashVal] = candidate;
            }
          CHECK (!collisions, "the Knuth trick failed to spread our hash values evenly enough, what a shame...");
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (HashGenerator_test, "unit common");
  
  
}} // namespace lib::test
