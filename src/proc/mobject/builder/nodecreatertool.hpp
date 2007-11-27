/*
  NODECREATERTOOL.hpp  -  central Tool implementing the Renderengine building
 
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


#ifndef MOBJECT_BUILDER_NODECREATERTOOL_H
#define MOBJECT_BUILDER_NODECREATERTOOL_H

#include "proc/mobject/builder/tool.hpp"
#include "proc/mobject/buildable.hpp"
#include "proc/engine/processor.hpp"



namespace mobject
  {
  namespace session
    {
    // Forward declarations
    class Effect;
    class Clip;
    template<class VAL> class Auto;
  }

  namespace builder
    {



    // TODO: define Lifecycle...

    /**
     * This Tool implementation plays the central role in the buld process:
     * given a MObject from Session, it is able to attach ProcNodes to the 
     * render engine under construction such as to reflect the properties 
     * of the MObject in the actual render.
     */
    class NodeCreatorTool : public Tool
      {
      public:
        virtual void treat (mobject::session::Clip& clip) ;
        virtual void treat (mobject::session::Effect& effect) ;
        virtual void treat (mobject::session::Auto<double>& automation) ;  //TODO: the automation-type-problem
        virtual void treat (mobject::Buildable& something) ;


      protected:
        /** holds the Processor (Render Engine Element)
         *  to be built by the current build step 
         */
        engine::Processor* proc;

      };

  } // namespace mobject::builder

} // namespace mobject
#endif
