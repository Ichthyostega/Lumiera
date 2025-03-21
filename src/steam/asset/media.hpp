/*
  MEDIA.hpp  -  key abstraction: media-like assets

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file media.hpp
 ** Media data represented a specific kind of Asset.
 ** For the different <i>kinds</i> of Assets, we use sub-interfaces inheriting
 ** from the general Asset interface. To be able to get asset::Media instances
 ** directly from the AssetManager, we define a specialisation of the Asset ID.
 ** 
 ** @ingroup asset
 ** @see asset.hpp for explanation
 ** @see MediaFactory creating concrete asset::Media instances
 **
 */

#ifndef ASSET_MEDIA_H
#define ASSET_MEDIA_H

#include "steam/asset.hpp"
#include "lib/time/timevalue.hpp"
#include "steam/mobject/mobject.hpp"
#include "steam/mobject/session/clip.hpp"
#include "lib/nocopy.hpp"




namespace steam {
namespace asset {
  
  class Clip;
  class Media;
  class MediaFactory;
  class ProcPatt;
  
  using lib::time::Duration;
  
  
  template<>
  class ID<Media> : public ID<Asset>
    {
    public:
      ID (HashVal id);
      ID (const Media&);
    };

    
    
  /**
   * key abstraction: media-like assets
   */
  class Media : public Asset
    {
      string filename_;
      const Duration len_;
      
    public:
      using PMedia = lib::P<Media>;
      using PClipAsset  = lib::P<steam::asset::Clip>;
      using PProcPatt = lib::P<steam::asset::ProcPatt>;
      using PClip = mobject::Placement<mobject::session::Clip>;
     
      
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
       *  the Session from a given Media or Clip Asset. As a sideeffect,
       *  a corresponding asset::Clip is created as well if necessary. 
       *  It is OK to use and throw away the returned Clip-MO, because
       *  it can be regenerated from the corresponding asset::Clip
       *  @return a Placement smart ptr owning the new Clip MObject 
       */
      PClip createClip ();
      
      /** @return the overall length of the media represented by this asset */ 
      virtual Duration getLength ()  const;
      
      
    protected:
      Media (const Asset::Ident& idi, const string& file, Duration length) 
        : Asset(idi), filename_(file), len_(length) {}
      friend class MediaFactory;
      
      /** get or create the correct asset::Clip 
       *  corresponding to this media */
      virtual PClipAsset getClipAsset ();
      
      /** predicate to decide if this asset::Media
       *  is part of a compound (multichannel) media.
       *  @return pointer to parent, or `null`
       */
      virtual PMedia checkCompound ()  const;
      friend class steam::asset::Clip;    ////////////////////////TODO better interface!!!

    };
    
    
    // definition of ID<Media> ctors is possible now,
   //  after providing full definition of class Media

  inline ID<Media>::ID(HashVal id)         : ID<Asset> (id)            {};
  inline ID<Media>::ID(const Media& media) : ID<Asset> (media.getID()) {};
  
  
  
  
  /** 
   * Factory specialised for creating Media Asset objects.
   */ 
  class MediaFactory 
    : util::NonCopyable
    {
    public:
      typedef lib::P<Media> PType;
      
      PType operator() (Asset::Ident& key, const string& file="");
      PType operator() (const string& file, const Category& cat);
      PType operator() (const string& file, asset::Kind);
      
      PType operator() (Asset::Ident& key, const char* file);  ///< convenience overload using C-String
      PType operator() (const char* file, const Category& cat);
      PType operator() (const char* file, asset::Kind);
      
      lib::P<Clip>
      operator() (Media& mediaref);

    };

  LUMIERA_ERROR_DECLARE (PART_OF_COMPOUND);
    
    
    
    
}} // namespace steam::asset
#endif
