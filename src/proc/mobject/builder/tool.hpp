/*
  TOOL.hpp  -  Interface, any tool for processing MObjects
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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

#include "proc/mobject/buildable.hpp"



namespace mobject
  {
  namespace builder
    {


    /**
     * Used according to the visitor pattern: each Tool contains 
     * the concrete implementation for one task to be done to the various MObject classes
     */
    class Tool
      {
      protected:
        typedef mobject::Buildable Buildable;

      public:
        /** This operation is to be overloaded for specific MObject subclasses to be treated.
         */
        virtual void treat (Buildable& mElement) = 0;
      };

  } // namespace mobject::builder

} // namespace mobject
#endif
