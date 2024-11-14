/*
  UtilIdentity(Test)  -  helpers for identity and memory location

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file util-identity-test.cpp
 ** unit test \ref UtilIdentity_test
 */


#include "lib/test/run.hpp"
#include "lib/format-obj.hpp"
#include "lib/util.hpp"

#include <utility>
#include <string>

using std::move;
using std::string;


namespace util {
namespace test {
  
  
  
  /*****************************************************************//**
   * @test verify identity based on memory address location,
   *       which can be relevant for custom allocation schemes
   *       and to prevent self-assignment.
   *     - access to a given entitie's address is used as foundation,
   *       with the _special twist_ that a pointer is »unpacked«
   *     - based on this address, an ID-number can be generated
   *     - moreover, two flavours of identity check are provided
   *       + util::isSameObject compares at the level of the
   *         _language object_ — it takes its arguments _solely_
   *         by reference and does not »unpack« a pointer.
   *         The term [object] is used here as in the C++ standard
   *       + util::isSameAdr accepts any mix of references and
   *         pointers, disregarding any type information, thereby
   *         _»unpacking«_ the address information contained in a
   *         pointer is (i.e. the address of the pointee is used)
   * [object]: https://en.cppreference.com/w/cpp/language/type
   */
  class UtilIdentity_test : public Test
    {
      
      void
      run (Arg)
        {
          verify_getAdr();
          verify_addrID();
          verify_isSameAdr();
          verify_isSameObject();
        }
      
      
      /** @test determine the address of an referred entity,
       *        possibly _unpacking_ a pointer (using its content)
       */
      void
      verify_getAdr()
        {
          CHECK (getAdr (this) == this);
          CHECK (getAdr (*this) == this);
          
          CStr aloof[2] = {"reality", "check"};
          CHECK (getAdr (aloof)     == &aloof);
          CHECK (getAdr (&aloof[0]) == &aloof);
          CHECK (getAdr (&aloof[1]) == aloof+1);
          CHECK (getAdr ( aloof[0]) == aloof[0]);
          CHECK (getAdr ( aloof[1]) == aloof[1]);
        }
      
      /** @test generate a numeric ID based on the memory address. */
      void
      verify_addrID()
        {
          uint ui[2] = {2,3};
          CHECK (addrID (ui[1]) == addrID (ui[0]) + sizeof(uint));
          
          uint* up{ui+1};
          CHECK (addrID (ui[1]) == addrID (up));
        }
      
      
      /** dummy entity to compare */
      struct Boo
        {
          short moo;
          string woo;
          
          Boo()
            : moo(rani(1000))
            , woo{toString(moo-1)}
            { }
        };
      
      struct SuBoo : Boo
        {
          using Boo::Boo;
          size_t poo{addrID (this)};
        };
      
      static Boo* asBoo (void* mem) { return static_cast<Boo*> (mem); }
      
      
      /** @test determine identity of referred arguments based on their
       *        memory location; pointers are unpacked, referring to the pointee.
       */
      void
      verify_isSameAdr()
        {
          Boo boo;
          Boo booo;
          Boo* boop = &boo;
          Boo const* const beep = boop;
          CHECK (boo.moo != booo.moo);
          CHECK (boo.moo == boop->moo);
          CHECK (    isSameAdr (boop,  beep));
          CHECK (not isSameAdr (&boop,&beep));
          CHECK (    isSameAdr (boo,   beep));
          CHECK (    isSameAdr (*beep, boo ));
          CHECK (not isSameAdr (*beep, booo));
          CHECK (    isSameAdr (boo,  boo.moo));
          CHECK (    isSameAdr (boo, &boo.moo));
          CHECK (not isSameAdr (boo.moo, booo));
          CHECK (    isSameAdr (booo, asBoo(&booo.moo)));
          CHECK (not isSameAdr (booo, asBoo(&booo.woo)));
          
          // handles also void*
          const void* voo = boop;
          CHECK (    isSameAdr (voo, boo));
          CHECK (    isSameAdr (voo, boop));
          CHECK (not isSameAdr (voo, booo));
          CHECK (    isSameAdr (voo, asBoo(&boo.moo)));
          CHECK (not isSameAdr (voo, asBoo(&booo.moo)));
          CHECK (not isSameAdr (voo, asBoo(&boo.woo)));
          
          // RValue taken by ref
          Boo&& roo = move(boo);
          CHECK (    isSameAdr (roo, boo));
          CHECK (    isSameAdr (voo, roo));
          CHECK (not isSameAdr (voo, Boo{roo}));
          
          // type information disregarded
          SuBoo* suBoo = static_cast<SuBoo*>(&boo);
          CHECK (    isSameAdr (boo, suBoo));
          CHECK (    isSameAdr (boo, suBoo->moo));
          CHECK (    isSameAdr (voo, suBoo->moo));
          CHECK (not isSameAdr (voo, suBoo->poo));
          
          SuBoo sudo{*suBoo};
          CHECK (not isSameAdr (sudo, boo));
          CHECK (not isSameAdr (sudo, suBoo));
          suBoo = &sudo;
          CHECK (    isSameAdr (sudo.poo, suBoo->poo));
        }
      
      /** @test determine strictly the identity of referred entities as given. */
      void
      verify_isSameObject()
        {
          Boo boo;
          Boo booo;
          Boo* boop = &boo;
          Boo* woop = boop;
          Boo& foop = *woop;
          CHECK (    isSameObject (boo,  boo ));
          CHECK (    isSameObject (booo, booo));
          CHECK (not isSameObject (boo,  booo));
          CHECK (not isSameObject (booo, boo ));
          // pointers count as »objects« and are not dereferenced
          CHECK (not isSameObject (boop, woop));
          CHECK (not isSameObject (boop, booo));
          CHECK (not isSameObject (boop, boo ));
          CHECK (not isSameObject (booo, woop));
          CHECK (not isSameObject (boo , woop));
          CHECK (    isSameObject (boo , foop));
          CHECK (    isSameObject (foop, boo ));
          CHECK (not isSameObject (foop, boop));
          CHECK (not isSameObject (foop, woop));
          CHECK (not isSameObject (foop, &boo ));
          CHECK (    isSameObject (foop, *boop));
          CHECK (    isSameObject (*boop, foop));
          
          // RValue taken by ref
          Boo&& roo = move(boo);
          CHECK (    isSameObject (roo, boo));
          CHECK (not isSameObject (boo, Boo{roo}));
          
          // type information disregarded
          SuBoo* suBoo = static_cast<SuBoo*>(&boo);
          CHECK (    isSameObject (boo, *suBoo));
          CHECK (    isSameObject (boo, suBoo->moo));
          CHECK (not isSameObject (boo, suBoo->woo));
          
          // void* is treated as a distinct object
          void* voo = boop;
          CHECK (not isSameObject (voo , boop));
          CHECK (not isSameObject (voo ,  boo));
          CHECK (not isSameObject (voo , &boo));     // ...not getting anywhere...
          CHECK (not isSameObject (voo , asBoo(&boo)));
          CHECK (not isSameObject (boo , asBoo(&boo)));
        }
    };
  
  
  LAUNCHER (UtilIdentity_test, "unit common");
  
}} // namespace util::test

