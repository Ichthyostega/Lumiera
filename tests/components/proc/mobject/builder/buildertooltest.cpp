/*
  BuilderTool(Test)  -  specialized visitor used within the builder for processing Placements
 
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

#include "proc/mobject/builder/buildertool.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/asset/category.hpp"
#include "proc/asset/media.hpp"
#include "proc/mobject/session/clip.hpp"
#include "common/util.hpp"

#include <iostream>

using util::cStr;
using std::string;
using std::cout;



namespace mobject {
  namespace builder {
    namespace test {
      
      using session::Clip;
      using session::AbstractMO;
      
      
      
      /** 
       *  Test MObject subclass, which, contrary to any real MObject,
       *  can be created directly without involving MObjectFactory.
       */
      class TestMO : public AbstractMO
        {
        public:
          TestMO() { };
          
          DEFINE_PROCESSABLE_BY (BuilderTool);
          
          virtual bool isValid()  const        { return true;}
          static void killDummy (MObject* dum) { delete (TestMO*)dum; }
        };
      
      /** 
       * Subclass-1 is \em not defined "processable",
       * thus will always be handled as TestMO...
       */
      class TestSubMO1 : public TestMO 
        { };
      
      /** 
       * Subclass-2 \em is defined "processable", 
       * but we omit the necessary "applicable" definition in TestTool,
       * resulting in an invocation of the error (catch-all) function...
       */
      class TestSubMO2 : public TestMO 
        { 
          DEFINE_PROCESSABLE_BY (BuilderTool);
        };
      
      
      template<class MO>
      class TestPlacement : public Placement<MO>
        {
        public:
          TestPlacement(TestMO& testObject) 
            : Placement<MO>::Placement(testObject, &TestMO::killDummy)
            { }
        };
      
      
      /**
       * BuilderTool implementation for checking the invokation of the correct
       * \c treat() function and for accessing the original Placement from 
       * within this invocation. It is declared to be applicable to Clip
       * and TestMO objects (wrapped into any acceptable shared-ptr).
       * Intentionally, we omit to declare it applicable to TestSubMO2 instances.
       * In reality this would be a case of misconfiguration, because TestSubMO2
       * is defined to be processable and consequently has an \apply() function,
       * which, due to this ommission can't find a dispatcher entry when invoked,
       * so it will call the \c onUnknown(Buildable&) instead
       */
      class TestTool 
        : public Applicable<TestTool, Types<Clip, TestMO>::List>
        {
        public:
          string log_;
          
          void treat (Clip& c)    
            { 
              Placement<Clip>& pC = getPlacement<Clip>();
              cout << "Clip on media : "<< str(pC->getMedia()) <<"\n";
              log_ = string (pC);
            }
          void treat (AbstractMO&)
            {
              cout << "treat (AbstractMO&);\n";
              log_ = string (getPlacement<MObject>());
            }
          void onUnknown (Buildable& bxx)
            { 
              cout << "catch-all-function called...\n";
              log_ = string (getPlacement<MObject>());
            }
        };
      
      
      
      
      
      
      /*************************************************************************************
       * @test the generic visitor pattern specialized for treating MObjects in the builder.
       * Besides using existing MObject types (at the moment session::Clip),
       * we create a yet-unknown new MObject subclass. When passing such to any
       * BuilderTool subclass, the compiler enforces the definition  of a catch-all
       * function, which is called, when there is no other applicable \c treat(MO&)
       * function. Note further, whithin the specific  treat-functions we get direct
       * references, whithout interfering with  Placements and memory management.
       * But from within the \c treat() function, we may access the wrapper object
       * (i.e. shared_ptr, or lumiera::P or Placement) used when invoking the 
       * BuilderTool by using the protected interface on BuilderTool.
       * 
       * @see VisitingTool_test for checking general visitor functionality
       */
      class BuilderTool_test : public Test
        {
          virtual void run(Arg arg) 
            {
                          
              TestTool t1;
              BuilderTool& tool = t1;
                                
              Placement<Clip> clip = asset::Media::create("test-1", asset::VIDEO)->createClip();
              TestPlacement<MObject> test1(*new TestSubMO1);
              TestPlacement<MObject> test2(*new TestSubMO2);
              

              cout << "apply (tool, clip);\n";
              apply (tool, clip);
              INFO (test, "got Wrapper = %s", t1.log_.c_str());
              ASSERT (t1.log_ == string(clip));
              
              cout << "apply (tool, test1);\n"; 
              apply (tool, test1);
              INFO (test, "got Wrapper = %s", t1.log_.c_str());
              ASSERT (t1.log_ == string(test1));
              
              cout << "apply (tool, test2);\n"; 
              apply (tool, test2);
              INFO (test, "got Wrapper = %s", t1.log_.c_str());
              ASSERT (t1.log_ == string(test2));
            } 
        };
      
      
      /** Register this test class... */
      LAUNCHER (BuilderTool_test, "unit builder");
      
      
      
    } // namespace test
    
  } // namespace builder
  
} // namespace mobject
