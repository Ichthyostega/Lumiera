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
#include "proc/asset/category.hpp"
#include "proc/asset/media.hpp"
#include "proc/mobject/session/clip.hpp"

#include "proc/mobject/explicitplacement.hpp"   //////////TODO


#include <iostream>
using std::string;
using std::cout;


namespace mobject
  {
  namespace builder
    {
    namespace test
      {
      
      using session::Clip;
      using session::AbstractMO;

      
      class DummyMO : public AbstractMO
        {
        public:
          DummyMO() { };
          virtual bool isValid()  const { return true;}
//          DEFINE_PROCESSABLE_BY (BuilderTool);
          
          static void killDummy (AbstractMO* dum) { delete (DummyMO*)dum; }
        };
      
      
  //////////////////////////////////////////////////////TODO: wip-wip
      
      class DummyPlacement : public Placement<AbstractMO>
        {
        public:
          DummyPlacement() 
            : Placement<AbstractMO>::Placement(*new DummyMO, &DummyMO::killDummy)
            { }
        };
  //////////////////////////////////////////////////////TODO: wip-wip
      
      

      class TestTool 
        : public Applicable<TestTool, Types<Placement<Clip>, Placement<AbstractMO> >::List>
        {
        public:
          void treat (Placement<Clip>& pC)   { cout << "media is: "<< str(pC->getMedia()) <<"\n"; }
          void treat (Placement<AbstractMO>&){ cout << "unspecific MO.\n"; }
          void onUnknown (Buildable&){ cout << "catch-all-function called.\n"; }
        };


      
      
      /*******************************************************************
       * @test the generic visitor pattern specialized for treating
       *       MObjects in the builder. Because the generic visitor 
       *       implementation is already covered by
       *       \link VisitingTool_test, it is sufficient to test
       *       the specialisation to the builder.
       * \par
       * Besides using existing MObject types (at the moment session::Clip),
       * we create inline a yet-unknown new MObject subclass. When passing
       * such to any BuilderTool subclass, the compiler enforces the definition
       * of a catch-all function, which is called, when there is no other
       * applicable \c treat(MO&) function. Note further, whithin the specific
       * treat-functions we get direct references, whithout interfering with
       * Placements and memory management. (Is this a good idea? it allows
       * client code to bypass the Placement (Handle). At least, I think
       * it is not really a problem...) 
       */
      class BuilderTool_test : public Test
        {
          virtual void run(Arg arg) 
            {
                          
              TestTool t1;
              BuilderTool& tool (t1);
                                
              DummyPlacement dumm;
              PMO clip = asset::Media::create("test-1", asset::VIDEO)->createClip();

              clip.apply (tool);
              dumm.apply (tool);
            } 
        };
      
      
      /** Register this test class... */
      LAUNCHER (BuilderTool_test, "unit builder");
      
      
      
    } // namespace test
    
  } // namespace builder
  
  //////////////////////////////////////////////////////TODO: wip-wip
  template<>
  ExplicitPlacement
  Placement<session::AbstractMO>::resolve ()  const 
    { 
      UNIMPLEMENTED ("just a test");
    }
  //////////////////////////////////////////////////////TODO: wip-wip


} // namespace mobject
