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

#include "proc/asset.hpp"
#include "common/p.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/explicitplacement.hpp"   //////////TODO

#include <boost/variant.hpp>
#include <iostream>
using std::string;
using std::cout;


namespace mobject
  {
  
  DEFINE_SPECIALIZED_PLACEMENT (session::AbstractMO);
  
  
  namespace builder
    {
    namespace test
      {
      
      using session::Clip;
      using session::AbstractMO;

/////////////////////////////////////////////////////TODO: move to buildertool.hpp

      using lumiera::P;

// Problem
/*
      - brauche Laufzeittyp des Zielobjektes
      - an wen wird die Dispatcher-Tabelle gebunden?
        - falls an den Wrapper: Gefahr, zur Laufzeit nicht die Tabelle zu bekommen, in die das Trampolin registriert wurde
        - falls an das Zielobjekt: wie gebe ich Referenz und konkreten Typ des Wrappers an den Funktionsaufruf im Tool?
      - vieleicht einen allgemeinen Argument-Adapter nutzen?
      - Zielobjekt oder Wrapper<Zielobjekt> als Argumenttyp? 
*/
    struct Nothing {};
      
    /** 
     * helper to treat various sorts of smart-ptrs uniformly.
     * Implemented as a variant-type value object, it is preconfigured
     * with the possible hierarchy-base classes used within this application.
     * Thus, when passing in an arbitrary smart-ptr, the best fitting smart-ptr
     * type pointing to the corresponding base class is selected for internal storage.
     * Later on, stored values can be retrieved either utilitzing static or dynamic casts;
     * error reporting is similar to the bahaviour of dynamic_cast<T>: when retrieving
     * a pointer, in case of mismatch NULL is returned.
     */
    class WrapperPtr         // NONCOPYABLE!!
      {
        
        template<typename X>
        struct accessor;
        
        template<template<class> class WRA, class TAR>
        struct accessor< WRA<TAR> > 
          : public boost::static_visitor<WRA<TAR>*>
          {
            template<typename X>    WRA<TAR>* operator() (X&)                { return 0; }
            template<typename BASE> WRA<TAR>* operator() (WRA<BASE>& stored) { return static_cast<WRA<TAR>*> (stored); }
          };

      
      private:
        /** storage: buffer holding either and "empty" marker,
         *  or one of the configured pointer to wrapper types */ 
        boost::variant<Nothing, Placement<MObject>*, P<asset::Asset>*> holder_;
        
      public:
        void reset () { holder_ = Nothing(); }
        
        template<typename WRA>
        WrapperPtr&
        operator= (WRA* src)      ///< store a ptr to the given wrapper, after casting to base
          {
            if (src) holder_ = src;
            else     holder_ = Nothing();
            return *this;
          }
        
        template<typename WRA>
        WRA*
        get ()                    ///< retrieve ptr and try to downcast to type WRA   
          {
            accessor<WRA> acc;
            WRA* res = boost::apply_visitor(acc, this->holder_);
            return res;
          }
      };
    
    
    class BuTuul
      : public lumiera::visitor::Tool<void, InvokeCatchAllFunction> 
      {
        WrapperPtr currentWrapper_;
        
      public:
        
      template<template<class> class WRA, class TAR>
      void rememberWrapper (WRA<TAR>* argument)
        {
          currentWrapper_ = argument;
        }
      
      void forgetWrapper ()
        {
          currentWrapper_.reset();
        }
      
      
      template<class TAR>
      Placement<TAR>&
      getPlacement ()
        {
          Placement<TAR>* pPlacement = currentWrapper_.get<Placement<TAR> >(); 
          return *pPlacement;
        }
      ExplicitPlacement
      getExplicitPlacement ()
        {
          return getPlacement<MObject>().resolve();
        }
      template<class TAR>
      P<TAR>
      getPtr ()
        {
          P<TAR>* pP = currentWrapper_.get<P<TAR> >(); 
          return *pP;
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
          
          static void killDummy (MObject* dum) { delete (DummyMO*)dum; }
          
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
      
      
      template<typename WRA>
      inline BDable::ReturnType
      apply (BuTuul& tool, WRA& wrappedTargetObj)
      {
        WRA* ptr = &wrappedTargetObj;
        (*ptr)->isValid();
        tool.rememberWrapper(ptr);
        wrappedTargetObj->apply (tool);
        tool.forgetWrapper();
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
              Placement<Clip>& pC = getPlacement<Clip>();
              cout << "media is: "<< str(pC->getMedia()) <<"\n"; 
            }
          void treat (AbstractMO&)
            {
              Placement<AbstractMO>& placement = getPlacement<AbstractMO>();
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
              Placement<AbstractMO>& dummy(dumm);
              
              Placement<Clip> clip = asset::Media::create("test-1", asset::VIDEO)->createClip();
              

              apply (tool, clip);
              cout << "Placement(adr) " << &dumm <<"\n"; 
              apply (tool, dummy);
            } 
        };
      
      
      /** Register this test class... */
      LAUNCHER (BuilderTool_test, "unit builder");
      
      
      
    } // namespace test
    
  } // namespace builder
  
  //////////////////////////////////////////////////////TODO: wip-wip
/*  template<>
  ExplicitPlacement
  Placement<session::AbstractMO>::resolve ()  const 
    { 
      UNIMPLEMENTED ("just a test");
    }
*/  //////////////////////////////////////////////////////TODO: wip-wip


} // namespace mobject
