/*
  NODECREATERTOOL.hpp  -  central Tool implementing the Renderengine building

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


/** @file nodecreatortool.hpp
 ** Visitor tool to work on the concrete model objects when building the render node network.
 ** @todo stalled design draft from 2008 -- basically still considered relevant as of 2016
 */


#ifndef MOBJECT_BUILDER_NODECREATORTOOL_H
#define MOBJECT_BUILDER_NODECREATORTOOL_H


#include "proc/mobject/builder/applicable-builder-target-types.hpp"

#include "proc/engine/rendergraph.hpp"



namespace proc    {
namespace mobject {
namespace builder {
  
  class ToolFactory;
  
  
  /**
   * This Tool implementation plays the central role in the build process:
   * given a MObject from Session, it is able to attach ProcNodes to the
   * render engine under construction such as to reflect the properties
   * of the MObject in the actual render.
   */
  class NodeCreatorTool 
    : public ApplicableBuilderTargetTypes<NodeCreatorTool>
    {
      ToolFactory& toolKit_;
      
      /** holds the RenderGraph (Render Engine Element)
       *  to be built by the current build step */
      engine::RenderGraph& proc_;
      
      
      NodeCreatorTool (ToolFactory& tofa, engine::RenderGraph& proc)
        : toolKit_(tofa),
          proc_(proc)
        { }
      
      friend class ToolFactory;
      
    public:
      virtual void treat (mobject::session::Clip& clip) ;
      virtual void treat (mobject::session::Effect& effect) ;
      virtual void treat (mobject::session::Auto<double>& automation) ;  //TODO: the automation-type-problem
      virtual void treat (mobject::Buildable& something) ;
      
      void onUnknown (Buildable& target) ;             /////////TODO why doesn't the treat(Buildable) function shadow this??
    };
  
  
  
}}} // namespace proc:mobject::builder
#endif /*MOBJECT_BUILDER_NODECREATORTOOL_H*/
