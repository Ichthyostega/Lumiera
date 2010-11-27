/*
  OutputMapping(Test)  -  verify generic output designation mapping
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
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
//#include "proc/asset/media.hpp"
//#include "proc/mobject/session.hpp"
//#include "proc/mobject/session/testclip.hpp"
#include "proc/asset/pipe.hpp"
//#include "proc/mobject/placement.hpp"
#include "proc/mobject/output-mapping.hpp"
#include "lib/util.hpp"
//#include "proc/mobject/session/mobjectfactory.hpp"   ////TODO: avoidable?

//#include <boost/format.hpp>
//#include <iostream>
#include <string>

//using boost::format;
//using lumiera::Time;
//using util::contains;
using util::isnil;
using std::string;
//using std::cout;


namespace mobject {
namespace test  {
  
//using asset::VIDEO;
  using asset::Pipe;
  using asset::PPipe;
  
  typedef asset::ID<Pipe> PID;

  namespace {
    
  }
  
  
  /*******************************************************************
   * @test create a synthetic / example mapping to verify
   *       generic mapping behaviour
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
          
          CHECK (!map[pX]);
          CHECK (!map[p2]);
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
          CHECK (m1[p1] == "hairy");
          CHECK (m2[p1] == "hairy");
          
          m1[pi] = p2;
          CHECK (m1[p1] == "furry");
          CHECK (m2[p1] == "hairy");
          
          m2 = m1;
          CHECK (m1[p1] == "furry");
          CHECK (m2[p1] == "furry");
          
          m1.clear();
          CHECK (isnil(m1));
          CHECK (!isnil(m2));
          CHECK (m2[p1] == "furry");
          CHECK (!m1[p1].isValid());
        }
      
      
      void
      default_mapping()
        {
          Mapping map;
          CHECK (isnil (map));
          
          PPipe p1 = Pipe::query("stream(hairy)");
          PPipe p2 = Pipe::query("stream(furry)");
          
          CHECK (map[p1] == "master(hairy)");
          CHECK (map[p2] == "master(furry)");
          
          Query<Pipe> special_bus ("stream(furry), ord(2)");
          CHECK (map[special_bus] == "master.2(furry)");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (OutputMapping_test, "unit session");
  
  
  
}} // namespace mobject::test
