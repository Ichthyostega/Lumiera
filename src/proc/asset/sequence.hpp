/*
  SEQUENCE.hpp  -  Compound of MObjects placed on a track tree. Building block of the Session
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file sequence.hpp
 ** Structural building block of the session: a sequence of clips.
 ** A Sequence is both a structural element of the session and part of the
 ** public session API exposed to clients for discovering the session contents
 ** and structure. Sequences are implemented as structural assets, delegating
 ** the actual implementation to the track-MObjects within the model. 
 ** 
 ** Contrary to usual habits in video/sound editing software, in Lumiera the
 ** tracks form a tree and serve as backbone of the individual \em sequence,
 ** as opposed to the timeline, which manages output and frame quantisation,
 ** but not the track structure.
 ** 
 ** Like every structural asset, the creation of sequences happens automatically
 ** on referral; Sequences can be queried from the StructFactory, providing additional
 ** requested capabilities. Commonly clients will retrieve a given sequence by query
 ** on the name-ID of the sequence: \c Struct::create(Query<Sequence> ("id(theName)."))
 ** Additionally, a specific root track may be requested: \c "id(theName),rootTrack(trackID)." 
 ** 
 ** @see Session
 ** @see StructFactory
 **
 */


#ifndef ASSET_SEQUENCE_H
#define ASSET_SEQUENCE_H


#include "proc/asset/struct.hpp"
//#include "proc/mobject/mobject.hpp"
//#include "proc/mobject/placement.hpp"
#include "proc/mobject/mobject-ref.hpp"

//#include <vector>
//#include <string>

//using std::vector;
//using std::string;

namespace mobject {
namespace session {
  
  class Track;
  typedef MORef<Track> RTrack;
}}


namespace asset {
  
  
  
  /**
   * TODO type comment
   */
  class Sequence 
    : public Struct
    {
      typedef mobject::session::RTrack RTrack;

    public:
       void detach() { TODO("Session-Sequence registration"); }
     
    protected:
      Sequence (const Asset::Ident& idi);
      friend class StructFactoryImpl;
    };
    
  
  typedef P<Sequence> PSequence;
    
///////////////////////////TODO currently just fleshing the API
  
  
} // namespace asset
#endif
