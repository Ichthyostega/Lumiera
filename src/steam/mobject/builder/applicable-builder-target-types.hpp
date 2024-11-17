/*
  ApplicableBuilderTargetTypes  -  definition header specifying all types treated by builder tools

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file applicable-builder-target-types.hpp
 ** Declaration of all kinds of MObjects to be treated by some "Builder tool".
 ** This is part of Lumiera's visitation mechanism: Individual MObject subclasses
 ** may declare by the \c DEFINE_PROCESSABLE_BY macro to be specifically processable
 ** by a builder tool (visitor). On the other hand, any concrete builder tool (visitor)
 ** is free to define a \c treat(Type) function for each of these specific subclasses.
 ** If the tool doesn't define such a specific \c treat(..) function, the next suitable
 ** function for a supertype will be used.
 ** 
 ** Now there needs to be \em one location where all the specific kinds of treat-able
 ** MObjects are declared together (in a typelist). Moreover, we need the full declaration
 ** of these classes. This is the catch of using the visitor pattern. Thus, any class
 ** to be treated \em specifically (as opposed to be just treated through a supertype
 ** or super interface) has two liabilities:
 ** - DEFINE_PROCESSABLE_BY
 ** - declare the type here in this file, including the header.
 ** 
 ** @note actually the ApplicableBuilderTargetTypes template, when used (as a baseclass
 **       of any concrete builder tool, causes the generation of the necessary
 **       dispatcher tables used by our visitor implementation.
 **       
 ** @see buildertool.hpp
 ** @see buildertooltest.hpp
 ** @see nodecreatertool.hpp
 */



#ifndef MOBJECT_BUILDER_APPLICABLEBUILDERTARGETTYPES_H
#define MOBJECT_BUILDER_APPLICABLEBUILDERTARGETTYPES_H

#include "steam/mobject/builder/buildertool.hpp"

// NOTE:  need to include *all* classes using DEFINE_PROCESSABLE_BY(BuilderTool)
#include "steam/mobject/session/root.hpp"
#include "steam/mobject/session/clip.hpp"
#include "steam/mobject/session/effect.hpp"
#include "steam/mobject/session/auto.hpp"
#include "steam/mobject/session/binding.hpp"

                                        /////////////////////////////////TICKET #414


namespace steam    {
namespace mobject {
namespace builder {
  
    typedef Types< session::Root, 
                   session::Clip, 
                   session::Effect,
                   session::Binding,
                   session::AbstractMO
                 > ::List
                   BuilderTargetTypes;
                                        /////////////////////////////////TICKET #414
    
    
    /**
     * Marker used to declare some visiting Tool class to be actually a
     * mobject::builder::BuilderTool and to possibly accept and treat the
     * common selection of types to be handled by any such builder tool.
     * The actual BuilderTool impl should inherit from this template by
     * feeding back its type (CRTP), this causes a dispatcher table entry
     * to be generated for this concrete BuilderTool implementation.
     */
    template<class TOOL>
    struct ApplicableBuilderTargetTypes 
      : Applicable<TOOL, BuilderTargetTypes>
      { };
    
    
    
}}} // namespace mobject::builder
#endif
