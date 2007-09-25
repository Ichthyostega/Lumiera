/*
  Media(Asset)  -  key abstraction: media-like assets
 
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
 
* *****************************************************/


#include "proc/assetmanager.hpp"
#include "proc/asset/media.hpp"
#include "proc/asset/clip.hpp"
#include "proc/asset/unknown.hpp"
#include "proc/mobject/session/clip.hpp"
#include "common/util.hpp"
#include "nobugcfg.h"

#include <boost/regex.hpp>

using util::isnil;

using boost::regex;
using boost::smatch;
using boost::regex_search;


namespace asset
  {
  
  namespace // Implementation details
  {
    /** helper: extract a name token out of a given path/filename
     *  @return sanitized token based on the name (minus extension),
     *          empty string if not the common filename pattern.
     */
    string extractName (const string& path)
    {
      regex pathname_pattern("([^/\\.]+)(\\.\\w+)?$");
      smatch match;
      
      if (regex_search (path, match, pathname_pattern))
        return util::sanitize (string (match[1]));
      else
        return "";
    }
  } 

  
  
  
  typedef shared_ptr<mobject::session::Clip> PClip;
  typedef shared_ptr<asset::ProcPatt> PProcPatt;
  
  
  PClip 
  Media::createClip ()
  {
    UNIMPLEMENTED ("create clip from media asset");
    PClip clip; //TODO:null
    
    ENSURE (clip);
    return clip;
  }
  
  PProcPatt
  Media::howtoProc ()
  {
    UNIMPLEMENTED ("calculate and return processing pattern for media asset");
    PProcPatt ppatt; //TODO:null
    
    ENSURE (ppatt);
    return ppatt;
  }
  
  
  
  MediaFactory Media::create;  ///< storage for the static MediaFactory instance
  
  
  
  /** Factory method for Media Asset instances. Depending on the filename given,
   *  either a asset::Media object or an "Unknown" placeholder will be provided. If
   *  the given Category already contains an "Unkown", we just get the
   *  corresponding smart-ptr. Otherwise a new asset::Unknown is created.
   *  @return an Media smart ptr linked to the internally registered smart ptr
   *          created as a side effect of calling the concrete Media subclass ctor.
   */
  MediaFactory::PType 
  MediaFactory::operator() (Asset::Ident& key, const string& file)
  { 
    asset::Media* pM (0);
    AssetManager& aMang = AssetManager::instance();
    
    TODO ("check and fix Category if necessary");
    
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
        TODO ("file exists?");
        pM = new Media (key,file); 
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
    Asset::Ident key(extractName(file), cat, "cin3", 1);
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


} // namespace asset
