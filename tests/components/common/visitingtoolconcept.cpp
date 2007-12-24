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
 **         some of the cooperating classe's vtables. Besides that, the 
 **         implementation should not be too wastefull</li>
 **     <li>individual Visiting Tool implementation classes should be able
 **         to opt in or opt out on implementing functions treating some of
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
#include <vector>

using boost::format;
using std::string;
using std::cout;


namespace cinelerra
  {
  namespace visitor
    {
    // ================================================================== Library ====
  
    template<class TOOL>
    class Tag
    {
      size_t tagID;
      
      template<class TOOLImpl>
      struct TagTypeRef { static Tag tag; };
      
      static size_t lastRegisteredID;
      
    public:
      operator size_t()  const { return tagID; }
      
      template<class TOOLImpl>
      static Tag
      get(TOOLImpl* concreteTool=0)
        {
          Tag& t = TagTypeRef<const TOOLImpl>::tag;
          TODO ("concurrent access");
          if (!t)
            t = ++lastRegisteredID;
          return t;
        }
    };
    
    
    /** storage for the Tag registry for each concrete tool */
    template<class TOOL, class TOOLImpl>
    Tag<TOOL> Tag<TOOL>::TagTypeRef<TOOLImpl>::tag;
    
    template<class TOOL>
    size_t Tag<TOOL>::lastRegisteredID (0);



    
    /** Marker interface "visiting tool".
     */
    template<typename RET>
    class Tool
      {
      public:
        typedef RET ReturnType;
        typedef Tool<RET> ToolBase;
        
        virtual ~Tool ()  { };   ///< use RTTI for all visiting tools
        
        /** allows discovery of the concrete Tool type
         *  when dispatching a visitor call */
        virtual Tag<ToolBase> 
        getTag() 
          { 
            return Tag<ToolBase>::get(this); 
          }
      };
    
      


    /**
     * For each posible call entry point via some subclass of the visitable hierarchy,
     * we maintain a dispatcher table to keep track of all concrete tool implementations
     * able to recieve and process calls on objects of this subclass.
     */
    template<class TAR, class TOOL>
    class Dispatcher
      {
        typedef TOOL::ReturnType ReturnType;
        
        /** generator for Trampoline functions,
         *  used to dispatch calls down to the 
         *  right "treat"-Function on the correct
         *  concrete tool implementation class
         */
        template<class TOOLImpl>
        ReturnType 
        static callTrampoline (TAR& obj, TOOL tool)
          {
            // cast down to real implementation type
            REQUIRE (INSTANCEOF (TOOLImpl, &tool));  
            TOOLImpl& toolObj = static_cast<TOOLImpl&> (tool);
            
            // trigger (compile time) overload resolution
            // based on concrete type, and dispatch call.
            // Note this may cause obj to be upcasted.
            return toolObj.treat (obj);
          }
        
        typedef ReturnType (*Trampoline) (TAR&, TOOL& );

        
        /** VTable for storing the Trampoline pointers */
        std::vector<Trampoline> table_;


      public:
        ReturnType 
        forwardCall (TAR& target, TOOL& tool)
          {
            // get concrete type via tool's VTable 
            Tag<TOOL> index = tool.getTag();
            Trampoline func = this->table_[index];
            TODO ("Errorhandling");
            return (*func) (target, tool);
          }
        
        template<class TOOLImpl>
        static inline void 
        enroll()
          {
            TODO ("skip if already registered");
            TODO ("concurrency handling");
            Tag<TOOL> index = Tag<TOOL>::get<TOOLImpl>();
            Trampoline func = callTrampoline<TOOLImpl>;
            this->table_[index] = func;
          }
      };
      
      
    /** 
     * concrete visiting tool implementation has to inherit from this
     * class for each kind of calls it wants to get dispatched, 
     * Allowing us to record the type information.
     */
    template<class TAR, class TOOLImpl>
    class Applicable
      {
        typedef TOOLImpl::ToolBase Base;
        typedef TOOLImpl::ToolBase::ReturnType Ret;
        
        Applicable ()
          {
            Dispatcher<TAR,Base>::enroll<TOOLImpl>();
          }
        
      public:
        virtual Ret treat (TAR& target) = 0;
        
      };
      
    
    
    // entweder die "halb-zyklische" Lösung, wo sich das einzelne
    // Tool noch über den Dispatcher (als Vaterklasse) einklinken muß
    // - vorteil: kann volle Auflösung (Konstruktoren-Trick mit rückgeführtem Typparameter
    // - Nachteil: einzelnes visitable hängt von allen tools ab
    //
    // oder die TMP-Lösung, die auf der Applicable<TOOLTYPE, TAR> beruht und via Tabelle arbeitet.
    // - Vorteil: der notwendige Kontext ist auf natürliche Weise da
    // - Nachteil: kann die fehlenden Kombinationen nicht auflösen
    // Variante: generiert die Applicable's über Typlisten
    // Abhilfe: auch die anderen Fälle verlangen zu deklarieren!
    // zu untersuchen: könnte ich einen fallback-Dispatcher generieren?
    // klar ist: alle Versuche, die beiden konkreten Typkontexte zusammenzutransportieren,
    //           sind zum Scheitern verurteilt, weil der Definitions-Kontext hierfür nicht gegeben ist.
    //           Wäre er gegeben, dann würde das voll-zyklische Abhängigkeiten bedeuten!
    // Frage ist also: wie könnte ich die Generierung des fehlenden Kontextes für die fehlenden Kombinationen
    // antreiben? Einsprung natürlich über Interface/virt.Funktionen.
    // Idee: der Benutzer deklariert Fallback<Types<A,B,C>, ToolType >
      
      
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
