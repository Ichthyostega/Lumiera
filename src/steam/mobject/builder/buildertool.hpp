/*
  BUILDERTOOL.hpp  -  Interface, (visiting) tool for processing MObjects

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

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
 ** to the objects found in the Session and Fixture. These BuilderTool instances
 ** contain the details of the builder implementation.
 ** 
 ** As the objects to be treated are normally handled by smart-ptrs, BuilderTool provides
 ** a special facility for dealing with these wrapped objects. There are some liabilities.
 ** - each concrete Buildable subtype to be treated specifically needs to
 **   declare `DEFINE_PROCESSABLE_BY(BuilderTool)`
 ** - at the same time, the concrete BuilderTool subclass has to declare
 **   being Applicable to this concrete Buildable subtype. The recommended way
 **   of ensuring this, is to add an entry to applicable-builder-target-types.hpp
 **   and then derive the concrete BuilderTool subclass from 
 **   ApplicableBuilderTargetTypes
 ** - when accessing the wrapper from within a `treat()` function, a suitable
 **   concrete wrapper type has to be specified. If the wrapper type used for
 **   invoking the BuilderTool (function `apply(BuilderTool&, WrappedObject&)`)
 **   can not be converted to this type requested from within the call, an 
 **   assertion failure (or segmentation fault in a release build) will result.
 ** 
 ** @see visitor.hpp
 ** @see applicable-builder-target-types.hpp
 ** @see buildertooltest.hpp
 ** @see nodecreatertool.hpp
 */



#ifndef MOBJECT_BUILDER_TOOL_H
#define MOBJECT_BUILDER_TOOL_H

#include "lib/visitor.hpp"
#include "lib/wrapperptr.hpp"

#include "lib/p.hpp"
#include "steam/mobject/placement.hpp"
#include "steam/mobject/explicitplacement.hpp"


namespace steam    {
namespace mobject {
  
  class Buildable;
  
  namespace builder {
  
    
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
     * should not directly inherit from this base interface, but rather through
     * an instantiation of the "Applicable" template parametrised with all 
     * concrete Buildable classes, for which it wants calls to be dispatched.
     * \par
     * In addition to lib::visitor::Tool, BuilderTool adds support for dealing
     * with objects normally handled by means of smart-pointers or similar
     * wrappers, most notably mobject::Placement. The visitation is initiated
     * by calling the stand-alone function \c (BuilderTool& tool, WRA& wrappedTargetObj) ,
     * which forwards to the visitation mechanism supported by the type contained
     * in the wrapper, but stores away a pointer to the wrapped object, which can
     * be retrieved in a typesafe manner from within the  \c treat(ConcreteType&)
     * function. 
     * @note retrieving the wrapper is <b>not threadsafe</b> and <b>not reentrant</b>,
     *       as we simply store a pointer within the BuilderTool instance.
     */
    class BuilderTool
      : public lib::visitor::Tool<void, InvokeCatchAllFunction>
      { lumiera::WrapperPtr currentWrapper_;
        
      public:
        
        template<template<class> class WRA, class TAR>
        void rememberWrapper (WRA<TAR>* ptr_toWrappedTarget)
          {
            currentWrapper_ = ptr_toWrappedTarget;
          }
        template<template<class,class> class Placement, class TAR, class B>
        void rememberWrapper (Placement<TAR,B>* ptr_toWrappedTarget)
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
        lib::P<TAR>
        getPtr ()
          {
            lib::P<TAR>* pP = currentWrapper_.get<lib::P<TAR>*>();
            ENSURE (pP, "wrong target type when invoking %s", __PRETTY_FUNCTION__);
            return *pP;
          }
      };
    
    
    
    /** 
     * declare the concrete types a BuilderTool may receive and treat.
     * @note it is recommended to use ApplicableBuilderTargetTypes
     */
    template
      < class TOOLImpl,  // concrete BuilderTool implementation
        class TYPELIST  //  list of all concrete Buildables to be treated
      >
    class Applicable
      : public lib::visitor::Applicable<TOOLImpl, TYPELIST, BuilderTool>
      { }
      ;
      
    using lib::meta::Types;  // convenience for the users of "Applicable"
  
  }// namespace mobject::builder
  
  
  
  
  
  /**
   *  Marker Interface for classes visitable by Builder tools. 
   */
  class Buildable : public lib::visitor::Visitable<builder::BuilderTool>
    { };
  
  
  
  
  namespace builder {
    
    /** to be picked up by ADL: redirect tool invocation for double dispatch.
     *  The purpose of this function is to apply a visitor, while the actual target
     *  is managed by a generic wrapper (smart-ptr). This template function serves
     *  to generate forwarding functions, which pass on the \c apply() call to the
     *  actual embedded target, while passing on the fully wrapped object for later
     *  referral and usage too.
     */
    template<typename WRA>
    inline Buildable::ReturnType
    apply (BuilderTool& tool, WRA& wrappedTargetObj)
    {
      tool.rememberWrapper(&wrappedTargetObj);
      wrappedTargetObj->apply (tool);   // dispatch to suitable treat() function
      tool.forgetWrapper();
    }
  
  
}}} // namespace steam::mobject::builder
#endif
