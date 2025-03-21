/*
  PROCPATT.hpp  -  template for building some render processing network

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file procpatt.hpp
 ** Definition of a [structural asset](\ref struct.hpp) to express patterns of wiring or processing
 ** Processing patterns can be seen as some kind of "micro code" (or macro code, if you prefer this
 ** view angle), which will be executed within the machinery to wire and configure the render nodes
 ** network, based on the objects in the Session model.
 */


#ifndef ASSET_PROCPATT_H
#define ASSET_PROCPATT_H

#include "steam/asset/struct.hpp"

#include <vector>

using std::vector;



namespace steam {
namespace asset {
  
  using lib::Symbol;
  
  class Proc;
  class ProcPatt;
  struct BuildInstruct;
  typedef lib::P<const asset::Proc> PProc;
  typedef lib::P<const asset::ProcPatt> PProcPatt;
  
  typedef vector<BuildInstruct> InstructionSequence;
  
  /**
   * "Processing Pattern" is a structural Asset 
   * representing information how to build some part 
   * of the render engine's processing nodes network.
   */
  class ProcPatt
    : public Struct
    {
      InstructionSequence instructions_;
      
     ~ProcPatt();
      
      ProcPatt (const Asset::Ident&, const InstructionSequence&);
      
    protected:
      explicit ProcPatt (const Asset::Ident& idi);
      friend class StructFactoryImpl;
      
    public:
      lib::P<ProcPatt> newCopy (string newID) const;
      
      ProcPatt& attach (Symbol where, PProc& node);
      ProcPatt& operator+= (PProcPatt& toReuse);
      
    };
  
  
  
  
}} // namespace steam::asset
#endif
