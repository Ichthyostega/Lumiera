/*
  TOOLFACTORY.hpp  -  supply of Tool implementations for the Builder
 
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


#ifndef MOBJECT_BUILDER_TOOLFACTORY_H
#define MOBJECT_BUILDER_TOOLFACTORY_H

#include "proc/mobject/builder/buildertool.hpp"
#include "proc/mobject/builder/mould.hpp"
#include "proc/asset/pipe.hpp"
#include "proc/mobject/session/clip.hpp"
#include "proc/mobject/builder/wiringrequest.hpp"

#include <vector>


namespace mobject {
  namespace builder {

    using std::vector;
    using asset::PPipe;
    using session::PClipMO;

    /**
     * provides the builder with the necessary, preconfigured tools.
     * Dedicated to a single build process, it holds the internal state
     * of this process and thus serves to coordinate and link together all
     * the individual parts fabricated by using the various tools. 
     */
    class ToolFactory
      {
      public:
        PipeMould&        provideMould(PPipe const&);
        CombiningMould&   provideMould(vector<PPipe const>&);
        SourceChainMould& provideMould(PClipMO const&);
        WiringMould&      provideMould(WiringRequest const&);
        
      };



  } // namespace mobject::builder

} // namespace mobject
#endif
