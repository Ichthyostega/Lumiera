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

#include <cstdlib>

//using mobject::Session;
//using mobject::MObject;

using lib::Symbol;
//using util::isnil;
//using util::contains;
//using asset::Query;
//using lumiera::query::LUMIERA_ERROR_CAPABILITY_QUERY;
//using lumiera::query::extractID;

namespace mobject {
namespace session {
  
  class Track;
  class Clip;
  
}}

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
    
    ///////////////////////////////////////////////////////////////////////////////////////////TICKET #581 intending to abandon asset::Track in favour of a plain EntryID 
    template<> Symbol StructTraits<asset::Track>::namePrefix = "track";
    template<> Symbol StructTraits<asset::Track>::catFolder  = "tracks";
    template<> Symbol StructTraits<asset::Track>::idSymbol   = "track";
    
    template<> Symbol StructTraits<mobject::session::Track>::namePrefix = "track";
    template<> Symbol StructTraits<mobject::session::Track>::catFolder  = "tracks";
    template<> Symbol StructTraits<mobject::session::Track>::idSymbol   = "track";
    
    template<> Symbol StructTraits<mobject::session::Clip>::namePrefix = "clip";
    template<> Symbol StructTraits<mobject::session::Clip>::catFolder  = "clips";
    template<> Symbol StructTraits<mobject::session::Clip>::idSymbol   = "clip";
    
    template<> Symbol StructTraits<Pipe>::namePrefix = "pipe";
    template<> Symbol StructTraits<Pipe>::catFolder  = "pipes";
    template<> Symbol StructTraits<Pipe>::idSymbol   = "pipe";
    
    template<> Symbol StructTraits<const ProcPatt>::namePrefix = "patt";
    template<> Symbol StructTraits<const ProcPatt>::catFolder  = "build-templates";
    template<> Symbol StructTraits<const ProcPatt>::idSymbol   = "procPatt";
    
    template<> Symbol StructTraits<Timeline>::namePrefix = "tL";
    template<> Symbol StructTraits<Timeline>::catFolder  = "timelines";
    template<> Symbol StructTraits<Timeline>::idSymbol   = "timeline";
    
    template<> Symbol StructTraits<Sequence>::namePrefix = "seq";
    template<> Symbol StructTraits<Sequence>::catFolder  = "sequences";
    template<> Symbol StructTraits<Sequence>::idSymbol   = "sequence";
    
    /* catch-all defaults */
    template<class X>
    Symbol StructTraits<X>::idSymbol = typeid(X).name(); ////////////////////TICKET #583   this default works but is ugly
    template<class X>
    Symbol StructTraits<X>::catFolder = StructTraits<X>::idSymbol;
    template<class X>
    Symbol StructTraits<X>::namePrefix = StructTraits<X>::idSymbol;
    
    
    
        
    template<class STRU>
    inline string
    generateSymbolID()
    {
        static uint i=0;
        static format namePattern ("%s.%03d");
        /////////////////////////////////////////////////////////TODO needs to be pushed down into a *.cpp
        
        return str(namePattern % StructTraits<STRU>::namePrefix % (++i) );
    }
    
    
}} // namespace asset::idi
#endif
