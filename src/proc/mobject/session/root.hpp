/*
  ROOT.hpp  -  root element of the high-level model, global session scope
 
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


#ifndef MOBJECT_SESSION_ROOT_H
#define MOBJECT_SESSION_ROOT_H

#include "proc/mobject/session/meta.hpp"
#include "proc/mobject/builder/buildertool.hpp"


namespace mobject {
namespace session {
    
    class DefsManager;
    
    
    /**
     * High-level model root element, corresponding to
     * the global session wide scope. Serves as link to any
     * definitions, rules and defaults valid throughout this
     * session. Will be created automatically and inserted into
     * the PlacementIndex of an empty session; causes the globals
     * to be saved/loaded alongside with the model.
     * 
     * @todo WIP-WIP.. Serialisation is postponed, the rules implementation
     *       is preliminary, the active link to the AssetManager is missing.
     *       Thus, as of 12/09 this is an empty placeholder and just serves
     *       as root scope.
     */
    class Root : public Meta
      {
        DefsManager& defaults_;
      
        ///////////TODO: timespan fields here or already in class Meta??
        ///////////TODO: any idea about the purpose of root's "timespan"??  ///////TICKET #448
      
        string
        initShortID()  const
          {
            return buildShortID("Root","(✼)");
          }
        
        virtual bool isValid()  const;
        
      public:
        Root (DefsManager&);
        
        DEFINE_PROCESSABLE_BY (builder::BuilderTool);
        
      };
    
    
  } // namespace mobject::session
  
  /** Placement<Root> defined to be subclass of Placement<Meta> */
  template class Placement<session::Root, session::Meta>;
  typedef Placement<session::Root, session::Meta> PRoot;
  
} // namespace mobject
#endif
