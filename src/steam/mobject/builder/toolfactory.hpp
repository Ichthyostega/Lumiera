/*
  TOOLFACTORY.hpp  -  supply of Tool implementations for the Builder

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


/** @file toolfactory.hpp
 ** Factory to create the tools for the build process.
 ** Obviously the logic to control and direct the build process will be attached
 ** behind this interface.
 ** 
 ** @todo stalled design draft from 2008 -- basically still considered relevant as of 2016
 */


#ifndef MOBJECT_BUILDER_TOOLFACTORY_H
#define MOBJECT_BUILDER_TOOLFACTORY_H

#include "steam/mobject/session/fixture.hpp"
#include "steam/mobject/builder/segmentation-tool.hpp"
#include "steam/mobject/builder/nodecreatortool.hpp"
#include "steam/mobject/builder/mould.hpp"
#include "steam/engine/rendergraph.hpp"

#include <memory>



namespace proc {
namespace mobject {
namespace builder {
  
  struct BuildProcessState;
  
  
  /**
   * provides the builder with the necessary, preconfigured tools.
   * Dedicated to a single build process, it holds the internal state
   * of this process and thus serves to coordinate and link together all
   * the individual parts fabricated by using the various tools.
   */
  class ToolFactory
    {
      std::unique_ptr<BuildProcessState> state_;
      
    public:
      /** prepare a builder tool kit for dealing with the given Fixture,
       *  which is a snapshot of some timeline made explicit. */
      ToolFactory (session::Fixture&);
      
      /** prepare a tool for properly segmenting the Fixture */
      SegmentationTool &  configureSegmentation ();
      
      /** prepare a tool for building the render engine (graph) for a single segment */
      NodeCreatorTool &   configureFabrication ();
      
      /** receive the finished product of the build process; effectively
       *  releases any other builder tool object
       * @todo unclear what to do and to return here
       */
      engine::RenderGraph& getProduct ();
    };
  
  
  
}}} // namespace proc::mobject::builder
#endif
