/*
  PROCPATT.hpp  -  template for building some render processing network
 
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


#ifndef ASSET_PROCPATT_H
#define ASSET_PROCPATT_H

#include "common/query.hpp"
#include "proc/asset/struct.hpp"

#include <vector>

using std::vector;



namespace asset
  {
  using cinelerra::Symbol;

  class Proc;
  class ProcPatt;
  class BuildInstruct; 
  typedef shared_ptr<const asset::Proc> PProc;
  typedef shared_ptr<const asset::ProcPatt> PProcPatt;
  
  typedef vector<BuildInstruct> InstructionSequence;    

  /**
   * "Processing Pattern" is a structural Asset 
   * representing information how to build some part 
   * of the render engine's processing nodes network.
   */
  class ProcPatt : public Struct
    {
      InstructionSequence instructions_;
      
      ProcPatt (const Asset::Ident&, const InstructionSequence&);      
      
    protected:
      explicit ProcPatt (const Asset::Ident& idi);
      friend class StructFactoryImpl;
      
    public:
      shared_ptr<ProcPatt> newCopy (string newID) const;
      
      ProcPatt& attach (Symbol where, PProc& node);
      ProcPatt& operator+= (PProcPatt& toReuse);

    };
    
  typedef shared_ptr<const asset::ProcPatt> PProcPatt;
    
    
    
} // namespace asset
#endif
