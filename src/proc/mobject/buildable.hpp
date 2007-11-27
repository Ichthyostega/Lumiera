/*
  BUILDABLE.hpp  -  marker interface denoting any (M)Object able to be treated by Tools
 
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
 
*/


#ifndef MOBJECT_BUILDABLE_H
#define MOBJECT_BUILDABLE_H


#include "common/visitor.hpp"



namespace mobject
  {


  namespace builder{ class BuilderTool; }

  using cinelerra::visitor::Visitable;
  

  /**
   *  All Buidables support double-dispatch of given Tool operations.
   *  The actual operation is thus selected at runtime based both on the 
   *  actual type of the Tool class /and/ the actual type of the Buildabele.
   */
  class Buildable : public Visitable<>
    {
    public:
      /** Catch-all implementation for applying any builder tool
       *  to some (uspecified) buildable object. Typically the provided
       *  actual Tool class will contain overloaded fuctions for treating
       *  different Buildable subclasses specifically and the concrete Buildables
       *  will define explicitly to be specifically visitable. 
       */
      virtual ReturnType apply (builder::BuilderTool&);
    };



} // namespace mobject
#endif
