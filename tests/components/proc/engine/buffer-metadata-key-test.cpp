/*
  BufferMetadataKey(Test)  -  calculation of (internal) buffer metadata type keys

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


#include "lib/error.hpp"
#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
//#include "lib/util-foreach.hpp"
#include "lib/util.hpp"
//#include "proc/play/diagnostic-output-slot.hpp"
//#include "proc/engine/testframe.hpp"
//#include "proc/engine/diagnostic-buffer-provider.hpp"
#include "proc/engine/buffer-metadata.hpp"
//#include "proc/engine/buffhandle.hpp"
//#include "proc/engine/bufftable.hpp"

//#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>
#include <cstdlib>
#include <cstring>
#include <limits>
//#include <iostream>

//using boost::format;
//using std::string;
//using std::cout;
//using util::for_each;
using boost::scoped_ptr;
using util::isnil;
using util::isSameObject;


namespace engine{
namespace metadata{
namespace test  {
  

//  using lib::AllocationCluster;
//  using mobject::session::PEffect;
//  using ::engine::BuffHandle;
  using lumiera::error::LUMIERA_ERROR_INVALID;
  using lumiera::error::LUMIERA_ERROR_LIFECYCLE;
  
  
  namespace { // Test fixture
    
    const size_t TEST_MAX_SIZE = 1024 * 1024;
    
    const size_t SIZE_A = 1 + rand() % TEST_MAX_SIZE;
    const size_t SIZE_B = 1 + rand() % TEST_MAX_SIZE;
    
//  const size_t CHAR_MAX = 1;//std::numeric_limits<char>::max();
//  const HashVal JUST_SOMETHING = 123;
//  const void* const  SOME_POINTER = &JUST_SOMETHING;
//  const uint TEST_SIZE = 1024*1024;
//  const uint TEST_ELMS = 20;
    
    /** 
     * Test Mock to verify the attachment of objects to the buffer.
     * An instance of this class overwrites the occupied storage
     * with an ascending sequence of numbers on construction,
     * and clears the memory area on destruction.
     * 
     * This allows to verify that an instance of this class
     * has actually been placed into the buffer, and will be
     * cleaned up properly
     */
    template<size_t siz>
    struct PlacedNumbers
      {
        typedef char Pattern[siz];
        
        Pattern pattern_;
        
        PlacedNumbers()
          {
            for (size_t i=0; i<siz; ++i)
              pattern_[i] = i % CHAR_MAX;
          }
        
       ~PlacedNumbers()
          {
            for (size_t i=0; i<siz; ++i)
              pattern_[i] = 0;
          }
        
        
        /* === diagnostics === */
        
        static bool
        verifyFilled (const void* buff)
          {
            REQUIRE (buff);
            const Pattern& patt = *reinterpret_cast<const Pattern*> (buff);
            
            for (size_t i=0; i<siz; ++i)
              if (patt[i] != char(i % CHAR_MAX))
                return false;
            
            return true;
          }
        
        static bool
        verifyCleared (const void* buff)
          {
            REQUIRE (buff);
            const Pattern& patt = *reinterpret_cast<const Pattern*> (buff);
            
            for (size_t i=0; i<siz; ++i)
              if (patt[i])
                return false;
            
            return true;
          }
        
      };
    
    
  }
  
  
  /***********************************************************************
   * @test verify calculation and relations of Buffer metadata type keys.
   *       These are used internally within the standard implementation
   *       of BufferProvider to keep track of various kinds of buffers,
   *       to provide a service for attaching metadata, e.g. a state flag.
   *       These metadata key entries are based on chained hash values,
   *       thus forming sort-of a "type" hierarchy.
   *       - the actual BufferProvider instance-ID is the top level
   *       - second level is the size of the buffer required
   *       - optionally, custom ctor/dtor functions can be registered
   *       - and, also optionally, the implementation might attach an type-ID
   */
  class BufferMetadataKey_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          CHECK (ensure_proper_fixture());
          buildSimpleKeys();
          verifyChainedHashes();
          verifyTypeHandler<500>();
          verifyTypeSpecialisation();
        }
      
      
      bool
      ensure_proper_fixture() 
        {
          return (SIZE_A != SIZE_B);
        }
      
      
      void
      buildSimpleKeys()
        {
          HashVal family(123);
          Key k1(family, SIZE_A);
          Key k12(k1, SIZE_B);
          Key k123(k12, LocalKey(56));
          
          CHECK (HashVal (k1));
          CHECK (HashVal (k12));
          CHECK (HashVal (k123));
        }
      
      
      void
      verifyChainedHashes()
        {
          HashVal family(123);
          HashVal otherFamily(456);
          
          Key k1(family, SIZE_A);
          Key k1o(otherFamily, SIZE_A);
          CHECK (HashVal(k1) != HashVal(k1o));
          
          // hash is reproducible
          CHECK (HashVal(k1) == HashVal(Key(family, SIZE_A)));
          
          // differentiate on buffer size
          Key k12(k1, SIZE_B);
          Key k121(k12, SIZE_A);
          Key k2(family, SIZE_B);
          
          CHECK (HashVal(k1) != HashVal(k121));
          CHECK (HashVal(k12) != HashVal(k2));
          
          // so the specialisation path really matters, but this is reproducible...
          CHECK (HashVal(k121) == HashVal(Key(Key(Key(family,SIZE_A),SIZE_B),SIZE_A)));
        }
      
      
      template<size_t SIZ>
      void
      verifyTypeHandler()
        {
          char buff[SIZ];
          memset (buff, '\0', SIZ);
          
          typedef PlacedNumbers<SIZ> Pattern;
          
          TypeHandler attachPattern = TypeHandler::create<Pattern>();
          
          CHECK (attachPattern.isValid());
          CHECK (0 != hash_value(attachPattern));
          
          CHECK (Pattern::verifyCleared (buff));
          attachPattern.createAttached (buff);          // invoke the ctor-functor to place an instance of PlacedNumbers
          CHECK (Pattern::verifyFilled  (buff));
          attachPattern.destroyAttached (buff);         // invoke the dtor-functor to clear the attached instance
          CHECK (Pattern::verifyCleared (buff));
        }
      
      
      void
      verifyTypeSpecialisation()
        {
          HashVal family(123);
          Key kb (family, SIZE_A);
          
          TypeHandler noHandler;
          
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (BufferMetadataKey_test, "unit player");
  
  
  
}}} // namespace engine::metadata::test
