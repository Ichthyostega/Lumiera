/*
  AllocationCluster(Test)  -  verify bulk (de)allocating a family of objects
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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



#include "common/test/run.hpp"
#include "common/util.hpp"

#include "lib/allocationcluster.hpp"

#include <vector>
#include <limits>
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;
using util::for_each;
using util::isnil;
using ::Test;

using std::numeric_limits;
using std::vector;

namespace lib {
  namespace test {
    
    namespace { // a family of test dummy classes
      
      uint NUM_CLUSTERS = 5;
      uint NUM_OBJECTS  = 500;
      uint NUM_FAMILIES = 5;
      
      long checksum = 0;
      
      template<uint i>
      class Dummy
        {
          char content[i];
          
        public:
          Dummy (char id)
            {
              content[0] = id;
              checksum += id;
            }
          Dummy (char i1, char i2, char i3=0)
            {
              char id = i1 + i2 + i3;
              content[0] = id;
              if (0 == (rand() % 20))
                throw id;
              checksum += id;
            }
          
          ~Dummy()
            {
              checksum -= content[0];
            }
        };
      
      
      typedef vector<AllocationCluster> ClusterList;
      
      inline char
      truncChar (uint x)
        {
          return x % numeric_limits<char>::max();
        }
      
      template<uint i>
      void
      place_object (AllocationCluster& clu, uint id)
        {
          clu.create<Dummy<i> > (id);
        }
      
      typedef void (Invoker)(AllocationCluster&, uint);
      
      Invoker* invoke[20] = { &place_object<1>
                            , &place_object<2>
                            , &place_object<3>
                            , &place_object<5>
                            , &place_object<10>
                            , &place_object<13>
                            , &place_object<14>
                            , &place_object<15>
                            , &place_object<16>
                            , &place_object<17>
                            , &place_object<18>
                            , &place_object<19>
                            , &place_object<20>
                            , &place_object<25>
                            , &place_object<30>
                            , &place_object<35>
                            , &place_object<40>
                            , &place_object<50>
                            , &place_object<100>
                            , &place_object<200>
                            };
      
      void
      fillIt (AllocationCluster& clu)
        {
          if (20<NUM_FAMILIES)
            NUM_FAMILIES = 20;
          
          for (uint i=0; i<NUM_OBJECTS; ++i)
            {
              char id = truncChar(i);
              (*(invoke[rand() % NUM_FAMILIES])) (clu,id);
            }
        }
      
    }
    
    
    /*************************************************************************
     *  @test verify the proper workings of our custom allocation scheme
     *        managing families of interconnected objects for the segments
     *        of the low-level model.
     */
    class AllocationCluster_test : public Test
      {
        virtual void 
        run (Arg arg)
          {
            if (0 < arg.size()) NUM_CLUSTERS = lexical_cast<uint> (arg[0]);
            if (1 < arg.size()) NUM_OBJECTS  = lexical_cast<uint> (arg[1]);
            if (2 < arg.size()) NUM_FAMILIES = lexical_cast<uint> (arg[2]);
            
            checkAllocation();
            checkErrorHandling();
          }
        
        void
        checkAllocation()
          {
            ASSERT (0==checksum);
            {
              ClusterList clusters (NUM_CLUSTERS);
              for_each (clusters, fillIt);
              ASSERT (0!=checksum);
            }
            ASSERT (0==checksum);
          }
        
        void
        checkErrorHandling()
          {
            ASSERT (0==checksum);
            {
              AllocationCluster clu;
              for (uint i=0; i<NUM_OBJECTS; ++i)
                try
                  {
                    char i1 = truncChar(i);
                    char i2 = truncChar(rand() % 5);
                    clu.create<Dummy<1> > (i1,i2);
                  }
                catch (char id)
                  {
                    checksum -= id;
                  }
            }
            ASSERT (0==checksum);
          }
      };
    
      LAUNCHER (AllocationCluster_test, "unit common");


  }// namespace test
    
} // namespace lib

