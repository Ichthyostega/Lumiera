/*
  ApplicableBuilderTargetTypes  -  definition header specifying all types treated by builder tools
 
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


#ifndef MOBJECT_BUILDER_APPLICABLEBUILDERTARGETTYPES_H
#define MOBJECT_BUILDER_APPLICABLEBUILDERTARGETTYPES_H

#include "proc/mobject/builder/buildertool.hpp"



namespace mobject {
  namespace session {
    
    class Clip;
    class Effect;
    class AbstractMO;
    template<class VAL> class Auto;
    // Forward declarations sufficient here...
    // actual definitions necessary only in the
    // implementation file (*cpp) of the builder tool.
  }
  
  namespace builder {
  
    typedef Types< session::Clip, 
                   session::Effect,
                   session::AbstractMO
                 > ::List
                   BuilderTargetTypes;
    
    
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
    
    
    
  } // namespace mobject::builder

} // namespace mobject
#endif
