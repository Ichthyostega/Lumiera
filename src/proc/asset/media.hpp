/*
  MEDIA.hpp  -  key abstraction: media-like assets
 
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


/** @file media.hpp
 ** Media data is a specific kind of Asset.
 ** For the different <i>kinds</i> of Assets, we use sub-intefaces inheriting
 ** from the general Asset interface. To be able to get asset::Media instances
 ** directly from the AssetManager, we define a specialization of the Asset ID.
 **
 ** @see asset.hpp for explanation
 ** @see MediaFactory creating concrete asset::Media instances
 **
 */

#ifndef ASSET_MEDIA_H
#define ASSET_MEDIA_H

#include "proc/asset.hpp"
#include "common/factory.hpp"
#include "proc/mobject/mobject.hpp"
#include "proc/mobject/session/clip.hpp"



namespace asset
  {
  
  class Clip;
  class Media;
  class MediaFactory;
  class ProcPatt;
  
  using lumiera::Time;
  
  
  template<>
  class ID<Media> : public ID<Asset>
    {
    public:
      ID (size_t id);
      ID (const Media&);
    };

    
    
  /**
   * key abstraction: media-like assets
   */
  class Media : public Asset
    {
      string filename_;
      const Time len_;
      
    public:
      typedef shared_ptr<Media> PMedia;
      typedef shared_ptr<asset::Clip> PClip;
      typedef shared_ptr<asset::ProcPatt> PProcPatt;
      typedef mobject::session::PClipMO PClipMO;
     
      
      static MediaFactory create;
      const string& getFilename ()  const { return filename_; }
      
      virtual const ID<Media>& getID()  const    ///< @return ID of kind Media 
        { 
          return static_cast<const ID<Media>& > (Asset::getID()); 
        }
      
      /** Service Access Point for getting a processing template
       *  describing how to build the render nodes network 
       *  necessary for this Media or Clip. This includes
       *  Codecs and postprocessing (stretching, deinterlacing...)
       */
      PProcPatt howtoProc ()  const;
      
      /** Service Access Point for creating a Clip entity usable within 
       *  the EDL/Session from a given Media or Clip Asset. As a sideeffect,
       *  a corresponding asset::Clip is created as well if necessary. 
       *  It is OK to use and throw away the returned Clip-MO, because
       *  it can be regenerated from the corresponding asset::Clip
       *  @return a Placement smart ptr owning the new Clip MObject 
       */
      PClipMO createClip ();
      
      /** @return the overall length of the media represented by this asset */ 
      virtual Time getLength ()  const;
      
      
    protected:
      Media (const Asset::Ident& idi, const string& file, Time length) 
        : Asset(idi), filename_(file), len_(length) {}
      friend class MediaFactory;
      
      /** get or create the correct asset::Clip 
       *  corresponding to this media */
      virtual PClip getClipAsset ();
      
      /** predicate to decide if this asset::Media
       *  is part of a compound (multichannel) media.
       *  @return pointer to parent, or \code null
       */
      virtual PMedia checkCompound ()  const;
      friend class asset::Clip;    ////////////////////////TODO better interface!!!

    };
    
    
    // definition of ID<Media> ctors is possible now,
   //  after providing full definition of class Media

  inline ID<Media>::ID(size_t id)          : ID<Asset> (id)            {};
  inline ID<Media>::ID(const Media& media) : ID<Asset> (media.getID()) {};
  
  
  
  
  /** 
   * Factory specialized for creating Media Asset objects.
   */ 
  class MediaFactory : public lumiera::Factory<asset::Media>
    {
    public:
      typedef shared_ptr<asset::Media> PType;
      
      PType operator() (Asset::Ident& key, const string& file="");
      PType operator() (const string& file, const Category& cat);
      PType operator() (const string& file, asset::Kind);
      
      PType operator() (Asset::Ident& key, const char* file);  ///< convienience overload using C-String
      PType operator() (const char* file, const Category& cat);
      PType operator() (const char* file, asset::Kind);
      
      shared_ptr<asset::Clip>
      operator() (asset::Media& mediaref)  throw(lumiera::error::Invalid);

    };

  LUMIERA_ERROR_DECLARE (PART_OF_COMPOUND);
    
    
    
    
} // namespace asset
#endif
