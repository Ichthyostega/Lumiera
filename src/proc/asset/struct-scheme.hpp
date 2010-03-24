/*
  STRUCT-SCHEME.hpp  -  naming and designation scheme for structural assets
 
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


/** @file struct-scheme.hpp
 ** Naming and labelling scheme for structural assets.
 ** Preconfigured traits data for the relevant types encountered in
 ** Lumiera's session data model.
 **
 ** @see struct-factory-impl.hpp
 ** @see entry-id.hpp
 **
 */


#ifndef ASSET_STRUCT_SCHEME_H
#define ASSET_STRUCT_SCHEME_H


//#include "proc/mobject/session.hpp"
//#include "proc/mobject/mobject.hpp"

#include "lib/symbol.hpp"
//#include "lib/error.hpp"
//#include "lib/util.hpp"

//#include <boost/format.hpp>

#include <boost/format.hpp>
using boost::format;
/////////////////////////////////////////////////////////TODO needs to be pushed down into a *.cpp

//using mobject::Session;
//using mobject::MObject;

using lib::Symbol;
//using util::isnil;
//using util::contains;
//using asset::Query;
//using lumiera::query::LUMIERA_ERROR_CAPABILITY_QUERY;
//using lumiera::query::extractID;

namespace asset{
  
  class Track;
  class Pipe;
  class ProcPatt;
  class Timeline;
  class Sequence;
  
  namespace idi  {
  
    // structural asset ID scheme   ///////////////////////////////////////////////////////////TICKET #565
    
    template<class STRU>
    struct StructTraits
      {
        static Symbol namePrefix;
        static Symbol catFolder;
        static Symbol idSymbol;
      };
    
    template<> Symbol Traits<Track>::namePrefix = "track";
    template<> Symbol Traits<Track>::catFolder  = "tracks";
    template<> Symbol Traits<Track>::idSymbol   = "track";
    
    template<> Symbol Traits<Pipe>::namePrefix = "pipe";
    template<> Symbol Traits<Pipe>::catFolder  = "pipes";
    template<> Symbol Traits<Pipe>::idSymbol   = "pipe";
    
    template<> Symbol Traits<const ProcPatt>::namePrefix = "patt";
    template<> Symbol Traits<const ProcPatt>::catFolder  = "build-templates";
    template<> Symbol Traits<const ProcPatt>::idSymbol   = "procPatt";
    
    template<> Symbol Traits<Timeline>::namePrefix = "tL";
    template<> Symbol Traits<Timeline>::catFolder  = "timelines";
    template<> Symbol Traits<Timeline>::idSymbol   = "timeline";
    
    template<> Symbol Traits<Sequence>::namePrefix = "seq";
    template<> Symbol Traits<Sequence>::catFolder  = "sequences";
    template<> Symbol Traits<Sequence>::idSymbol   = "sequence";
    
    
    
        
    template<class STRU>
    inline string
    generateSymbolID()
    {
        static uint i=0;
        static format namePattern ("%s.%02d");
        /////////////////////////////////////////////////////////TODO needs to be pushed down into a *.cpp
        
        return str(namePattern % StructTraits<STRU>::namePrefix % (++i) );
    }
    
    
}} // namespace asset::idi
#endif
