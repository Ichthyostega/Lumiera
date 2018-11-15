/*
  SEQUENCE.hpp  -  Compound of MObjects placed within a tree of tracks. Building block of the Session

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file asset/sequence.hpp
 ** Structural building block of the session: a sequence of clips.
 ** A Sequence is both a structural element of the session and part of the
 ** public session API exposed to clients for discovering the session contents
 ** and structure. Sequences are implemented as structural assets, delegating
 ** the actual implementation to the fork-MObjects within the model, which
 ** naturally form the necessary recursive and nested structure.
 ** 
 ** Contrary to usual habits in video/sound editing software, in Lumiera the
 ** "tracks" form a tree and serve as backbone of the individual \em sequence,
 ** as opposed to the timeline, which manages output and frame quantisation,
 ** but do not hold any contents themselves. Thus, the sequence is really
 ** a tree of nested scopes (session::mobject::Fork), together with all the
 ** media elements placed within these scopes.
 ** 
 ** Like every structural asset, the creation of sequences happens automatically
 ** on referral; Sequences can be queried from the StructFactory, providing additional
 ** requested capabilities. Commonly clients will retrieve a given sequence by query
 ** on the name-ID of the sequence: \c Struct::retrieve(Query<Sequence> ("id(theName)."))
 ** Additionally, a specific fork root may be requested: \c "id(theName),rootFork(trackID)." 
 ** 
 ** @todo this is a very crucial part of the emerging design of the session model. Unfortunately,
 **       development on that frontier has stalled since 2010, since we focused efforts towards
 **       the player, the engine and the GUI lately (as of 2016)
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
#include "lib/p.hpp"
#include "lib/element-tracker.hpp"

//#include <vector>
//#include <string>

//using std::vector;
//using std::string;

namespace proc {
namespace mobject {
namespace session {
  
  class Fork;
  typedef MORef<Fork> RFork;
}}


namespace asset {
  
  
  class Sequence;
  typedef lib::P<Sequence> PSequence;
   
  
  
  /**
   * TODO type comment
   */
  class Sequence 
    : public Struct
    , public lib::AutoRegistered<Sequence>
    {
      typedef mobject::session::RFork RFork;
      
      Sequence (Ident const&);                //////////////////////////////////////////////TICKET #692  pass in fork here
      
    public:
      /** create and register a new Sequence instance */
      static PSequence create (Asset::Ident const& idi);
      
    protected:
      virtual void unlink ();
      
    };
    
  
  
///////////////////////////TODO currently just fleshing the API
  
  
}} // namespace proc::asset
#endif
