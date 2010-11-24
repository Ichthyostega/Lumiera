/*
  OUTPUT-MAPPING.hpp  -  generic interface for translation of output designations
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#ifndef PROC_MOBJECT_OUTPUT_MAPPING_H
#define PROC_MOBJECT_OUTPUT_MAPPING_H

#include "proc/mobject/output-designation.hpp"
#include "lib/meta/function.hpp"



namespace mobject {

  namespace { // Helper to extract and rebind definition types
    
    template<class DEF>
    struct _def
      {
        typedef asset::ID<asset::Pipe>      PId;
        
        template<typename RET>
        RET extractFunctionSignature (RET(DEF::*func)(PId));
        
        template<typename FUN>
        struct Rebind;
        
        template<typename RET>
        struct Rebind<RET(DEF::*)(PId)>
          {
            typedef RET Res;
          };
        
        
        typedef typename Rebind<&DEF::output> Rebinder;
        
        typedef typename Rebinder::Res      Res;
        typedef typename function<Res(PId)> OutputMappingFunc; 
      };
    
  }
  
  
  /**
   * OutputMapping is a facility to resolve output designations.
   * For a given specification, resolution to the desired
   * target specification may be derived. Here, the 
   * type of the target specification is defined 
   * through the type parameter.
   * 
   * This is an Interface, intended to be used in the signature
   * of API functions either providing or requiring a Mapping.
   */
  template<class DEF>
  class OutputMapping
    {
    public:
    };
  
  
  
} // namespace mobject
#endif
