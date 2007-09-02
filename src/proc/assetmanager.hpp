/*
  ASSETMANAGER.hpp  -  Facade for the Asset subsystem
 
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


#ifndef PROC_INTERFACE_ASSETMANAGER_H
#define PROC_INTERFACE_ASSETMANAGER_H

#include <string>

#include "common/error.hpp"


using std::string;


namespace proc_interface
  {

  /**
   * Facade for the Asset subsystem
   */
  class AssetManager
    {
      int bla;
    public:
      /** registers an asset object in the internal DB, providing its unique key
       */
      static long reg (string& name, string& category, string& org, uint version)
      ;
  //      throw(cinelerra::error::Invalid);
      
      /** find and return corresponging object */
      template<class KIND>
//      void*  /////////////////TODO
      KIND
      getAsset (long id)  ;///throw(cinelerra::error::Invalid);
      
      /** @return true if the given id is registered in the internal asset DB  */
      bool known (long id) ;
      
      /**remove the given asset from the internal DB.
       * <i>together with all its dependants</i> 
       */
      void remove (long id)  ;///throw(cinelerra::error::Invalid, cinelerra::error::State);
    };

} // namespace proc_interface
#endif
