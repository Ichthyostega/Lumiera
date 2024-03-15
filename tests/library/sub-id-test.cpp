/*
  SubID(Test)  -  exploring possible properties of an extensible symbolic identifier

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file sub-id-test.cpp
 ** unit test \ref SubID_test
 */


#include "lib/test/run.hpp"
#include "lib/util.hpp"
#include "lib/util-foreach.hpp"
#include "lib/format-cout.hpp"

#include "lib/sub-id.hpp"

#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <functional>
#include <vector>
#include <string>



namespace lib {
namespace test{
  
  using util::for_each;
  using std::bind;
  using std::placeholders::_1;
  using boost::hash;
  using std::vector;
  using std::string;
  
  
  
  namespace { // test data
    
    enum Colour { R,G,B };
    
    
    inline string
    toString (Colour c) ///< make the enum printable
    {
      static string sym("RGB");
      return sym.substr(c,1);
    }
    
  }
  
  
  
  
  /************************************************************************//**
   * @test for now (9/09) this is a playground for shaping a vague design idea
   *       - base types and casts 
   *       - exploring some extensions
   *       - use this ID as Hash-Map key
   *       
   * @see lib::SubID
   */
  class SubID_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          checkBaseType();
          checkExtension();
          checkSubIDHash();
        }
      
      
      void
      checkBaseType ()
        {
          typedef SubId<Colour> CID;
          CID c1 (R);
          CID c2 (G);
          CID c3 (B);
          
          cout << "...." << c1 << c2 << c3 << endl;
        }
      
      
      void
      checkExtension ()
        {
          typedef SubId<uint>   UID;
          
          typedef ExtendedSubId<Colour, UID> CUID;
          
          SubID const& id1 = CUID(R, 12);
          SubID const& id2 = CUID(G, 13);
          
          cout << "id1=" << id1 << endl;
          cout << "id2=" << id2 << endl;
        }
      
      
      void
      checkSubIDHash()
        {
          typedef SubId<Colour> CID;
          typedef SubId<uint>   UID;
          typedef ExtendedSubId<Colour, UID> CUID;
          
          vector<CID> simpleIDs;
          simpleIDs.push_back(CID(R));
          simpleIDs.push_back(CID(R));
          simpleIDs.push_back(CID(G));
          simpleIDs.push_back(CID(B));
          
          vector<CUID> extendedIDs;
          extendedIDs.push_back(CUID(R,22));
          extendedIDs.push_back(CUID(R,22));  // note the duplicates get dropped
          extendedIDs.push_back(CUID(R,23));
          extendedIDs.push_back(CUID(R,24));
          extendedIDs.push_back(CUID(G,24));
          extendedIDs.push_back(CUID(B,25));
          
          buildHashtable<CID>  (simpleIDs);
          buildHashtable<CUID> (extendedIDs);
        }
      
      
      
      template<class KEY>
      struct HashTable
        : std::unordered_map<KEY, string, hash<KEY>>
        {
          void
          add (KEY key)
            {
              (*this)[key] = string(key);
            }
          
          void
          verify (KEY key)
            {
              cout << "verify....." << key << endl;
              CHECK (string(key) == (*this)[key]);
            }
        };
      
      
      template<class KEY>
      void
      buildHashtable (vector<KEY> keys)
        {
          
          typedef HashTable<KEY> HTab;
          HTab tab;
          
          for_each (keys, bind (&HTab::add,    ref(tab), _1 ));
          for_each (keys, bind (&HTab::verify, ref(tab), _1 ));
          
          cout << "Elements in hashtable: " << tab.size() << endl;
        }
      
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (SubID_test, "unit common");
  
  
}} // namespace lib::test
