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


#ifndef MOBJECT_BUILDER_TOOL_H
#define MOBJECT_BUILDER_TOOL_H

#include "common/visitor.hpp"



namespace mobject
  {
  class Buildable;
  
  namespace builder
    {
    
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
     */
    typedef cinelerra::visitor::Tool<void, InvokeCatchAllFunction> BuilderTool;
    
    
    template
      < class TOOLImpl,  // concrete BuilderTool implementation
        class TYPELIST  //  list of all concrete Buildables to be treated
      >
    class Applicable
      : public cinelerra::visitor::Applicable<TOOLImpl, TYPELIST, BuilderTool>
      { }
      ;
      
    using cinelerra::typelist::Types;   // convienience for the users of "Applicable"

  } // namespace mobject::builder
  
  
  
  /**
   *  Marker Interface for classes Visitable by Builder tools. 
   */
  class Buildable : public cinelerra::visitor::Visitable<builder::BuilderTool>
    { };

} // namespace mobject
#endif
