/*
  ErrorLog  -  Entity to collect and persist incident records

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

* *****************************************************/


/** @file error-log.cpp
 ** Implementation details of incident logging and persistence.
 */


#include "proc/asset/meta/error-log.hpp"
//#include "proc/asset/struct-scheme.hpp"
//#include "proc/assetmanager.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/format-string.hpp"
//#include "lib/util.hpp"

//#include <string>

//using util::_Fmt;
//using util::cStr;
//using util::isnil;
//using std::string;


namespace proc {
namespace asset {
namespace meta {
  
  namespace error = lumiera::error;
  
  
 
  /** */
  ErrorLog::ErrorLog (LogID const& nameID)
    : Meta{nameID}
    { }
  
  
//  using lib::time::Time;
//  using lib::time::TimeValue;
  
  
  /** Setup of an ErrorLog: validate the settings within this builder instance,
   *  then create an appropriately configured ErrorLog instance.
   * @return shared_ptr holding onto the new asset::Meta, which has already been
   *         registered with the AssetManager.
   * @todo currently (8/2018) this is a mere placeholder, we just need an EntryID<ErrorLog>.
   */
  lib::P<ErrorLog>
  Builder<ErrorLog>::commit()
  {
//  return new ErrorLog;
    UNIMPLEMENTED ("how to build and properly register an error log entity");
  }
  
  
}}} // namespace asset::meta
