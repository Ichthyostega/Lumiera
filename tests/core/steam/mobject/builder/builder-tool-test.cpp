/*
  BuilderTool(Test)  -  specialised visitor used within the builder for processing Placements

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file builder-tool-test.cpp
 ** unit test \ref BuilderTool_test
 */


#include "lib/test/run.hpp"

#include "steam/mobject/builder/buildertool.hpp"
#include "steam/mobject/placement.hpp"
#include "steam/asset/category.hpp"
#include "steam/asset/media.hpp"
#include "steam/mobject/session/clip.hpp"
#include "steam/mobject/test-dummy-mobject.hpp"
#include "vault/media-access-mock.hpp"
#include "lib/depend-inject.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

using std::string;



namespace steam {
namespace mobject {
namespace builder {
namespace test    {
      
      using session::Clip;
      using session::AbstractMO;
      using namespace mobject::test;
  
      using MediaAccessMock = lib::DependInject<vault::MediaAccessFacade>
                                    ::Local<vault::test::MediaAccessMock>;
      
      
      
      /**
       * BuilderTool implementation for checking the invocation of the correct
       * \c treat() function and for accessing the original Placement from 
       * within this invocation. It is declared to be applicable to Clip
       * and DummyMO objects (wrapped into any acceptable shared-ptr).
       * Intentionally, we omit to declare it applicable to TestSubMO2 instances.
       * In reality this would be a case of misconfiguration, because TestSubMO2
       * is defined to be processable and consequently has an \apply() function,
       * which, due to this omission can't find a dispatcher entry when invoked,
       * so it will call the \c onUnknown(Buildable&) instead
       */
      class TestTool 
        : public Applicable<TestTool, Types<Clip, DummyMO>::List>
        {
        public:
          string log_;
          
          void treat (Clip& c)    
            { 
              Placement<Clip>& pC = getPlacement<Clip>();
              cout << "Clip on media : "<< pC->getMedia() <<"\n";
              CHECK (pC->operator==(c));
              log_ = string (pC);
            }
          void treat (AbstractMO&)
            {
              cout << "treat (AbstractMO&);\n";
              log_ = string (getPlacement<MObject>());
            }
          void onUnknown (Buildable&)
            { 
              cout << "catch-all-function called...\n";
              log_ = string (getPlacement<MObject>());
            }
        };
      
      
      
      
      
      
      /*********************************************************************************//**
       * @test the generic visitor pattern specialised for treating MObjects in the builder.
       * Besides using existing MObject types (at the moment session::Clip),
       * we create a yet-unknown new MObject subclass. When passing such to any
       * BuilderTool subclass, the compiler enforces the definition  of a catch-all
       * function, which is called, when there is no other applicable \c treat(MO&)
       * function. Note further, within the specific  treat-functions we get direct
       * references, without interfering with  Placements and memory management.
       * But from within the \c treat() function, we may access the wrapper object
       * (i.e. shared_ptr, or lumiera::P or Placement) used when invoking the 
       * BuilderTool by using the protected interface on BuilderTool.
       * 
       * @see VisitingTool_test for checking general visitor functionality
       */
      class BuilderTool_test : public Test
        {
          virtual void
          run(Arg)
            {
              MediaAccessMock useMockMedia;
              
              
              TestTool t1;
              BuilderTool& tool = t1;
                                
              Placement<Clip> clip = asset::Media::create("test-1", asset::VIDEO)->createClip();
              TestPlacement<> test1(*new TestSubMO1);
              TestPlacement<> test2(*new TestSubMO2);
              
              
              cout << "apply (tool, clip);\n";
              apply (tool, clip);
              INFO (test, "got Wrapper = %s", t1.log_.c_str());
              CHECK (t1.log_ == string(clip));
              
              cout << "apply (tool, test1);\n"; 
              apply (tool, test1);
              INFO (test, "got Wrapper = %s", t1.log_.c_str());
              CHECK (t1.log_ == string(test1));
              
              cout << "apply (tool, test2);\n"; 
              apply (tool, test2);
              INFO (test, "got Wrapper = %s", t1.log_.c_str());
              CHECK (t1.log_ == string(test2));
            } 
        };
      
      
      /** Register this test class... */
      LAUNCHER (BuilderTool_test, "unit builder");
      
      
      
}}}} // namespace steam::mobject::builder::test
