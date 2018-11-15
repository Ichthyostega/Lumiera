/*
  MOULD.hpp  -  builder tool kit for the basic building situations

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


/** @file mould.hpp
 ** Builder tool kit.
 ** On the basic level, the builder works by wiring up new processing nodes,
 ** guided by a small number of preconfigured building patterns. For each such
 ** basic wiring situation, there is a builder::Mould, which acts as a fixed
 ** counterpart for the node network in construction to hinge on.
 ** 
 ** @todo plan and draft from 2008 -- still considered relevant as of 2016
 */


#ifndef MOBJECT_BUILDER_MOULD_H
#define MOBJECT_BUILDER_MOULD_H


#include "steam/asset/procpatt.hpp"
#include "steam/mobject/builder/operationpoint.hpp"

#include <vector>

namespace steam    {
namespace mobject {
namespace builder {
  
  using std::vector;
  using asset::PProcPatt;
  using lib::Symbol;
  
  
  /**
   * Interface: a workbench-like tool used by the builder
   * for wiring up a specific building situation, followed by
   * the application/execution of a single building step. Mould is
   * conceived as the passive part, while usually the ProcPatt plays
   * the role of the active counterpart. By means of the Mould interface,
   * the specifics of a build situation are abstracted away, thus allowing
   * the processing pattern to be defined as working on symbolic locations.
   * Most commonly this is "current", denoting the render node just being built.
   * - PipeMould supports attaching an effect to a pipe
   * - combining pipes via a transition is done by a CombiningMould
   * - a SourceChainMould allows to start out from a source reader and build a clip
   * - wiring general connections is supported by the WiringMould
   * 
   * @see ToolFactory
   * @see NodeCreatorTool
   */
  class Mould
    {
    public:
      vector<Mould> operate ();
      
      OperationPoint& getLocation (Symbol locationID);
    };
  
  
  class PipeMould : public Mould
    {
    };
  
  class CombiningMould : public Mould
    {
    };
  
  class SourceChainMould : public Mould
    {
    };
  
  class WiringMould : public Mould
    {
    };
  
  
}}} // namespace steam::mobject::builder
#endif
