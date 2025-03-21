/*
  Media(Asset)  -  key abstraction: media-like assets

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file media.cpp
 ** Implementation regarding the Media Asset abstraction
 */


#include "lib/error.hpp"
#include "steam/common.hpp"
#include "steam/assetmanager.hpp"
#include "steam/asset/media.hpp"
#include "steam/asset/clip.hpp"
#include "steam/asset/unknown.hpp"
#include "vault/media-access-facade.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"
#include "include/logging.h"

#include <regex>


using util::_Fmt;
using util::isnil;
using lib::time::FSecs;
using lib::time::Duration;
using backend_interface::MediaDesc;
using backend_interface::MediaAccessFacade;

using std::regex;
using std::smatch;
using std::regex_search;
using std::dynamic_pointer_cast;

namespace error = lumiera::error;

namespace steam {
namespace asset {
  
  namespace { // Implementation details
    
    /** helper: extract a name token out of a given path/filename
     *  @return sanitised token based on the name (minus extension),
     *          empty string if not the common filename pattern.
     */
    string extractName (const string& path)
    {
      static regex PATHNAME_PATTERN("([^/\\.]+)(\\.\\w+)?$");
      smatch match;
      
      if (regex_search (path, match, PATHNAME_PATTERN))
        return util::sanitise (string (match[1]));
      else
        return "";
    }
  } 

  
  
  
  
  Media::PClip
  Media::createClip ()
  {
    PClipAsset clipAsset (getClipAsset());
    PClip      clipMO = clipAsset->createClip();
    
    ENSURE (clipMO->isValid());
    return clipMO;
  }
  
  
  /** @internal used to either create an asset::Clip denoting the whole media,
   *            or to get the right reference to some already existing asset::Clip,
   *            especially when this media is part of a compound (multichannel) media.
   */
  Media::PClipAsset
  Media::getClipAsset ()
  {
    if (PMedia parent = this->checkCompound())
      return parent->getClipAsset();
    else
      return Media::create(*this);
  }
  
  
  Media::PMedia
  Media::checkCompound()  const
  {
    vector<PAsset> parents = this->getParents();
    PMedia parent;
    if ( !isnil (parents))  // primary parent is a media asset?
       parent = dynamic_pointer_cast<Media,Asset> (parents[0]);
    return parent;
  }
  
  
  Media::PProcPatt
  Media::howtoProc ()  const
  {
    UNIMPLEMENTED ("calculate and return processing pattern for media asset");
    PProcPatt ppatt; //TODO:null
    
    ENSURE (ppatt);
    return ppatt;
  }
  
  
  Duration
  Media::getLength()  const
  {
     return len_;
  }
  
  
  
  MediaFactory Media::create;  ///< storage for the static MediaFactory instance
  
  
  
  /** Factory method for Media Asset instances. Depending on the filename given,
   *  either a asset::Media object or an "Unknown" placeholder will be provided. If
   *  the given Category already contains an "Unkown", we just get the
   *  corresponding smart-ptr. Otherwise a new asset::Unknown is created.
   *  @throw  error::Invalid when media file is inaccessible or inappropriate
   *  @return an Media smart ptr linked to the internally registered smart ptr
   *          created as a side effect of calling the concrete Media subclass ctor.
   */
  MediaFactory::PType 
  MediaFactory::operator() (Asset::Ident& key, const string& file)
  { 
    asset::Media* pM (0);
    AssetManager& aMang = AssetManager::instance();
    
    //////////////////////////////////////////////////////////TICKET #841 check and fix Category if necessary
    
    if (isnil (file))
      {
        if (isnil (key.name)) key.name="nil";
        ID<Asset> id = aMang.getID (key);
        if (aMang.known (id))
          return aMang.getAsset(ID<Media>(id));
        else
          pM = new Unknown(key);
      }
    else
      {
        if (isnil (key.name)) key.name=extractName(file);
        
        MediaAccessFacade& maf = MediaAccessFacade::instance();
        MediaDesc& handle = maf.queryFile(key.name);
        Duration length = handle.length;
        
        //////////////////////////////////////////////////////////TICKET #841 detecting and wiring multichannel compound media
        pM = new Media (key,file,length); 
      }
    ASSERT (pM);
    ENSURE (key.category.hasKind (VIDEO) || key.category.hasKind(AUDIO));
    ENSURE (!isnil (key.name));
    ENSURE (dynamic_cast<Media*>(pM) || (isnil (file) && dynamic_cast<Unknown*>(pM)));
    
    return aMang.getAsset (pM->getID());  // note: because we query with an ID<Media>, 
                                         //        we get a Media smart ptr.    
  }

  
  /** Variant of the Factory method for Media Assets, automatically 
   *  providing most of the Asset key fields based on the filename given
   */
  MediaFactory::PType 
  MediaFactory::operator() (const string& file, const Category& cat)
  { 
    Asset::Ident key(extractName(file), cat, "lumi", 1);
    return operator() (key, file);
  }
  
  MediaFactory::PType 
  MediaFactory::operator() (const string& file, asset::Kind kind)
  { 
    Category cat(kind);
    return operator() (file, cat);
  }

  
  MediaFactory::PType 
  MediaFactory::operator() (const char* file, const Category& cat)
  { 
    if (!file) file = "";
    return operator() (string(file),cat);
  }
  
  MediaFactory::PType 
  MediaFactory::operator() (const char* file, asset::Kind kind)
  { 
    if (!file) file = "";
    return operator() (string(file),kind);
  }
  
  MediaFactory::PType 
  MediaFactory::operator() (Asset::Ident& key, const char* file)
  {
    if (!file) file = "";
    return operator() (key, string(file));
  }
  
  
  /** Factory method for creating a Clip asset based
   *  on the given Media asset. This asset::Clip can be used
   *  to create a clip in the session covering the whole length
   *  of this media.
   *  @note  creates a dependency between media and new clip 
   *  @throw Invalid if the given media asset is not top-level,
   *         but rather part or a multichannel (compound) media
   */
  lib::P<Clip>
  MediaFactory::operator() (Media& mediaref)
  {
    if (mediaref.checkCompound())
      throw error::Invalid (_Fmt("Attempt to create a asset::Clip from the media %s, "
                                 "which is not toplevel but rather part of a compound "
                                 "(multichannel) media. Found parent Media %s.") 
                                 % mediaref
                                 % *mediaref.checkCompound()
                           ,LUMIERA_ERROR_PART_OF_COMPOUND);
    Clip* pC = new Clip (mediaref);
    return AssetManager::instance().wrap (*pC);
  }
  
  LUMIERA_ERROR_DEFINE (PART_OF_COMPOUND, "part of compound used as toplevel element");
  



}} // namespace asset
