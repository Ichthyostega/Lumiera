/*
  VISITOR.hpp  -  Generic Visitor library implementation

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
    by Anand Shankar Krishnamoorthi   (July 2007)
    http://www.artima.com/cppsource/cooperative_visitor.html

*/


/** @file visitor.hpp
 ** A library implementation of the *Visitor Pattern* tailored specifically
 ** to Lumiera's needs within the Steam Layer. Visitor enables *double dispatch*
 ** calls, based both on the concrete type of some target object and the concrete type of
 ** a tool object being applied to this target. The code carrying out this tool application
 ** (and thus triggering the double dispatch) need not know any of these concrete types and is
 ** thus completely decoupled form implementation details encapsulated within the visiting tool.
 ** The visiting tool implementation class provides specific "treat(ConcreteVisitable&)" functions,
 ** and this visitor lib will dispatch the call to the* correct "treat"-function based on the 
 ** concrete target visitable type.
 **
 ** Implementation notes
 ** - driven by dispatch tables with trampoline functions.
 ** - uses Typelists and Template metaprogramming to generate
 **   Dispatcher tables for the concrete types.
 ** - individual Visiting Tool implementation classes need to derive
 **   from some Applicable<TOOLImpl, Types<Type1,Type2,...> > instantiation
 **   and thus define which calls they get dispatched. _This is crucial_.
 **   A concrete type not declared in this way will never be dispatched to this
 **   concrete visiting tool implementation class. Of course, the latter is free
 **   to implement corresponding "treat(ConcreteVisitable&) functions or fall back
 **   on some treat(VisitableInterface&) function.
 ** - any concrete Visitable subclass wanting to be treated by some concrete tool
 **   needs to use the `DECLARE_PROCESSABLE_BY(TOOLBASE)` macro. By this, it gets an
 **   virtual `apply(TOOLBASE&)` function. Otherwise, it will be treated by the
 **   interface of the next base class using this macro.
 ** 
 ** @remarks as of 2016, it is not clear if we'll really use this facility; it was meant to play a
 **   crucial role in the Builder (which is not implemented yet....). The fundamental idea of relying
 **   on a visitor seems still adequate though. For design questions and more detailed implementation
 **   notes, see the [TiddlyWiki].
 **
 ** [TiddlyWiki]: http://lumiera.org/wiki/renderengine.html#VisitorUse%20VisitingToolImpl%20BuilderStructures%20BuilderMechanics "Lumiera Tiddly Wiki"
 **
 ** @see visitingtooltest.cpp test cases using our lib implementation
 ** @see BuilderTool one especially important instantiation
 ** @see visitor-dispatcher.hpp
 ** @see typelist.hpp
 **
 */



#ifndef LIB_VISITOR_H
#define LIB_VISITOR_H

#include "lib/visitor-policies.hpp"
#include "lib/visitor-dispatcher.hpp"

#include "lib/meta/typelist.hpp"


namespace lib {
namespace visitor {
  
  namespace typelist = lib::meta;
  
  
  /** 
   * Marker interface / base class for all "visiting tools".
   * When applying such a tool to some concrete instance
   * derived from Visitable, a special function treating
   * this concrete subclass will be selected on the 
   * concrete visiting tool instance.
   */
  template
    < typename RET = void
    , template <class> class ERR = UseDefault
    >
  class Tool : public ERR<RET>
    {
    public:
      typedef RET ReturnType;      ///< Tool function invocation return type
      typedef Tool ToolBase;      ///<  for templating the Tag and Dispatcher
      
      virtual ~Tool ()  { };    ///< use RTTI for all visiting tools
      
      /** allows discovery of the concrete Tool type when dispatching a
       *  visitor call. Can be implemented by inheriting from ToolTag */
      virtual Tag<ToolBase> getTag() = 0; 
    };
  
  
  
  
  /** 
   * Marker template to declare that some "visiting tool" 
   * wants to treat a set of concrete Visitable classes.
   * 
   * Each "first class" concrete visiting tool implementation has
   * to inherit from an instance of this template parametrised with
   * the desired types; for each of the mentioned types, calls will 
   * be dispatched to the tool implementation. (To make it clear:
   * Calls to all other types not marked by such an "Applicable"
   * won't ever be dispatched to this tool class.).
   * A Sideeffect of inheriting from such an "Applicable" is that 
   * the tool gets an unique Tag entry, which is used internally
   * as index in the dispatcher tables. And the automatic ctor call
   * allows us to record the type information and pre-register the
   * dispatcher entry.
   */
  template
    < class TOOLImpl,      // concrete tool implementation type
      class TYPES,        // List of applicable Types goes here...
      class BASE=Tool<>  // "visiting tool" base class 
    >
  class Applicable;
  
  template           // recursion end: inherit from BASE
    < class TOOLImpl,
      class BASE 
    >
  class Applicable<TOOLImpl, typelist::NullType, BASE>
    : public BASE
  { }
  ;
  
  template
    < class TOOLImpl,
      class TAR, class TYPES,
      class BASE 
    >
  class Applicable<TOOLImpl, typelist::Node<TAR, TYPES>, BASE>
    : public Applicable<TOOLImpl, TYPES, BASE>
    {
      
      typedef typename BASE::ToolBase ToolBase;
      
    protected:
      virtual ~Applicable () {}
      Applicable ()
        {
          TOOLImpl* typeref = 0;
          Dispatcher<TAR,ToolBase>::instance().enrol (typeref);
        }
      
    public:
      virtual Tag<ToolBase> 
      getTag ()
        {
          TOOLImpl* typeref = 0;
          return Tag<ToolBase>::get (typeref);
        }
    };
  
  using typelist::Types;  // convenience for the user of "Applicable"
  
  
  
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
    public:
      typedef typename TOOL::ReturnType ReturnType;
      
      /** to be defined by the DEFINE_PROCESSABLE_BY macro
       *  in all classes wanting to be treated by some tool */
      virtual ReturnType apply (TOOL&) = 0;
      
      
    protected:
      virtual ~Visitable () { };
      
      /// @note may differ from TOOL
      typedef typename TOOL::ToolBase ToolBase;
      
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
    };
    

/** mark a Visitable subclass as actually treat-able by some
 * "visiting tool" base interface. Defines the apply-function,
 *  which is the actual access point to invoke the visiting
 */
#define DEFINE_PROCESSABLE_BY(TOOL) \
        virtual ReturnType  apply (TOOL& tool) \
        { return dispatchOp (*this, tool); }
  
  
  
}} // namespace lib::visitor
#endif
