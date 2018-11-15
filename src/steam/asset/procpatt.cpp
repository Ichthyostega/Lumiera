/*
  ProcPatt  -  template for building some render processing network

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

* *****************************************************/


/** @file procpatt.cpp
 ** Implementation details related to "processing pattern" assets.
 */


#include "steam/asset/procpatt.hpp"
#include "steam/asset/proc.hpp"
#include "steam/assetmanager.hpp"
#include "steam/asset/buildinstruct.hpp"
#include "lib/util.hpp"

using util::isnil;

namespace proc {
namespace asset {
  
  
  /// emit VTable and member destructors here...
  ProcPatt::~ProcPatt() { }
  
  
  
  /** new processing pattern with empty instruction list.
   *  @todo preliminary implementation, storing the capabilities
   *        in the asset name field. We can do better, when
   *        it's clear what capabilities we need
   */
  ProcPatt::ProcPatt (const Asset::Ident& idi)
    : Struct (idi),
      instructions_()
  { }
  
  
  /** @internal used for creating a clone copy */
  ProcPatt::ProcPatt (const Asset::Ident& idi, const InstructionSequence& instru)
    : Struct (idi),
      instructions_(instru)
  {
    TODO ("verify building instructions, maybe preprocess...");
  }
  
  
  
  /** create a new ProcPatt asset as a literal copy
   *  of this one. The new ProcPatt can then be customised
   *  independently of the original one. This allows using
   *  some ProcPatt as a template for creating more
   *  specialised patterns. 
   */
  lib::P<ProcPatt> 
  ProcPatt::newCopy (string newID)  const
  {
    TODO ("implement the Pattern-ID within the propDescriptor!");
    TODO ("implement a consistent struct asset naming scheme at one central location!!!!!");
    const Asset::Ident newIdi ( this->ident.name+".X"
                              , this->ident.category
                              );
    ProcPatt* pP = new ProcPatt (newIdi, this->instructions_);
    return AssetManager::instance().wrap (*pP);
  }
  
  
  /** extend the processing instructions to add some Effect
   *  @param where denotes the insertion point where to attach the Effect
   *  @param node  prototype of the Effect to be inserted when building.
   */
  ProcPatt& 
  ProcPatt::attach(Symbol where, PProc& node)
  {
    DoAttach *last (0);
    if ( !isnil (instructions_) 
       && (last = boost::get<DoAttach> (&(instructions_.back())))
       && last->point==where
       )
       // instead of adding a new build instruct entry, 
      //  we can extend the list in the last "DoAttach" entry.
      last->nodes.push_back(node);
    else
      {
        DoAttach entry(node, where);
        BuildInstruct instruction(entry);
        instructions_.push_back(instruction);
      }
    TODO ("declare dependency??");
    return *this;
  }
  
  
  /** extend the processing instructions by reference to another
   *  ProcPatt, which will be "executed" at this point while building.
   *  This allowes for using simple PorcPatt instances as building blocks
   *  to define more complicated patterns.
   */
  ProcPatt& 
  ProcPatt::operator+= (PProcPatt& toReuse)
  {
    DoRecurse entry(toReuse);
    instructions_.push_back(BuildInstruct (entry));
    TODO ("declare dependency??");
    
    return *this;
  }
  
  


}} // namespace asset
