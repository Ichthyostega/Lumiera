/*
  ERROR-LOG.hpp  -  Entity to collect and persist incident records

   Copyright (C)
     2018,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file error-log.hpp
 ** An entity to collect, possibly filter and persist incident records.
 ** 
 ** @todo 8/2018 created as mere placeholder; for now we just need an EntryID<ErrorLog>
 **       in order to mark the corresponding receiver widget in the UI. The idea is eventually
 **       to persist relevant messages, filtering them out as time passes. Such an incident log
 **       would be part of the session model, thus replicating its contents into the corresponding
 **       stage::widget::ErrorLogView -- which displays notifications without blocking the UI.
 ** 
 ** @see MetaFactory creating concrete asset::Meta instances
 ** @see stage::ctrl::NotificationHub corresponding UI controller
 ** @see stage::interact::Wizard information service in the UI
 **
 */

#ifndef ASSET_META_ERROR_LOG_H
#define ASSET_META_ERROR_LOG_H

#include "steam/asset/meta.hpp"
#include "lib/idi/entry-id.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/symbol.hpp"

#include <string>



namespace steam {
namespace asset{
namespace meta {
  
  using std::string;
//  using lib::Symbol;
//  using lib::time::Time;
//  using lib::time::TimeVar;
//  using lib::time::TimeValue;
  
  
  class ErrorLog;
  using PLog = lib::P<ErrorLog>;
  using LogID = lib::idi::EntryID<ErrorLog>;
  
  extern LogID theErrorLog_ID;
  
  
  /**
   * Receive, collect, filter and possibly persist incident records.
   * @todo 8/2018 mere placeholder type for now, to allow defining an EntyID<ErrorLog>.
   *       We conceptually need "the" ErrorLog entity as correspondence to the ErrorLogView in the GUI.
   */
  class ErrorLog
    : public Meta
    {
      
    public:
      /** retrieve (possibly create) the global singleton asset
       *  corresponding to "the" global error log, as shown in the UI.
       */
      static PLog global();
      
    protected:
      ErrorLog (LogID const&);
      
      friend class Builder<ErrorLog>;
    };
  
  
  
  
  
  template<>
  struct Builder<ErrorLog>
    {
      string nameID;
      /**
       * @todo what is "the default log"??
       */
      Builder(string name)
        : nameID{name}
        { }
      
      /** create a specific error log
       *  configured as defined through this builder
       */
      lib::P<ErrorLog> commit();
      
    };
  
}}} // namespace steam::asset::meta
#endif /*ASSET_META_ERROR_LOG_H*/
