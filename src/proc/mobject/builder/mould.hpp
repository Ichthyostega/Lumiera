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


#ifndef MOBJECT_BUILDER_MOULD_H
#define MOBJECT_BUILDER_MOULD_H


#include "proc/asset/procpatt.hpp"
#include "proc/mobject/builder/operationpoint.hpp"

#include <vector>

namespace proc    {
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
   * <ul><li>PipeMould supports attaching an effect to a pipe</li>
   *     <li>combining pipes via a transition is done by a CombiningMould</li> 
   *     <li>a SourceChainMould allows to start out from a source reader and build a clip</li> 
   *     <li>wiring general connections is supported by the WiringMould</li>
   * </ul>
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
  
  
}}} // namespace proc::mobject::builder
#endif
