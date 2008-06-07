/*
  BUILDERTOOL.hpp  -  Interface, (visiting) tool for processing MObjects
 
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
 
*/

/** @file buildertool.hpp
 ** Visiting-tool mechanism configured specifically for the Builder.
 ** The Builder creates the render nodes network by applying several Builder Tools
 ** to the objects found in the Session, EDL and Fixture. These BuilderTool instances
 ** contain the details of the builder implementation.
 ** 
 ** As the objects to be treated are normally handled by smart-ptrs, BuilderTool provides
 ** a special facility for dealing with these wrapped objects. There are some liabilities.
 ** <ul><li>each concrete Buildable subtype to be treated specifically needs to
 **         declare \c DEFINE_PROCESSABLE_BY(BuilderTool) </li>
 **     <li>at the same time, the concrete BuilderTool subclass has to declare
 **         being Applicable to this concrete Buildable subtype. The recommended way
 **         of ensuring this, is to add an entry to applicablebuildertargettypes.hpp
 **         and then derive the concrete BuilderTool subclass from 
 **         ApplicableBuilderTargetTypes</li>
 **     <li>when accessing the wrapper from within a \c treat() function, a suitable
 **         concrete wrapper type has to be specified. If the wrapper type used for
 **         invoking the BuilderTool (function \c apply(BuilderTool&l, WrappedObject&) )
 **         can not be converted to this type requested from within the call, an 
 **         assertion failure (or segmentation fault in a release build) will result.</li>
 ** </ul>   
 ** 
 ** @see visitor.hpp
 ** @see applicablebuildertargettypes.hpp
 ** @see buildertooltest.hpp
 ** @see nodecreatertool.hpp
 */



#ifndef MOBJECT_BUILDER_TOOL_H
#define MOBJECT_BUILDER_TOOL_H

#include "common/visitor.hpp"
#include "common/wrapperptr.hpp"

#include "common/p.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/explicitplacement.hpp"


namespace mobject {

  class Buildable;
  
  namespace builder {
  
    using lumiera::P;
    
    /** 
     * Policy invoking an catch-all function for processing
     * an unknown tool / target pair, effectively enforcing the
     * implementation of a catch-all function \c onUnknown(BASE&)
     */
    template<class RET>
    class InvokeCatchAllFunction
      {
      protected:
        virtual ~InvokeCatchAllFunction() {}
      public:
        virtual RET onUnknown (Buildable& target) = 0;
      };
    
    
    
    /**
     * Base class of all BuilderTools, used according to the visitor pattern: 
     * each Tool contains the concrete implementation for one task to be done
     * to the various MObject classes. The concrete builder tool implementation
     * should not diretcly inherit from this base interface, but rather through
     * an instantiation of the "Applicable" template parametrized with all 
     * concrete Buildable classes, for which it wants calls to be dispatched.
     * \par
     * In addition to lumiera::visitor::Tool, BuilderTool adds support for dealing
     * with objects normally handled by means of smart-pointers or similar
     * wrappers, most notably mobject::Placement. The visitaion is initiated
     * by calling the stand-alone function \c (BuilderTool& tool, WRA& wrappedTargetObj) ,
     * which forwards to the visitation mechanism supported by the type contained
     * in the wrapper, but stores away a pointer to the wrapped object, which can
     * be retrieved in a typesafe manner from within the  \c treat(ConcreteType&)
     * function. 
     * @note retrieving the wrapper is <b>not threadsafe</b> and <b>not reentrant</b>,
     *       as we simply store a pointer within the BuilderTool instance.
     */
    class BuilderTool
      : public lumiera::visitor::Tool<void, InvokeCatchAllFunction> 
      {
        lumiera::WrapperPtr currentWrapper_;
        
      public:
        
        template<template<class> class WRA, class TAR>
        void rememberWrapper (WRA<TAR>* ptr_toWrappedTarget)
          {
            currentWrapper_ = ptr_toWrappedTarget;
          }
        
        void forgetWrapper ()
          {
            currentWrapper_.reset();
          }
        
        
      protected:  /* == interface for accessing the wrapper from within tool application == */
        
        template<class TAR>
        Placement<TAR>&
        getPlacement ()
          {
            Placement<TAR>* pPlacement = currentWrapper_.get<Placement<TAR>*>();
            ENSURE (pPlacement,  "wrong target type when invoking %s", __PRETTY_FUNCTION__);
            return *pPlacement;
          }
        
        ExplicitPlacement
        getExplicitPlacement ()
          {
            return getPlacement<MObject>().resolve();
          }
        
        template<class TAR>
        lumiera::P<TAR>
        getPtr ()
          {
            P<TAR>* pP = currentWrapper_.get<P<TAR>*>(); 
            ENSURE (pP, "wrong target type when invoking %s", __PRETTY_FUNCTION__);
            return *pP;
          }
      };
    
    
    
    /** 
     * declare the concrete types a BuilderTool may recievee and treat.
     * @note it is recommended to use ApplicableBuilderTargetTypes
     */
    template
      < class TOOLImpl,  // concrete BuilderTool implementation
        class TYPELIST  //  list of all concrete Buildables to be treated
      >
    class Applicable
      : public lumiera::visitor::Applicable<TOOLImpl, TYPELIST, BuilderTool>
      { }
      ;
      
    using lumiera::typelist::Types;  // convenience for the users of "Applicable"
  
  } // namespace mobject::builder
  
  
  
  /**
   *  Marker Interface for classes visitable by Builder tools. 
   */
  class Buildable : public lumiera::visitor::Visitable<builder::BuilderTool>
    { };
  
  
  
  
  namespace builder { // to be found by ADL
    
    template<typename WRA>
    inline Buildable::ReturnType
    apply (BuilderTool& tool, WRA& wrappedTargetObj)
    {
      tool.rememberWrapper(&wrappedTargetObj);
      wrappedTargetObj->apply (tool);   // dispatch to suitable treat() function
      tool.forgetWrapper();
    }
    
  } // namespace mobject::builder
  
} // namespace mobject
#endif
