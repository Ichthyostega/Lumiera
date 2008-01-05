/*
  VISITOR.hpp  -  Generic Visitor library implementation
 
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

====================================================================
This code is heavily inspired by  
 The Loki Library (loki-lib/trunk/include/loki/Visitor.h)
    Copyright (c) 2001 by Andrei Alexandrescu
    This Loki code accompanies the book:
    Alexandrescu, Andrei. "Modern C++ Design: Generic Programming
        and Design Patterns Applied". 
        Copyright (c) 2001. Addison-Wesley. ISBN 0201704315
 
Credits for many further implementation ideas go to
 Cooperative Visitor: A Template Technique for Visitor Creation
    by Anand Shankar Krishnamoorthi
    July 11, 2007
    http://www.artima.com/cppsource/cooperative_visitor.html

*/


/** @file visitor.hpp
 ** A library implementation of the <b>Visitor Pattern</b> taylored specifically
 ** to cinelerra's needs within the Proc Layer. Visitor enables <b>double dispatch</b>
 ** calls, based both on the concrete type of some target object and the concrete type of
 ** a tool object being applied to this target. The code carrying out this tool application
 ** (and thus triggering the double dispatch) need not know any of these concret types and is
 ** thus completely decoupled form implementation details encapsulated within the visiting tool.
 ** The visiting tool implementation class provides specific "treat(ConcreteVisitable&)" functions,
 ** and this visitor lib will dispatch the call to the* correct "treat"-function based on the 
 ** concrete target visitable type.
 **
 ** Implementation notes
 ** <ul><li>driven by dispatch tables with trampoline functions.</li>
 **     <li>uses Typelists and Template metaprogramming to generate 
 **         Dispatcher tables for the concrete types.</li>
 **     <li>individual Visiting Tool implementation classes need to derive
 **         from some Applicable<TARGET, TOOLImpl> instantiation and thus
 **         define which calls they get dispatched. They are free to implement
 **         corresponding "treat(ConcreteVisitable&) functions or fall back
 **         on some treat(VisitableInterface&) function.</li>
 ** </ul>
 ** For design questions and more detailed implementation notes, see the Proc Layer Tiddly Wiki.  
 **
 ** @see visitingtooltest.cpp test cases using our lib implementation
 ** @see BuilderTool one especially important instantiiation
 ** @see visitordispatcher.hpp
 ** @see typelist.hpp
 **
 */



#ifndef CINELERRA_VISITOR_H
#define CINELERRA_VISITOR_H

#include "common/visitorpolicies.hpp"
#include "common/visitordispatcher.hpp"


namespace cinelerra
  {
  namespace visitor
    {
  
  
    /** 
     * Marker interface / base class for all "visiting tools".
     * When applying such a tool to some concrete instance
     * derived from Visitable, a special function treating
     * this concrete subclass will be selected on the 
     * concrete visiting tool instance.
     */
    template
      < typename RET = void,                
        template <class> class ERR = UseDefault
      >        
    class Tool : public ERR<RET>
      {
      public:
        typedef RET ReturnType;      ///< Tool function invocation return type
        typedef Tool ToolBase;      ///<  for templating the Tag and Dispatcher
        
        virtual ~Tool ()  { };   ///< use RTTI for all visiting tools
        
        /** allows discovery of the concrete Tool type when dispatching a
         *  visitor call. Can be implemented by inheriting from ToolTag */
        virtual Tag<ToolBase> getTag() = 0; 
      };
    
    
    /** 
     * Mixin for attaching a type tag to the concrete tool implementation 
     */
    template<class TOOLImpl, class BASE=Tool<> >
    class ToolTag : public BASE
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
     * Mixin template to declare that some "visiting tool" 
     * wants to treat a concrete subclass of Visitable.
     * 
     * Concrete visiting tool implementation has to inherit from 
     * an instance of this template class for each kind of calls 
     * it wants to get dispatched, allowing us to record the type
     * information and register the dispatcher entry via the 
     * automatic base ctor call.
     */
    template
      < class TAR,            // concrete Visitable to be treated
        class TOOLImpl,      // concrete tool implementation type
        class BASE=Tool<>   // "visiting tool" base class 
      >
    class Applicable
      {
        typedef typename BASE::ReturnType Ret;
        typedef typename BASE::ToolBase ToolBase;
        
      protected:
        Applicable ()
          {
            TOOLImpl* typeref = 0;
            Dispatcher<TAR,ToolBase>::instance().enroll (typeref);
          }
        
        virtual ~Applicable () {}
      };
    
    
      
    /** 
     * Marker interface or base class for all "Visitables". 
     * Concrete types to be treated by a "visiting tool" derive from
     * this interface and need to implement an #apply(Tool&), forwarding
     * to the (internal, static, templated) #dispatchOp. This is done
     * best by using the #DEFINE_PROCESSABLE_BY macro.
     */
    template 
      < class TOOL = Tool<> 
      >
    class Visitable
      {
      protected:
        virtual ~Visitable () { };
        
        /// @note may differ from TOOL
        typedef typename TOOL::ToolBase ToolBase;
        typedef typename TOOL::ReturnType ReturnType;

        /** @internal used by the #DEFINE_PROCESSABLE_BY macro.
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
      

/** mark a Visitable subclass as actually treatable by some
 * "visiting tool" base interface. Defines the apply-function,
 *  which is the actual access point to invoke the visiting
 */
#define DEFINE_PROCESSABLE_BY(TOOL) \
        virtual ReturnType  apply (TOOL& tool) \
        { return dispatchOp (*this, tool); }
    
    
    
  } // namespace visitor

} // namespace cinelerra
#endif
