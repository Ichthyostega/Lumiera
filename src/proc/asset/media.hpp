/*
  MEDIA.hpp  -  key abstraction: media-like assets
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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
 ** Media data is a specific Kind of Asset.
 ** For the different <i>Kinds</i> of Assets, we use sub-intefaces inheriting
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



namespace asset
  {
  
  class Media;
  class MediaFactory;
  
  
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
      
    public:
      static MediaFactory create;
      const string& getFilename ()  const { return filename_; }
      
      virtual const ID<Media>& getID()  const    ///< @return ID of kind Media 
        { 
          return static_cast<const ID<Media>& > (Asset::getID()); 
        }
      
    protected:
      Media (const Asset::Ident& idi, const string& file) : Asset(idi), filename_(file) {}
      friend class MediaFactory;
    };
    
    
    // definition of ID<Media> ctors is possible now,
   //  after providing full definition of class Media

  inline ID<Media>::ID(size_t id)          : ID<Asset> (id)            {};
  inline ID<Media>::ID(const Media& media) : ID<Asset> (media.getID()) {};
  
  
  
  
  /** 
   * Factory specialized for createing Media Asset objects.
   */ 
  class MediaFactory : public cinelerra::Factory<asset::Media>
    {
    public:
      typedef shared_ptr<asset::Media> PType;
      
      PType operator() (Asset::Ident& key, const string& file="");
      PType operator() (const string& file, const Category& cat);
      PType operator() (const string& file, asset::Kind);
      
      PType operator() (Asset::Ident& key, const char* file);  ///< convienience overload using C-String
      PType operator() (const char* file, const Category& cat);
      PType operator() (const char* file, asset::Kind);

    };

    
    
    
} // namespace asset
#endif
