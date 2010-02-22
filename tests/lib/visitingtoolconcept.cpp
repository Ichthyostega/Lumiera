/*
  VisitingTool(Concept)  -  working out our own Visitor library implementation
 
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


/** @file visitingtoolconept.cpp
 ** While laying the foundations for Session and Builder, Ichthyo came across 
 ** the necessity to create a custom implementation of the Visitor Pattern
 ** optimally suited for Lumiera's needs. This implementation file was 
 ** used for the draft and is self-contained. The final solution was then
 ** extracted as library implementation to visitor.hpp 
 **
 ** Basic considerations
 ** <ul><li>cyclic dependencies should be avoided or at least restricted
 **         to some library related place. The responsibilities for
 **         user code should be as small as possible.</li>
 **     <li>Visitor is about <i>double dispatch</i>, thus we can't avoid
 **         using some table lookup implementation, and we can't avoid using
 **         some of the cooperating classes vtables. Besides that, the 
 **         implementation should not be too wasteful...</li>
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
 ** @see BuilderTool one especially important instantiation
 **
 */


#include "lib/test/run.hpp"
#include "lib/singleton.hpp"

#include <boost/format.hpp>
#include <iostream>
#include <vector>

using lib::Singleton;
using boost::format;
using std::string;
using std::cout;


namespace lumiera
  {
  namespace visitor_concept_draft
    {
    // ================================================================== Library ====
    
    

    template<class TOOL> class Tag;
    
    
    template<class TOOL, class TOOLImpl>
    struct TagTypeRegistry 
      {
        static Tag<TOOL> tag;
      };
      
      
    template<class TOOL>
    class Tag
    {
      size_t tagID;
      static size_t lastRegisteredID;
      
    public:
      Tag() : tagID(0) { }
      operator size_t()  const { return tagID; }
      
      
      template<class TOOLImpl>
      static Tag&
      get (TOOLImpl* const =0)  // param used to pass type info
        {
          // we have a race condition here...
          Tag& t = TagTypeRegistry<TOOL,TOOLImpl>::tag;
          if (!t)
            t.tagID = ++lastRegisteredID;
          return t;
        }
    
    };
    
    
    
    /** storage for the Tag registry for each concrete tool */
    template<class TOOL, class TOOLImpl>
    Tag<TOOL> TagTypeRegistry<TOOL,TOOLImpl>::tag; 

    template<class TOOL>
    size_t Tag<TOOL>::lastRegisteredID (0);

    
    
    
    
    
    /** Marker interface "visiting tool" */
    template<typename RET>
    class Tool
      {
      public:
        typedef RET ReturnType;
        typedef Tool<RET> ToolBase; ///< for templating the Tag and Dispatcher
        
        virtual ~Tool ()  { };   ///< use RTTI for all visiting tools
        
        /** allows discovery of the concrete Tool type when dispatching a
         *  visitor call. Can be implemented by inheriting from ToolType */
        virtual Tag<ToolBase> getTag() = 0; 
      };
    
    
    /** Mixin for attaching a type tag to the concrete tool implementation */
    template<class TOOLImpl, class BASE=Tool<void> >
    class ToolType : public BASE
      {
        typedef typename BASE::ToolBase ToolBase;
        
      public:
        virtual Tag<ToolBase> getTag()
          {
            TOOLImpl* typeref = 0;
            return Tag<ToolBase>::get (typeref); 
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
        typedef typename TOOL::ReturnType ReturnType;
        
        /** generator for Trampoline functions,
         *  used to dispatch calls down to the 
         *  right "treat"-Function on the correct
         *  concrete tool implementation class
         */
        template<class TOOLImpl>
        static ReturnType 
        callTrampoline (TAR& obj, TOOL& tool)
          {
            // cast down to real implementation type
            REQUIRE (INSTANCEOF (TOOLImpl, &tool));  
            TOOLImpl& toolObj = static_cast<TOOLImpl&> (tool);
            
            // trigger (compile time) overload resolution
            // based on concrete type, then dispatch the call.
            // Note this may cause obj to be upcasted.
            return toolObj.treat (obj);
          }
        
        typedef ReturnType (*Trampoline) (TAR&, TOOL& );

        
        /** VTable for storing the Trampoline pointers */
        std::vector<Trampoline> table_;
        
        
        inline bool
        is_known (size_t id)
          { 
            return id<=table_.size() && table_[id-1]; 
          }
        
        inline void 
        storePtr (size_t id, Trampoline func)
          {
            // lacks error- and concurrency handling....
            if (id>table_.size())
              table_.resize (id);
            table_[id-1] = func;
          }
        
        inline Trampoline 
        storedTrampoline (size_t id)
          {
            if (id<=table_.size() && table_[id-1])
              return table_[id-1];
            else
              return &errorHandler;
          }
        
        static ReturnType
        errorHandler (TAR&, TOOL&)
          {
            cout << "Error Handler: unregistered combination of (Tool, TargetObject) invoked!\n";
          }

        
      public:
        static Singleton<Dispatcher<TAR,TOOL> > instance;
        
        inline ReturnType 
        forwardCall (TAR& target, TOOL& tool)
          {
            // get concrete type via tool's VTable
            Tag<TOOL> index = tool.getTag();
            return (*storedTrampoline(index)) (target, tool);
          }
        
        template<class TOOLImpl>
        inline void 
        enrol(TOOLImpl* typeref)
          {
            Tag<TOOL>& index = Tag<TOOL>::get (typeref);
            if (is_known (index))
              return;
            else
              {
                Trampoline func = &callTrampoline<TOOLImpl>;
                storePtr (index, func);
              }
              
          }
      };

    /** storage for the dispatcher table(s) */
    template<class TAR, class TOOL>
    Singleton<Dispatcher<TAR,TOOL> > Dispatcher<TAR,TOOL>::instance;

    
    
    
    
    /** 
     * concrete visiting tool implementation has to inherit from this
     * class for each kind of calls it wants to get dispatched, 
     * Allowing us to record the type information.
     */
    template<class TAR, class TOOLImpl, class BASE=Tool<void> >
    class Applicable
      {
        typedef typename BASE::ReturnType Ret;
        typedef typename BASE::ToolBase ToolBase;
        
      protected:
        Applicable ()
          {
            TOOLImpl* typeref = 0;
            Dispatcher<TAR,ToolBase>::instance().enrol (typeref);
          }
        
        virtual ~Applicable () {}
        
      public:
        // we could enforce the definition of treat()-functions by:
        //
        // virtual Ret treat (TAR& target) = 0;
        
      };
    
    
    
    
    
    /** Marker interface "visitable object".
     */
    template 
      < class TOOL = Tool<void> 
      >
    class Visitable
      {
      protected:
        virtual ~Visitable () { };
        
        /// @note may differ from TOOL
        typedef typename TOOL::ToolBase ToolBase;
        typedef typename TOOL::ReturnType ReturnType;

        /** @internal used by the DEFINE_PROCESSABLE_BY macro.
         *            Dispatches to the actual operation on the 
         *            "visiting tool" (visitor implementation)
         *            Note: creates a context templated on concrete TAR.
         */
        template <class TAR>
        static inline ReturnType
        dispatchOp (TAR& target, TOOL& tool)
          {
            return Dispatcher<TAR,ToolBase>::instance().forwardCall (target,tool);
          }
        
      public:
        /** to be defined by the DEFINE_PROCESSABLE_BY macro
         *  in all classes wanting to be treated by some tool */
        virtual ReturnType apply (TOOL&) = 0;
      };
      
      
/** mark a Visitable subclass as actually treatable
 *  by some "visiting tool". Defines the apply-function,
 *  which is the actual access point to invoke the visiting
 */
#define DEFINE_PROCESSABLE_BY(TOOL) \
        virtual ReturnType  apply (TOOL& tool) \
        { return dispatchOp (*this, tool); }
    
    
    // =============================================================(End) Library ====

    
    
    
    
    namespace test
      {
        
      typedef Tool<void> VisitingTool;

      class HomoSapiens : public Visitable<>
        {
        public:
          DEFINE_PROCESSABLE_BY (VisitingTool);
        };
      
      class Boss : public HomoSapiens
        {
        public:
          DEFINE_PROCESSABLE_BY (VisitingTool);
        };
      
      class BigBoss : public Boss
        {
        public:
          DEFINE_PROCESSABLE_BY (VisitingTool);
        };
        
      class Leader : public Boss
        {
        };
        
      class Visionary : public Leader
        {
        };

      
      class VerboseVisitor
        : public VisitingTool
        {
        protected:
          void talk_to (string guy)
            {
              cout << format ("Hello %s, nice to meet you...\n") % guy;
            }
        };
       
        
      class Babbler
        : public Applicable<Boss,Babbler>,
          public Applicable<BigBoss,Babbler>,
          public Applicable<Visionary,Babbler>,
          public ToolType<Babbler, VerboseVisitor>
        {
        public:
          void treat (Boss&)    { talk_to("Boss"); }
          void treat (BigBoss&) { talk_to("Big Boss"); }
          
        };

        
        
        
      
      
      
      /*************************************************************************
       * @test build and run some common cases for developing and verifying
       *       ichthyo's implementation concept for the Visitor Pattern.
       *       Defines a hierarchy of test classes to check the following cases
       *       <ul><li>calling the correct visiting tool specialized function
       *               for given concrete hierarchy classes</li>
       *           <li>visiting tool not declaring to visit some class</li>
       *           <li>newly added and not properly declared Visitable class
       *               causes the dispatcher to invoke an error handler</li>
       *       </ul>
       */
      class VisitingTool_concept : public Test
        {
          virtual void run(Arg) 
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
              VisitingTool& vista (bab);
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
              VisitingTool& vista (bab);
              homo1.apply (vista);  // error handler (not Applicable to HomoSapiens)
              homo2.apply (vista); //  treats Visionary as Boss
            }
            
        };
      
      
      /** Register this test class... */
      LAUNCHER (VisitingTool_concept, "unit common");
      
      
      
    } // namespace test
    
  } // namespace visitor_concept_draft

} // namespace lumiera
