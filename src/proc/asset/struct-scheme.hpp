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


#include "lib/symbol.hpp"

#include <cstdlib>

/////////////////////////////////////////////////////////TICKET #166 : needs to be pushed down into a *.cpp
#include <boost/format.hpp>
using boost::format;


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
    
    using lib::Symbol;
    
    
    
    /* ==== structural asset ID scheme ==== */  /////////////////////////////////////////////TICKET #565 : better organisation of this naming scheme
    
    template<class STRU>
    struct StructTraits
      {
        static Symbol namePrefix();
        static Symbol catFolder();
        static Symbol idSymbol();
      };
    
    
  ///////////////////////////////////////////////////////////////////////////////////////////TICKET #581 intending to abandon asset::Track in favour of a plain EntryID 
    template<> struct StructTraits<asset::Track>
      {
        static Symbol namePrefix() { return "track"; }
        static Symbol catFolder()  { return "tracks";}
        static Symbol idSymbol()   { return "track"; }
      };
    template<> struct StructTraits<mobject::session::Track>
      {
        static Symbol namePrefix() { return "track"; }
        static Symbol catFolder()  { return "tracks";}
        static Symbol idSymbol()   { return "track"; }
      };
    template<> struct StructTraits<mobject::session::Clip>
      {
        static Symbol namePrefix() { return "clip"; }
        static Symbol catFolder()  { return "clips";}
        static Symbol idSymbol()   { return "clip"; }
      };
    template<> struct StructTraits<Pipe>
      {
        static Symbol namePrefix() { return "pipe"; }
        static Symbol catFolder()  { return "pipes";}
        static Symbol idSymbol()   { return "pipe"; }
      };
    template<> struct StructTraits<const ProcPatt>
      {
        static Symbol namePrefix() { return "patt";           }
        static Symbol catFolder()  { return "build-templates";}
        static Symbol idSymbol()   { return "procPatt";       }
      };
    template<> struct StructTraits<Timeline>
      {
        static Symbol namePrefix() { return "tL";       }
        static Symbol catFolder()  { return "timelines";}
        static Symbol idSymbol()   { return "timeline"; }
      };
    template<> struct StructTraits<Sequence>
      {
        static Symbol namePrefix() { return "seq";      }
        static Symbol catFolder()  { return "sequences";}
        static Symbol idSymbol()   { return "sequence"; }
      };
    
    
    /* catch-all defaults */
    template<class X>
    Symbol StructTraits<X>::idSymbol() { return typeid(X).name(); } ////////////////////TICKET #583   this default works but is ugly
    template<class X>
    Symbol StructTraits<X>::catFolder(){ return idSymbol(); }
    template<class X>
    Symbol StructTraits<X>::namePrefix(){return idSymbol(); }
    
    
    
        
    template<class STRU>
    inline string
    generateSymbolID()
    {
        static uint i=0;
        static format namePattern ("%s.%03d");
        ////////////////////////////////////////////////////////////////////////////////TICKET #166 : needs to be pushed down into a *.cpp
        
        return str(namePattern % StructTraits<STRU>::namePrefix() % (++i) );
    }
    
    
}} // namespace asset::idi
#endif
