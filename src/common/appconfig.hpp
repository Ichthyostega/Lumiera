/*
  APPCONFIG.hpp  -  for global initialization and configuration 
 
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


#ifndef CINELERRA_APPCONFIG_H
#define CINELERRA_APPCONFIG_H

#include <map>
#include <string>
#include <memory>


using std::string;
using std::auto_ptr;



#include <nobug.h>
NOBUG_DECLARE_FLAG(config);



namespace cinelerra
  {


  /**
   * Singleton to hold inevitable global flags and constants 
   * and for performing erarly (static) global initialization tasks.
   */
  class Appconfig
    {
    private:
      
      /** holds the single instance and triggers initialization */
      static auto_ptr<Appconfig> theApp_;

      
      /** perform initialization on first access.
       *  A call is placed in static initialization code
       *  included in cinelerra.h; thus it will happen
       *  ubiquitous very early.
       */
      Appconfig () ;


    public:
      static Appconfig& instance()
        {
          if (!theApp_.get()) theApp_.reset (new Appconfig ());
          return *theApp_;
        }

      /** access the configuation value for a given key.
       *  @return empty string for unknown keys, else the corresponding configuration value
       */
      static string get (const string & key)  throw();
      
      
    private:
      typedef std::map<string,string> Configmap; 
      typedef auto_ptr<Configmap> PConfig;
      
      /** <b>the following is just placeholder code!</b>
       *  Appconfig <i>could</i> do such things if necessary.
       */
      PConfig configParam_;
      
    };




  namespace
    {
    /** generate "magic code" causing early static initialization */
    Appconfig* init (&Appconfig::instance ());
    } 

} // namespace cinelerra
#endif
