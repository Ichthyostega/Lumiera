/*
  STRUCT-SCHEME.hpp  -  naming and designation scheme for structural assets

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
#include "steam/asset.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/meta/util.hpp"
#include "lib/format-string.hpp"

#include <cstdlib>



namespace steam {
  struct StreamType;
  
namespace mobject {
namespace session {
  
  class Fork;
  class Clip;
  
}}

namespace asset{
  
  class Pipe;
  class ProcPatt;
  class Timeline;
  class Sequence;
  
  namespace meta {
    class TimeGrid;
  }
  
  
  namespace idi  {
    
    using lib::Symbol;
    
    
    
    /* ==== structural asset ID scheme ==== */  /////////////////////////////////////////////TICKET #565 : better organisation of this naming scheme
    
    template<class STRU>
    struct StructTraits
      {
        static Symbol namePrefix();
        static Symbol catFolder();
        static Symbol idSymbol();   ///< used as type predicate symbol
      };
    
    
    template<> struct StructTraits<mobject::session::Fork>
      {
        static Symbol namePrefix() { return "fork"; }
        static Symbol catFolder()  { return "forks";}
        static Symbol idSymbol()   { return "fork"; }
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
    template<> struct StructTraits<steam::StreamType>
      {
        static Symbol namePrefix() { return "type"; }
        static Symbol catFolder()  { return "stream-types";}
        static Symbol idSymbol()   { return "stype"; }
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
    template<> struct StructTraits<meta::TimeGrid>
      {
        static Symbol namePrefix() { return "grid";     }
        static Symbol catFolder()  { return "time-scales";}
        static Symbol idSymbol()   { return "timeGrid"; }
      };
    
    
    /* catch-all defaults */
    template<class X>
    Symbol StructTraits<X>::idSymbol() { return lib::meta::typeSymbol<X>(); }
    template<class X>
    Symbol StructTraits<X>::catFolder(){ return idSymbol(); }
    template<class X>
    Symbol StructTraits<X>::namePrefix(){return idSymbol(); }
    
    
    
    /** generate an Asset identification tuple
     *  based on this EntryID's symbolic ID and type information.
     *  The remaining fields are filled in with hardwired defaults.
     * @note there is a twist, as this asset identity tuple generates
     *       a different hash as the EntryID. It would be desirable
     *       to make those two addressing systems interchangeable. ////////////////////////////////TICKET #739 : make identification schemes compatible
     */
    template<typename TY>
    inline Asset::Ident
    getAssetIdent (lib::idi::EntryID<TY> const& entryID, asset::Kind assetKind =STRUCT)
    {                                                    //////////////////////////////////////////TICKET #1156 : do we need the distinction between STRUCT and META?
      Category cat (assetKind, idi::StructTraits<TY>::catFolder());
      return Asset::Ident (entryID.getSym(), cat);
    }
    
    
    
}}} // namespace asset::idi
#endif
