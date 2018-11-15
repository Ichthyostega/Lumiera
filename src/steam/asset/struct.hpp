/*
  STRUCT.hpp  -  key abstraction: structural asset

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


/** @file struct.hpp
 ** Asset representation of structural elements within the model.
 ** Structural parts of the Session (e.g. "tracks", i.e. Fork objects) can be
 ** reflected into the "bookkeeping view" as a specific Kind of Asset.
 ** For the different _kinds of Assets_, we use sub-interfaces inheriting
 ** from the general Asset interface, each of which expose a distinguishing feature.
 ** In the case of structural assets, the key point is the ability to retrieve an
 ** instance based on a capabilities query; structural assets are typically created
 ** on demand, just by referral. Thus, the collection of these assets provides a map
 ** for exploring the current session's structure and allow for tweaking of the
 ** default behaviour.
 ** - Timeline and Sequence are facades, part of the session API
 ** - Pipe is an attachment point for wiring connections and defines a StreamType
 ** - "tracks" are implemented as Fork represented as \c entryID within the asset view.
 ** - ProcPatt is used as a blueprint in the build process, a standard connection pattern
 ** 
 ** ## access and creation
 ** 
 ** asset::Struct instances are created on demand; the interface is to invoke the
 ** StructFactory with a (typed) Query describing properties or capabilities.
 ** In case this query succeeds, an existing asset will be returned, otherwise
 ** a suitable new instance is created automatically. Typically, structural assets
 ** aren't deleted. Doing so would require a dedicated function which not only drops
 ** an asset instance from AssetManager, but also ensures removal of all properties
 ** within the model which could cause automatic re-creation of this asset. E.g.
 ** purging a track asset (=unique trackID) would necessitate to remove or
 ** disconnect all placements located within the scope of the referred Fork;
 ** this could be sub forks, clips, effects, automation or labels.
 ** Obviously, this is way beyond the realm of asset management.
 ** 
 ** @ingroup asset
 ** @see asset.hpp for explanation regarding asset IDs
 ** @see StructFactory creating concrete asset::Struct instances
 **
 */


#ifndef ASSET_STRUCT_H
#define ASSET_STRUCT_H

#include "lib/symbol.hpp"
#include "proc/asset.hpp"
#include "common/query.hpp"
#include "lib/nocopy.hpp"

#include <memory>
#include <string>



namespace proc {
namespace asset {
  
  using std::string;
  using std::unique_ptr;
  using lumiera::Query;
  using lib::Symbol;
  
  class Struct;
  class StructFactory;
  class StructFactoryImpl;
  class Pipe;
  
  
  template<>
  class ID<Struct> : public ID<Asset>
    {
    public:
      ID (HashVal id);
      ID (const Struct&);
    };
  
  
  
  /**
   * key abstraction: structural asset
   * Created automatically as a sideeffect of building the structure
   * of the high-level-model (session contents), thus providing IDs
   * for later referral, search and attachment of metadata.
   * 
   * Examples being tracks, sequences, timelines, pipes, processing patterns
   * @note embedded access point to instance creation or retrieval
   *       through the static field #retrieve
   * @todo the actual meaning of a "structural asset" needs to be sharpened and evolved.
   *       The idea is to have a generic mechanism for attaching properties and relations.
   *       This will become relevant once we build the real query subsystem.
   *       Right now (as of 1/2015), asset::Struct is just an ID provider.
   *                                                             //////////////////////////////////TICKET #1156 : do we need the distinction between STRUCT and META?
   */
  class Struct : public Asset
    {
    public:
      static StructFactory retrieve;
      
      virtual const ID<Struct>&
      getID()  const             ///< @return ID of kind asset::Struct 
        { 
          return static_cast<const ID<Struct>& > (Asset::getID()); 
        }
      
      
      
    protected:
      Struct (const Asset::Ident& idi) : Asset(idi) {}
    };
    
    
    // definition of ID<Struct> ctors is possible now,
   //  after providing full definition of class Struct
  
  inline ID<Struct>::ID(HashVal id)         : ID<Asset> (id)           {};
  inline ID<Struct>::ID(const Struct& stru) : ID<Asset> (stru.getID()) {};
  
  
  
  
  
  
  /** 
   * Factory specialised for creating Structural Asset objects.
   */ 
  class StructFactory
    : util::NonCopyable
    {
      unique_ptr<StructFactoryImpl> impl_;
      
    protected:
      StructFactory ();
      friend class Struct;
      
      
    public:
      template<class STRU>
      lib::P<STRU> operator() (Query<STRU> const& query);
      
//    lib::P<Timeline> operator() (MORef<Binding>);  ///////////TODO doesn't this create circular includes?? Any better idea how to refer to an existing binding?
      
      template<class STRU>
      lib::P<STRU> newInstance (Symbol nameID  ="");
      
      template<class STRU>
      lib::P<STRU> made4fake  (Query<STRU> const& query);  ///< @warning to be removed in Alpha when using a real resolution engine /////TICKET #710
      
      lib::P<Pipe> newPipe (string pipeID, string streamID);
    };
  
  
  
}} // namespace proc::asset
#endif
