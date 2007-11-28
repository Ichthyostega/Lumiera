/*
  VISITOR.hpp  -  Acyclic Visitor library
 
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
 
*/



#ifndef CINELERRA_VISITOR_H
#define CINELERRA_VISITOR_H

#include "common/visitorpolicies.hpp"


namespace cinelerra
  {
  namespace visitor
    {
  
  
    /** Marker interface / base class for all "visiting tools".
     *  When applying such a tool to some concrete instance
     *  derived from Visitable, a special function treating
     *  this concrete subclass can be selected on the visiting
     *  tool instance.
     */
    class Tool
      {
      public:
        virtual ~Tool ()  { };  ///< use RTTI for all visiting tools
      };
    
    
    
    /** mixin template to declare that some "visiting tool" 
     *  wants to treat a concrete subclass of Visitable
     */
    template
      < class TAR,
        typename RET = void
      >
    class Applicable
      {
      public:
        virtual ~Applicable ()  { };
        
        /** to be implemented by concrete tools
         *  wanting to visit type TAR */
        virtual RET  treat (TAR& visitable)  = 0;
      };
    
    
      
    /** Marker interface / base class for all types
     *  to be treated by a "visiting tool" or visitor.
     */
    template 
      < typename RET = void,
        class TOOL = Tool,
        template<typename,class> class ERR = UseDefault 
      >
    class Visitable
      {
      public:
        /** to be defined by the DEFINE_PROCESSABLE_BY macro
         *  in all classes wanting to be treated by some tool */
        virtual RET apply (TOOL&) = 0;
        typedef RET ReturnType;
        
      protected:
        virtual ~Visitable () { };
        
        /** @internal used by the DEFINE_VISITABLE macro.
         *            Dispatches to the actual operation on the 
         *            "visiting tool" (acyclic visitor implementation)
         */
        template <class TAR>
        static RET dispatchOp(TAR& target, TOOL& tool)
          {
            if (Applicable<TAR,RET>* concreteTool
                  = dynamic_cast<Applicable<TAR,RET>*> (&tool))
              
              return concreteTool->treat (target);
            
            else
              return ERR<RET,TOOL>::onUnknown (target,tool);
          }
      };
      

/** mark a Visitable subclass as actually treatable
 *  by some "visiting tool". Defines the apply-function,
 *  which is the actual access point to invoke the visiting
 */
#define DEFINE_PROCESSABLE_BY(TOOL) \
        virtual ReturnType  apply (TOOL& tool) \
        { return dispatchOp (*this, tool); }
    
    
    
  } // namespace visitor

} // namespace cinelerra
#endif
