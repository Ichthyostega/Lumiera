/*
  StreamTypeBasics(Test)  -  check the fundamentals of stream type information

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file stream-type-basics-test.cpp
 ** unit test \ref StreamTypeBasics_test
 */


#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "steam/control/stypemanager.hpp"
#include "teststreamtypes.hpp"

using ::test::Test;
using util::isnil;


namespace steam {
namespace test_format {
  
  using control::STypeManager;
  typedef StreamType const& SType;
  typedef StreamType::ImplFacade const& ImplType;
  
  
  /***************************************************************//**
   * @test check the basic workings of the stream type handling.
   *       create some stream implementation data, build a 
   *       StreamType::ImplFacade from this, and derive a prototype
   *       and a full StreamType based on this information.
   */
  class StreamTypeBasics_test : public Test
    {
      virtual void
      run (Arg)
        {
          ImplType iType = buildImplType ();
          basicImplTypeProperties (iType);
          
          SType type = extend2fullType (iType);
          basicStreamTypeProperties (type, iType);
        }
      
      ImplType
      buildImplType ()
        {
          STypeManager& typeManager = STypeManager::instance();
      
          gavl_video_format_t rawType = test_createRawType();
          ImplType iTy (typeManager.getImpl (GAVL, rawType));
          
          UNIMPLEMENTED ("at least preliminary implementation of the MediaImplLib interface for lib GAVL");
          
          TODO ("how to do a simple consistency check on the returned ImplFacade? can we re-create the GAVL frame type?");
          CHECK (GAVL==iTy.libraryID);
          return iTy;
        }
      
      void
      basicImplTypeProperties (ImplType refType)
        {
          ImplType iTy2 = test_createImplType ();
          CHECK (iTy2==refType);
          CHECK (refType==iTy2);
          TODO ("add equality comparable concept to the ImplType class");
          
          CHECK (StreamType::VIDEO==refType.getKind());
          UNIMPLEMENTED ("get a lib descriptor"); 
          UNIMPLEMENTED ("check the lib of the type"); 
          UNIMPLEMENTED ("compare two types"); 
        }
      
      SType
      extend2fullType (ImplType iTy)
        {
          return STypeManager::instance().getType(iTy);
        }
      
      void
      basicStreamTypeProperties (SType type, ImplType iTy)
        {
          CHECK (type.implType);
          CHECK (iTy==(*type.implType));  /////////////TODO: really by ptr???
          CHECK (&iTy==type.implType);   // actually using the same object (in the registry)
          
          CHECK (!isnil (type.prototype.id));
          CHECK (StreamType::VIDEO==type.prototype.kind);
          CHECK (StreamType::VIDEO==type.implType->getKind());
          
          CHECK (type.implType->canConvert(iTy));  // of course... they are actually the same
          CHECK (iTy.canConvert(type));           // because it's based on the same impl type
          
          CHECK (StreamType::RAW==type.intentionTag);
        }
    };
  
  LAUNCHER (StreamTypeBasics_test, "unit common");
  
  
}} // namespace steam::test_format

