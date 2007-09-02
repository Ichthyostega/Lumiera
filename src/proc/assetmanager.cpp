/*
  AssetManager  -  Facade for the Asset subsystem
 
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

namespace proc_interface
  {



  /**
   * registers an asset object in the internal DB, providing its unique key
   */
  long
  AssetManager::reg (string& name, string& category, string& org, uint version)  
    //throw(cinelerra::error::Invalid)
  {
  }


  /**
   * find and return corresponging object
   */
  template<class KIND>         ////TODO: does this work????
  KIND
  AssetManager::getAsset (long id)  ////throw(cinelerra::Invalid)
  {
  }


  /**
   * @return true if the given id is registered in the internal asset DB
   */
  bool
  AssetManager::known (long id)
  {
  }


  /**
   * remove the given asset <i>together with all its dependants</i> from the internal DB
   */
  void
  AssetManager::remove (long id)  /////throw(cinelerra::Invalid, cinelerra::State)
    {
    }



} // namespace proc_interface
