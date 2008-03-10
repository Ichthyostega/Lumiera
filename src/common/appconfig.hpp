/*
  APPCONFIG.hpp  -  for global initialization and configuration 
 
  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>
                        Hermann Vosseler <Ichthyostega@web.de>
 
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

/** @file appconfig.hpp
 ** This header is special, as it causes global system initialisation
 ** to happen. On inclusion, it places static initialisation code,
 ** which on first run will create the Appconfig singleton instance.
 ** Additionally, the inclusion, configuration and initialisation
 ** of the NoBug library is handled here. Global <i>definitions</i>
 ** for NoBug are placed into the corresponding translation unit 
 ** appconfig.cpp"
 **
 ** @see nobugcfg.h
 */


#ifndef CINELERRA_APPCONFIG_H
#define CINELERRA_APPCONFIG_H

#include <map>
#include <string>
#include <boost/scoped_ptr.hpp>

#include "nobugcfg.h"



namespace cinelerra
  {
  using std::string;
  using boost::scoped_ptr;


  /**
   * Singleton to hold inevitable global flags and constants 
   * and for performing early (static) global initialization tasks.
   * Appconfig services are available already from static 
   * initialsation code.
   * @warning don't use Appconfig in destuctors.
   */
  class Appconfig
    {
    private:
      /** perform initialization on first access.
       *  @see #instance()  for Lifecycle     */
      Appconfig ();
      
      Appconfig (const Appconfig&); ///< copy prohibited, not implemented
      ~Appconfig ()  throw()   {}; ///< deletion prohibited
      friend void boost::checked_delete<Appconfig>(Appconfig*);

      
    public:
      /** get the (single) Appconfig instance. 
       *  Implemented as Meyers singleton.
       *  @warning don't use it in destruction code!
       */
      static Appconfig& instance()
      {
        static scoped_ptr<Appconfig> theApp_ (0);
        if (!theApp_) theApp_.reset (new Appconfig ());
        return *theApp_;
      }
      
      
      /** access the configuation value for a given key.
       *  @return empty string for unknown keys, config value else
       *  @todo do we need such a facility?
       */
      static const string & get (const string& key)  throw();
      
      
    private:
      typedef std::map<string,string> Configmap; 
      typedef std::auto_ptr<Configmap> PConfig;
      
      /** @todo <b>the following is just placeholder code!</b>
       *  Appconfig <i>could</i> do such things if necessary,
       *  or provide similar "allways available" services.
       */
      PConfig configParam_;
      
    };



} // namespace cinelerra
#endif
