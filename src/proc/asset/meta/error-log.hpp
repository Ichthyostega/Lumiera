/*
  ERROR-LOG.hpp  -  Entity to collect and persist incident records

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file error-log.hpp
 ** An entity to collect, possibly filter and persist incident records.
 ** 
 ** @todo 8/2018 created as mere placeholder; for now we just need an EntryID<ErrorLog>
 **       in order to mark the corresponding receiver widget in the UI. The idea is eventually
 **       to persist relevant messages, filtering them out as time passes. Such an incident log
 **       would be part of the session model, thus replicating its contents into the corresponding
 **       gui::widget::ErrorLogView -- which displays notifications without blocking the UI.
 ** 
 ** @see MetaFactory creating concrete asset::Meta instances
 ** @see gui::ctrl::NotificationHub corresponding UI controller
 ** @see gui::interact::Wizard information service in the UI
 **
 */

#ifndef ASSET_META_ERROR_LOG_H
#define ASSET_META_ERROR_LOG_H

#include "proc/asset/meta.hpp"
#include "lib/idi/entry-id.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/symbol.hpp"



namespace proc {
namespace asset {
namespace meta {
  
//  using lib::Symbol;
//  using lib::time::Time;
//  using lib::time::TimeVar;
//  using lib::time::TimeValue;
  
  
  class ErrorLog;
  using PLog = lib::P<ErrorLog>;
  using LogID = lib::idi::EntryID<ErrorLog>;
  
  
  /**
   * Receive, collect, filter and possibly persist incident records.
   * @todo 8/2018 mere placeholder type for now, to allow defining an EntyID<ErrorLog>.
   *       We conceptually need "the" ErrorLog entity as correspondence to the ErrorLogView in the GUI.
   */
  class ErrorLog
    : public Meta
    {
      
    public:
      
    protected:
      ErrorLog (LogID const&);
    };
  
  
  
  
  
  template<>
  struct Builder<ErrorLog>
    {
      
      /**
       * @todo what is "the default log"??
       */
      Builder()
        { }
      
      /** create a specific error log
       *  configured as defined through this builder
       */
      lib::P<ErrorLog> commit();
      
    };
  
}}} // namespace proc::asset::meta
#endif /*ASSET_META_ERROR_LOG_H*/
