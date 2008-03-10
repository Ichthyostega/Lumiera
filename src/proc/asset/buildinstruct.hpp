/*
  BUILDINSTRUCT.hpp  -  Instructions for building some configuration of render nodes.
 
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


/** @file buildinstruct.hpp
 ** Helper classes used by asset::ProcPatt to represent the processing informations.
 ** Consider these classes as owned by ProcPatt. Non-inline functions go to procpatt.cpp 
 **
 */


#ifndef ASSET_BUILDINSTRUCT_H
#define ASSET_BUILDINSTRUCT_H


#include <boost/variant.hpp>
#include <string>

using std::string;



namespace asset
  {
  
  class Proc;
  class ProcPatt;

  typedef shared_ptr<const asset::Proc> PProc;
  typedef shared_ptr<const asset::ProcPatt> PProcPatt;

  
  static Symbol CURRENT = "current";
  
  struct DoAttach
    {
      vector<PProc> nodes;

      /** identifying the point where the nodes should be attached */
      string point;
      
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
      BuildInstruct (T& instr) : InstructEntry(instr) {}
    };
    
    
    
    
    
} // namespace asset
#endif
