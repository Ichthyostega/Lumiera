/*
  ApplicableBuilderTargetTypes  -  definition header specifying all types treated by builder tools

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

#include "proc/mobject/builder/buildertool.hpp"

// NOTE:  need to include *all* classes using DEFINE_PROCESSABLE_BY(BuilderTool)
#include "proc/mobject/session/root.hpp"
#include "proc/mobject/session/clip.hpp"
#include "proc/mobject/session/effect.hpp"
#include "proc/mobject/session/auto.hpp"
#include "proc/mobject/session/binding.hpp"

                                        /////////////////////////////////TICKET #414


namespace proc    {
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
