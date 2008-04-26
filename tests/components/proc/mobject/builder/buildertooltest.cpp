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

#include <boost/any.hpp>
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

/////////////////////////////////////////////////////TODO: move to buildertool.hpp
      using boost::any;
      using boost::any_cast;

// Problem
/*
      - brauche Laufzeittyp des Zielobjektes
      - an wen wird die Dispatcher-Tabelle gebunden?
        - falls an den Wrapper: Gefahr, zur Laufzeit nicht die Tabelle zu bekommen, in die das Trampolin registriert wurde
        - falls an das Zielobjekt: wie gebe ich Referenz und konkreten Typ des Wrappers an den Funktionsaufruf im Tool?
      - vieleicht einen allgemeinen Argument-Adapter nutzen?
      - Zielobjekt oder Wrapper<Zielobjekt> als Argumenttyp? 
*/
      
    class BuTuul
      : public lumiera::visitor::Tool<void, InvokeCatchAllFunction> 
      {
        any currentArgument_;
        
      public:
        
      void rememberWrapper (any argument)
        {
          currentArgument_ = argument;
        }
      
      template<typename WRA>
      WRA& getCurrentArgumentWrapper ()
        {
          WRA* argument = any_cast<WRA*> (currentArgument_);
          ASSERT (argument);
          return *argument;
        }
      };
      

    
    
    template
      < class TOOLImpl,  // concrete BuilderTool implementation
        class TYPELIST  //  list of all concrete Buildables to be treated
      >
    class Appli
      : public lumiera::visitor::Applicable<TOOLImpl, TYPELIST, BuTuul>
      { }
      ;
      
    using lumiera::typelist::Types;   // convienience for the users of "Applicable"

    class BDable : public lumiera::visitor::Visitable<BuTuul>
      {
        
      };
/////////////////////////////////////////////////////TODO: move to buildertool.hpp
      
      class DummyMO : public AbstractMO, public BDable
        {
        public:
          DummyMO() { };
          virtual bool isValid()  const { return true;}
//          DEFINE_PROCESSABLE_BY (BuilderTool);
          
          static void killDummy (AbstractMO* dum) { delete (DummyMO*)dum; }
          
          virtual void
          apply (BuTuul& tool) 
            { 
              return BDable::dispatchOp (*this, tool); 
            }
          
        };
      
      class Clop : public Clip, public BDable
        {
          Clop (Clip& base) : Clip (base.clipDef_, base.mediaDef_) {}
          
          virtual void
          apply (BuTuul& tool) 
            { 
              return BDable::dispatchOp (*this, tool); 
            }
        };
      
      
      template<typename TAR>
      inline BDable::ReturnType
      apply (BuTuul& tool, TAR& wrappedTargetObj)
      {
        tool.rememberWrapper(any (&wrappedTargetObj));
        wrappedTargetObj->apply (tool);
      }
      
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
        : public Appli<TestTool, Types<Clip, AbstractMO>::List>
        {
        public:
          void treat (Clip& c)    
            { 
              Placement<Clip>& pC = getCurrentArgumentWrapper<Placement<Clip> >();
              cout << "media is: "<< str(pC->getMedia()) <<"\n"; 
            }
          void treat (AbstractMO&)
            {
              Placement<AbstractMO>& placement = getCurrentArgumentWrapper<DummyPlacement>();
              cout << "unspecific MO; Placement(adr) " << &placement <<"\n"; 
            }
          void onUnknown (Buildable&)
            { 
              cout << "catch-all-function called.\n"; 
            }
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
              BuTuul& tool (t1);
                                
              DummyPlacement dumm;
              Placement<Clip> clip = asset::Media::create("test-1", asset::VIDEO)->createClip();

              apply (tool, clip);
              cout << "Placement(adr) " << &dumm <<"\n"; 
              apply (tool, dumm);
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
