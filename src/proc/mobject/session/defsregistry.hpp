/*
  DEFSREGISTRY.hpp  -  implementation of the default object store
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#ifndef MOBJECT_SESSION_DEFSREGISTRY_H
#define MOBJECT_SESSION_DEFSREGISTRY_H


#include "common/singleton.hpp"

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
    class DefsRegistry
      {
      protected:
        
        DefsRegistry ();
        friend class lumiera::singleton::StaticCreate<SessManagerImpl>;
        
      public:
        
      };

      

  } // namespace mobject::session

} // namespace mobject

#endif
