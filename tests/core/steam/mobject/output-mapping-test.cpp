/*
  OutputMapping(Test)  -  verify generic output designation mapping

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file output-mapping-test.cpp
 ** unit test \ref OutputMapping_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "proc/mobject/output-mapping.hpp"
#include "proc/asset/pipe.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"

#include <string>

using util::_Fmt;
using util::isnil;
using std::string;


namespace proc {
namespace mobject {
namespace test {
  
  using asset::Pipe;
  using asset::PPipe;
  
  typedef asset::ID<Pipe> PID;
  
  
  /*****************************************************************************//**
   * @test create a synthetic / example mapping to verify generic mapping behaviour.
   *       We're creating a custom mapping type here, for this test only: The
   *       struct DummyDef provides a "definition context" for this custom mapping
   *       - there is a function to retrieve the actual target object
   *         for any target pipe stored in the mapping. In this case,
   *         we just extract the name-ID string from the pipe as result
   *       - as an additional sideeffect, this DummyDef::output functor
   *         also defines the Target type of this custom mapping to be std::string
   *       - DummyDef::buildQuery provides a template query, to be issued whenever
   *         a yet nonexistent mapping is requested. In this special case here
   *         we query for a pipe with the name "master_XXX", where XXX denotes
   *         the stream-type of the source pipe to be mapped.
   *       
   * @see  mobject::OutputDesignation
   * @see  mobject::session::Binding
   */
  class OutputMapping_test : public Test
    {
      struct DummyDef
        {
          string
          output (PID target)
            {
              return Pipe::lookup(target)->ident.name;
            }
          
          Query<Pipe>
          buildQuery (PID sourcePipeID, uint seqNr =0)
            {
              PPipe srcP = Pipe::lookup (sourcePipeID);
              _Fmt queryPattern{"id(master_%1%), stream(%1%), ord(%2%)"};
              return Query<Pipe> (queryPattern % srcP->getStreamID().getSym() % seqNr);
            }
        };
      
      typedef OutputMapping<DummyDef> Mapping;
      
      virtual void
      run (Arg)
        {
          map_and_retrieve();
          instance_copy();
          default_mapping();
        }
      
      
      void
      map_and_retrieve()
        {
          Mapping map;
          CHECK (isnil (map));
          
          PPipe p1 = Pipe::query("id(hairy)");
          PPipe p2 = Pipe::query("id(furry)");
          PPipe pX = Pipe::query("id(curly)");
          
          map[p1] = p2;
          CHECK (!isnil (map));
          CHECK (1 == map.size());
          CHECK (map[p1] == "furry");
          CHECK (map[p1].isValid());
          CHECK (map[p1]);
          
          CHECK (!map.contains (pX));
          CHECK (!map.contains (p2));
          
          // create an unconnected mapping
          map[pX].disconnect();
          CHECK (map.contains (pX));
          CHECK (!map[pX].isValid());
          CHECK (!map[pX]);
        }
      
      
      void
      instance_copy()
        {
          Mapping m1;
          
          PPipe p1 = Pipe::query("id(hairy)");
          PPipe p2 = Pipe::query("id(furry)");
          PPipe pi = Pipe::query("id(nappy)");
          
          m1[pi] = p1;
          Mapping m2(m1);
          CHECK (!isnil (m2));
          CHECK (1 == m2.size());
          CHECK (m1[pi] == "hairy");
          CHECK (m2[pi] == "hairy");
          
          m1[pi] = p2;
          CHECK (m1[pi] == "furry");
          CHECK (m2[pi] == "hairy");
          
          m2 = m1;
          CHECK (m1[pi] == "furry");
          CHECK (m2[pi] == "furry");
          
          m1.clear();
          CHECK (isnil(m1));
          CHECK (!isnil(m2));
          CHECK (m2[pi] == "furry");
          CHECK (!m1.contains (pi));
        }
      
      
      void
      default_mapping()
        {
          Mapping map;
          CHECK (isnil (map));
          
          PPipe p1 = Pipe::query("stream(hairy)");
          PPipe p2 = Pipe::query("stream(furry)");
          
          CHECK (map[p1] == "master_hairy");
          CHECK (map[p2] == "master_furry");
          
          // create new mapping to an explicitly queried target
          Query<Pipe> some_pipe ("pipe(super_curly)");
          CHECK (map[some_pipe] == "super_curly");
          
          // create a new mapping to the 2nd master for "furry" data
          Query<Pipe> special_bus ("stream(furry), ord(2)");
          CHECK (map[special_bus] == "master_furry.2");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (OutputMapping_test, "unit session builder");
  
  
  
}}} // namespace proc::mobject::test
