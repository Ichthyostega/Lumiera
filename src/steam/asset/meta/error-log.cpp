/*
  ErrorLog  -  Entity to collect and persist incident records

   Copyright (C)
     2018,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file error-log.cpp
 ** Implementation details of incident logging and persistence.
 */


#include "steam/asset/meta/error-log.hpp"
#include "steam/asset/struct-scheme.hpp"
#include "steam/assetmanager.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/format-string.hpp"
//#include "lib/util.hpp"

//#include <string>

//using util::_Fmt;
//using util::cStr;
//using util::isnil;
//using std::string;
using std::dynamic_pointer_cast;


namespace steam {
namespace asset{
namespace meta {
  
  namespace error = lumiera::error;
  
  
  /** storage for an unique, globally known ID.
   * Used to address the corresponding ErrorLogView in the UI
   * @todo 8/2018 for now this is a mere placeholder, but someone
   *       need to build a singleton asset to incorporate this eventually ///////////////////////////////////TICKET #1157
   */
  LogID theErrorLog_ID{"global_ErrorLog"};
  
  
  /** */
  ErrorLog::ErrorLog (LogID const& nameID)
    : Meta{nameID}
    { }
  
  
  PLog
  ErrorLog::global()
  {
    Asset::Ident ident = asset::idi::getAssetIdent (theErrorLog_ID, META);
    ID<Asset> globalLogID = AssetManager::instance().getID(ident);
                            /////////////////////////////////////////////////////////////////////////////////TICKET #739 sort out this mess with asset::ID vs EntryID
    
    if (not AssetManager::instance().known (globalLogID))
      return asset::Meta::create (theErrorLog_ID)
                         .commit();
    else
      return dynamic_pointer_cast<ErrorLog> (AssetManager::instance().getAsset (globalLogID));
  }

//  using lib::time::Time;
//  using lib::time::TimeValue;
  
  
  /** Setup of an ErrorLog: validate the settings within this builder instance,
   *  then create an appropriately configured ErrorLog instance.
   * @return shared_ptr holding onto the new asset::Meta, which has already been
   *         registered with the AssetManager.
   * @todo currently (8/2018) this is a mere placeholder, we just need an EntryID<ErrorLog>.  ///////////////TICKET #1157
   */
  lib::P<ErrorLog>
  Builder<ErrorLog>::commit()
  { 
    ASSERT (nameID == theErrorLog_ID.getSym(), "only the single global Error Log is implemented for now");///TICKET #1157 : and the entity created here is bare of any functionality
    return AssetManager::wrap (*new ErrorLog{theErrorLog_ID});
  }
  
  
}}} // namespace asset::meta
