/*
  NODEWIRINGCONFIG.hpp  -  Helper for representing and selecting the wiring case
 
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


#ifndef ENGINE_NODEWIRINGCONFIG_H
#define ENGINE_NODEWIRINGCONFIG_H



//#include <cstddef>
#include <vector>
#include <bitset>
#include <boost/scoped_ptr.hpp>



namespace engine {


  class WiringFactory;
  
  enum Cases
    { 
      CACHING,
      PROCESS,
      INPLACE,
      
      NUM_Cases
    };
  typedef std::bitset<NUM_Cases> Bits;
  
  Bits
  getCaseRepresentation (bool caching, bool process, bool inplace)
    {
      Bits repr_as_bits;
      repr_as_bits.set (CACHING, caching)
                  .set (PROCESS, process)
                  .set (INPLACE, inplace);
      return repr_as_bits;
    }
  
  const ulong numCases (Bits().flip().to_ulong());
  std::vector<Bits> possibleConfigs;
  
  
  void
  registerPossibleCases(bool (*possible)(Bits&) )
  {
    ulong numCases (Bits().flip().to_ulong());
    for (ulong combi=0; combi<numCases; ++i)
      if (possible (combi))
        possibleConfigs.push_back (Bits (combi));
  }
  
  /**
   * Helper type used to describe the \em situation on which to base
   * the selection of the wiring and configuration of a ProcNode.
   * It is based on a tuple of elementary parameters (here of type bool);
   * a combination of these describes the situation and is used to select
   * a strategy, which in turn determines what concret operations to use
   * while pulling the node, and how to combine these operations.
   * @see engine::Strategy ///TODO namespace.... 
   */
  template<bool caching, bool process, bool inplace>
  struct Config
    {
      static Bits getRepr () { return getCaseRepresentation(caching,process,inplace); }
    };
  
  
  
  template<class RET, template<class CONF> class Factory>
  class ConfigSelector
    {
      struct FacFunctor
        {
          virtual ~FacFunctor() {}
          virtual RET operator() ()  =0;
        };
      
      typedef boost::scoped_ptr<FacFunctor> PFunc;
      typedef std::map<Bits, PFunc> ConfigTable;
      
      ConfigTable possibleConfig_;
      
      void
      createFactory (Bits config)
        {
          this->possibleConfig_[config].reset (new Factory);    ////////////TODO: wie den richtigen Config-Typ bekommen??
        }
      
    public:
      configSelector()
        {
          for_each (possibleConfigs, bind(&createFactory, this, _1));
        }
    };
  

  
} // namespace engine
#endif
