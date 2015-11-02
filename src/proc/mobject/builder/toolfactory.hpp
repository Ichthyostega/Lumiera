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


#ifndef MOBJECT_BUILDER_TOOLFACTORY_H
#define MOBJECT_BUILDER_TOOLFACTORY_H

#include "proc/mobject/session/fixture.hpp"
#include "proc/mobject/builder/segmentation-tool.hpp"
#include "proc/mobject/builder/nodecreatortool.hpp"
#include "proc/mobject/builder/mould.hpp"
#include "proc/engine/rendergraph.hpp"

#include <boost/scoped_ptr.hpp>
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
      boost::scoped_ptr<BuildProcessState> state_;
      
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
       *                                   //////////////////////////////////////////TODO a better idea than using auto_ptr?
       */
      std::auto_ptr<engine::RenderGraph> getProduct ();
    };
  
  
  
}}} // namespace proc::mobject::builder
#endif
