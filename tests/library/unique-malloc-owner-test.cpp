/*
  UniqueMallocOwner(Test)  -  Concept to dispatch according to the verbs of a DSL

  Copyright (C)         Lumiera.org
    2014,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "lib/test/test-helper.hpp"
#include "lib/unique-malloc-owner.hpp"
#include "lib/util.hpp"

#include <utility>
#include <cstring>
#include <string>

using std::string;
using util::isnil;


namespace lib {
namespace test{
  
  namespace {
    
    void*
    mallocMess (size_t siz)
    {
      return malloc(siz);
    }
  }
  
  
  
  
  
  
  
  
  /***************************************************************************//**
   * @test Verify automatic management of memory originally allocated by C malloc.
   *       This situation typically arises when some plain-C function returns
   *       results in a heap allocated buffer, requiring the client to care
   *       for proper clean-up. To avoid creating a liability, we wrap the
   *       buffer into a smart pointer, which acts as ownership token
   *       and abstracts the specifics of clean-up.
   *       
   *       This test places a given (or random) string into a heap malloced
   *       buffer, and then pushes the "hot potato" of ownership responsibility
   *       around; another function checks the content of the buffer and consumes
   *       the token as a side-effect. At the end, there should be no memory leak
   *       and the ownership token should be empty.
   *       
   * @see lib::UniqueMallocOwner
   * @see lib::meta::demangleCxx
   */
  class UniqueMallocOwner_test : public Test
    {
      
      using CharOwner = UniqueMallocOwner<char>;
      
      virtual void
      run (Arg args)
        {
          string probeString = isnil(args)? randStr(123) : args[0];
          auto hotPotato = place_into_malloced_buffer (probeString);
          
          CHECK (!isnil (hotPotato));
          verify_and_consume (std::move(hotPotato), probeString);
          CHECK (isnil (hotPotato));
        }
      
      
      CharOwner
      place_into_malloced_buffer (string probeString)
        {
          CharOwner hotPotato(mallocMess (1 + probeString.length()));
          std::strcpy(hotPotato.get(), probeString.c_str());
          return hotPotato;
        }
      
      
      void
      verify_and_consume (CharOwner hotPotato, string refString)
        {
           CHECK (refString == hotPotato.get());
           
        }  // note sideeffect: hotPotato goes out of scope here...
    };
  
  
  /** Register this test class... */
  LAUNCHER (UniqueMallocOwner_test, "unit common");
  
  
  
}} // namespace lib::test
