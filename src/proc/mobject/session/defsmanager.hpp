/*
  DEFSMANAGER.hpp  -  access to preconfigured default objects and definitions
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#ifndef MOBJECT_SESSION_DEFSMANAGER_H
#define MOBJECT_SESSION_DEFSMANAGER_H


#include "proc/asset/query.hpp"

//#include <boost/scoped_ptr.hpp>
//#include <vector>

//using std::vector;
#include <tr1/memory>



namespace mobject
  {
  namespace session
    {
    using std::tr1::shared_ptr;


    /**
     * Organize a collection of preconfigured default objects.
     * For various kinds of objects we can tweek the default parametrisation
     * as part of the general session configuration. A ref to an instance of
     * this class is accessible through the current session and can be used
     * to fill in parts of the configuration of new objects, if the user 
     * code didn't give more specific parameters. Necessary sub-objects 
     * will be created on demand, and any default configuration, once
     * found, will be remembered and stored with the current session.
     */
    class DefsManager
      {
      protected:
        
        DefsManager ()  throw();
        friend class SessManagerImpl;
        
      public:
        template<class TAR>
        shared_ptr<const TAR> operator() (const asset::Query<TAR>&);
        
      };

      

  } // namespace mobject::session

} // namespace mobject
#endif
