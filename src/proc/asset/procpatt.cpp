/*
  ProcPatt  -  template for building some render processing network
 
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
 
* *****************************************************/


#include "proc/asset/procpatt.hpp"
#include "proc/asset/buildinstruct.hpp"
#include "proc/assetmanager.hpp"

namespace asset
  {
  
  namespace // ProcPattern implementation details
    {
      /** @internal derive a sensible asset ident tuple when creating 
       *  a processing pattern asset based on a query
       *  @todo define the actual naming scheme of struct assets
       */
      const Asset::Ident
      createPatternIdent (const string& properties)
        {
          string name ("pattern-" + properties);  // TODO something sensible here; append number, sanitize etc.
          TODO ("Implement ProcPatt name scheme!!");
          Category category (STRUCT,"patterns");
          return Asset::Ident (name, category );
        }
    } 
  
  /** */
  ProcPatt::ProcPatt (const string& properties, const vector<BuildInstruct>& instr)
    : Struct (createPatternIdent (properties)),
      propDescriptor_ (properties),
      instructions (instr)
  {
    TODO ("verify building instructions, maybe preprocess...");
  }

  
  /** query the currently defined properties of this
      processing pattern for a stream-ID predicate */
  const string& 
  ProcPatt::queryStreamID()  const
  {
    TODO ("really implement querying the properties");
    return propDescriptor_; /////////////////////////////TODO grober Unfug
  }
  
  
  /** create a new ProcPatt asset as a literal copy
   *  of this one. The new ProcPatt can then be customized
   *  independently of the original one. This allows using
   *  some ProcPatt as a template for creatind more
   *  spezialized patterns. 
   */
  shared_ptr<ProcPatt> 
  ProcPatt::newCopy (string newID)  const
  {
    TODO ("implement the Pattern-ID within the propDescriptor!");
    ProcPatt* pP = new ProcPatt (this->propDescriptor_, this->instructions);
    return AssetManager::instance().wrap (*pP);
  }



} // namespace asset
