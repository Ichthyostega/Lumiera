/*
  DelStash(Test)  -  verify a facility to memorise and trigger deleter functions

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

/** @file del-stash-test.cpp
 ** unit test \ref DelStash_test
 */


#include "lib/test/run.hpp"
#include "lib/del-stash.hpp"

#include <cstdlib>



namespace lib {
namespace test{
  
  using std::rand;
  
  
  
  namespace { // probe victims
    
    ulong MAX_MASS = 200;  // number of victims to kill at once
    
    ulong checksum = 0;
    
    
    template<uint siz>
    class Probe
      {
        uint mySiz_;
        char myCrap_[siz];
        
      public:
        Probe()
          : mySiz_(siz)
          {
            REQUIRE (siz);
            for (uint i=0; i<siz; ++i)
              {
                char c (rand() % 256);
                checksum += c;
                myCrap_[i] = c;
              }
          }
        
       ~Probe()
          {
            CHECK (siz == mySiz_, "using wrong type information for de-allocation");
            for (uint i=0; i<siz; ++i)
              checksum -= myCrap_[i];
          }
      };
    
    
    template<uint x>
    inline Probe<x>*
    makeViktim ()
    {
      return new Probe<x>();
    }
    
    template<uint x>
    inline void
    feedViktim (DelStash& killer)
      {
        killer.manage (new Probe<x>());
      }
    
    
  }//(End) test data
  
  
  
  
  /************************************************************************//**
   * @test create a bunch of objects with varying type and size, memorising
   *       how to kill them properly. Verify everyone is dead after mass-kill.
   *       Use a checksum not only to verify the number of objects created and
   *       destroyed, but also the individual (random) contents of the data
   *       within the objects, to ensure that the correct destructor
   *       actually is invoked for each type.
   * 
   * @see lib::DelStash
   */
  class DelStash_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          checksum = 0;
          checkSingleKill();
          checkCustomKill();
          checkMassKill();
          checkAutoKill();
        }
      
      
      void
      checkSingleKill ()
        {
          DelStash killer;
          CHECK (0 == killer.size());
          
          killer.manage<short> (NULL);
          CHECK (0 == killer.size());
          
          Probe<5> *p =  makeViktim<5>();
          Probe<7> &r = *makeViktim<7>();
          void     *v =  makeViktim<9>();
          CHECK (0 < checksum);
          
          killer.manage (p);
          killer.manage (r);
          killer.manage (static_cast<Probe<9>*> (v));
          
          CHECK (3 == killer.size());
          
          killer.kill (r);
          CHECK (2 == killer.size());
          
          killer.kill (p);
          CHECK (1 == killer.size());
          
          killer.kill (p);
          CHECK (1 == killer.size()); // spurious kill requests ignored
          
          killer.kill (v);
          CHECK (0 == killer.size());
          CHECK (0 == checksum);
        }
      
      
      void
      feedViktims (DelStash& killer)
        {
          for (uint i=1; i <= MAX_MASS; ++i)
            switch (i% 5) {
              case 0: feedViktim<12> (killer); break;
              case 1: feedViktim<23> (killer); break;
              case 2: feedViktim<34> (killer); break;
              case 3: feedViktim<45> (killer); break;
              case 4: feedViktim<56> (killer); break;
            }
        }
      
      
      void
      checkMassKill ()
        {
          DelStash killer;
          CHECK (0 == killer.size());
          
          CHECK (0 == checksum);
          CHECK (0 == killer.size());
          
          feedViktims (killer);
          CHECK (MAX_MASS == killer.size());
          
          killer.killAll();
          CHECK (0 == killer.size());
          CHECK (0 == checksum);
        }
      
      
      void
      checkAutoKill()
        {
          {
            DelStash killer;
            CHECK (0 == killer.size());
            CHECK (0 == checksum);
            
            feedViktims (killer);
            Probe<444> * individuum = makeViktim<444>();
            killer.manage (individuum);
            feedViktims (killer);
            killer.manage (makeViktim<5555>());
            feedViktims (killer);
            
            CHECK (3*MAX_MASS + 2 == killer.size());
            
            killer.kill(individuum);
            CHECK (3*MAX_MASS + 1 == killer.size());
            
            CHECK (0 < checksum);
          }// killer going out of scope...
          
          CHECK (0 == checksum);
        }
      
      
      /** @test use a custom-provided
       *        deleter function 
       */
      void
      checkCustomKill ()
        {
          DelStash killer;
          CHECK (0 == killer.size());
          
          /** a very specific setup,
           *  bound to mess up the checksum,
           *  unless the random bias is removed
           *  by the custom deleter function
           */ 
          class Special
            : Probe<555>
            {
              char secret_;
              
            public:
              Special()
                : Probe<555>()
                , secret_('a' + (rand() % (1+'z'-'a')))
                {
                  checksum += secret_;
                }
              
              static void
              selfKill (void *it)
                {
                  Special *self = static_cast<Special*> (it);
                  checksum -= self->secret_;
                  delete self;
                }
            };
          
          
          void * type_erased = new Special();
          CHECK (0 < checksum);
          
          killer.manage (type_erased, &Special::selfKill);
          CHECK (1 == killer.size());
          
          killer.kill(type_erased);
          CHECK (0 == killer.size());
          CHECK (0 == checksum);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DelStash_test, "unit common");
  
  
}} // namespace lib::test
