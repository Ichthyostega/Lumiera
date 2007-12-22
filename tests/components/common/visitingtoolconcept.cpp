/*
  VisitingTool(Concept)  -  working out our own Visitor library implementation
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file visitingtoolconept.cpp
 ** While laying the foundations for EDL and Builder, Ichthyo came accross 
 ** the necessity to create a custom implementation of the Visitor Pattern
 ** optimally suited for Cinelerra's needs. This implementation file was 
 ** used for the draft and is self-contained. The final solution was then
 ** extracted as library implementation to visitor.hpp 
 **
 ** Basic considerations
 ** <ul><li>cyclic dependencies should be avoided or at least restricted
 **         to some library related place</li>
 **     <li>Visitor is about <i>double dispatch</i>, thus we can't avoid
 **         using some table lookup implementation, and we can't avoid using
 **         some of the cooperating classes vtables. Besides that, the 
 **         implementation should not be too wastefull</li>
 **     <li>individual Visiting Tool implementation classes should be able
 **         to opt in or opt out on implementing function treating some of
 **         the visitable subclasses.</li>
 **     <li>there should be a safe fallback mechanism backed by the
 **         visitable object's hierarchy relations. If some new class declares
 **         to be visitable, existing Visiting Tools not treating this new
 **         visitable type should fall back to the next best match in the
 **         hierarchy, not to some deaf base class</li>
 ** </ul>
 **
 ** @see visitor.hpp the final lib implementation
 ** @see visitingtooltest.cpp test cases using our lib implementation
 ** @see BuilderTool one especially important instantiiation
 **
 */


#include "common/test/run.hpp"
//#include "common/factory.hpp"
//#include "common/util.hpp"

#include <boost/format.hpp>
#include <iostream>

using boost::format;
using std::string;
using std::cout;


namespace cinelerra
  {
  namespace visitor
    {
    // ================================================================== Library ====
  
    /** Marker interface "visiting tool".
     */
    template<typename RET>
    class Tool
      {
      public:
        typedef RET ReturnType;
        
        virtual ~Tool ()  { };  ///< use RTTI for all visiting tools
      };
    
      
    template<class TOOL>
    class ToolRegistry
      {
        
      };
    
    
    template<class TOOLTYPE>
    class ToolReg
      {
        //getTag()
        void regCombination(Invoker ii)
          {
            ////OOOOhhh, da hab ich wiiiieder das gleiche Problem: wie krieg ich die Typinfo TAR????
          }
      };
      
    // alternativ ("halb-zyklisch")
    // 
    template<class TAR, class TOOLTYPE>
    class ReToo
      {
        ReToo()
          {
            //expliziten Eintrag machen für TAR und TOOLTYPE
          }
      };
      
    template<class TAR>
    class DisTab : ReToo<TAR, ToolTyp1>,
                   ReToo<TAR, ToolTyp2>
      {
      };

    // der konkrete Dispatch instantiiert diese Klasse
    // Voraussetzung: alle Tools hängen nur per Name von ihren Targets ab

      
    template<class TAR, class TOOL>
    class Dispatcher
      {
        // Ctor: for_each(ToolRegistry, call regCombination() )
      public:
        static TOOL::ReturnType call (TOOL& tool, TAR& target)
          {
            
          }
      };
    
      
    /** Marker interface "visitable object".
     */
    template 
      < class TOOL = Tool<void> 
      >
    class Visitable
      {
      public:
        /** to be defined by the DEFINE_PROCESSABLE_BY macro
         *  in all classes wanting to be treated by some tool */
        typedef TOOL::ReturnType ReturnType;
        virtual ReturnType apply (TOOL&) = 0;
        
      protected:
        virtual ~Visitable () { };
        
        /** @internal used by the DEFINE_VISITABLE macro.
         *            Dispatches to the actual operation on the 
         *            "visiting tool" (acyclic visitor implementation)
         */
        template <class TAR>
        static ReturnType dispatchOp(TAR& target, TOOL& tool)
          {
            return Dispatcher<TAR,TOOL>::call (tool, target);
          }
      };
      

/** mark a Visitable subclass as actually treatable
 *  by some "visiting tool". Defines the apply-function,
 *  which is the actual access point to invoke the visiting
 */
#define DEFINE_PROCESSABLE_BY(TOOL) \
        virtual ReturnType  apply (TOOL& tool) \
        { return dispatchOp (*this, tool); }
    
    
    

    
    // ================================================================== Library ====
    
    namespace test
      {
      
      
      class HomoSapiens : public Visitable<>
        {
        public:
          DEFINE_PROCESSABLE_BY (Tool);
        };
      
      class Boss : public HomoSapiens
        {
        public:
          DEFINE_PROCESSABLE_BY (Tool);
        };
      
      class BigBoss : public Boss
        {
        public:
          DEFINE_PROCESSABLE_BY (Tool);
        };
        
      class Leader : public Boss
        {
        };
        
      class Visionary : public Leader
        {
        };

        
      class VerboseVisitor
        : public Tool
        {
        protected:
          void talk_to (string guy)
            {
              cout << format ("Hello %s, nice to meet you...\n") % guy;
            }
        };
        
      class Babbler
        : public VerboseVisitor,
          public Applicable<Boss>,
          public Applicable<BigBoss>
        {
        public:
          void treat (Boss&)    { talk_to("Boss"); }
          void treat (BigBoss&) { talk_to("big Boss"); }
        };

        
        
        
      
      
      
      /*************************************************************************
       * @test build and run some common cases for developing and verifying
       *       ichthyo's implementation concept for the Visitor Pattern.
       *       Defines a hierarchy of test classes to check the following cases
       *       <ul><li>calling the correct visiting tool specialized function
       *               for given concrete hierarchy classes</li>
       *           <li>visiting tool not declaring to visit some class</li>
       *           <li>newly added class causes the catch-all to be invoked
       *               when visited by known visitor</li>
       *       </ul>
       */
      class VisitingTool_concept : public Test
        {
          virtual void run(Arg arg) 
            {
              known_visitor_known_class();
              visitor_not_visiting_some_class();
            } 
          
          void known_visitor_known_class()
            {
              Boss x1;
              BigBoss x2;
              
              // masquerade as HomoSapiens...
              HomoSapiens& homo1 (x1);
              HomoSapiens& homo2 (x2);
              
              cout << "=== Babbler meets Boss and BigBoss ===\n";
              Babbler bab;
              Visitor& vista (bab);
              homo1.apply (vista);
              homo2.apply (vista);
            }
          
          void visitor_not_visiting_some_class()
            {
              HomoSapiens x1;
              Visionary x2;
              
              HomoSapiens& homo1 (x1);
              HomoSapiens& homo2 (x2);
              
              cout << "=== Babbler meets HomoSapiens and Visionary ===\n";
              Babbler bab;
              Visitor& vista (bab);
              homo1.apply (vista);  // doesn't visit HomoSapiens
              homo2.apply (vista); //  treats Visionary as Boss
            }
            
        };
      
      
      /** Register this test class... */
      LAUNCHER (VisitingTool_concept, "unit common");
      
      
      
    } // namespace test
    
  } // namespace visitor

} // namespace cinelerra
