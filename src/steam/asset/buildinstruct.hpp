/*
  BUILDINSTRUCT.hpp  -  Instructions for building some configuration of render nodes.

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


/** @file buildinstruct.hpp
 ** Helper classes used by asset::ProcPatt to represent the processing information.
 ** Consider these classes as owned by ProcPatt. Non-inline functions go to procpatt.cpp 
 **
 */


#ifndef STEAM_ASSET_BUILDINSTRUCT_H
#define STEAM_ASSET_BUILDINSTRUCT_H


#include "lib/symbol.hpp"

#include <boost/variant.hpp>
#include <string>

using std::string;



namespace proc {
namespace asset {

  using lib::Symbol;
  using lib::Literal;
  
  class Proc;
  class ProcPatt;

  using PProc     = lib::P<const asset::Proc>;
  using PProcPatt = lib::P<const asset::ProcPatt>;

  
  static Symbol CURRENT = "current";
  
  struct DoAttach
    {
      vector<PProc> nodes;

      /** identifying the point where the nodes should be attached */
      Literal point;
      
      DoAttach (Symbol where = CURRENT) 
        : point(where) 
        { }
      
      DoAttach (PProc& node, Symbol where = CURRENT) 
        : point(where) 
        { 
          nodes.push_back(node); 
        }
      
    };
    
    
  struct DoRecurse
    {
      PProcPatt subPattern_;

      explicit DoRecurse (PProcPatt& pattern) : subPattern_(pattern) {}
    };
    
    
    
  class DoConditional
    {
      // How to do this? we need some context to test the condition...
    };

    
  typedef boost::variant< DoAttach, DoRecurse, DoConditional > InstructEntry;
    
  /**
   * (Interface) building instructions to be executed by the Builder
   * on the render node network under construction. The purpose of this
   * "micro language" is to be able to store in the configuration or session
   * how certain parts of the model should be assembled. One important example
   * is how to build a source reading chain to read and decode frames from a
   * media file. Another example being a global audio Pipe, comprised of an
   * EQ plugin, a fader and a panner.
   * \par
   * Build instructions are tightliy coupled to asset::ProcPatt and always
   * created from there.
   * @see ProcPatt::attach
   * @see ProcPatt::operator+=  
   * 
   */
  struct BuildInstruct
    : public InstructEntry 
    {
      template<typename T>
      BuildInstruct (T& instr) : InstructEntry() {}

      // TODO: this ctor is *not* correct, just to make it compile
      // There is a strange problem with boost::variant, probably because the
      // template parameter T could be anything (but actually we know it's one
      // of our Instruction types.
      // I have to re-investigate this design anyway, and probably will replace
      // the boost::variant by something else, derive from a common base or such.
      // Note: as of 8/2008 ProcPatt is just a draft and not implemented.
      
      // Note: 9/2013 : meanwhile the obvious solution would be to use our "polymorphic value",
      //                which was invented exactly to solve this notorious design mismatch in C++
    };
    
    
    
    
    
}} // namespace proc::asset
#endif
