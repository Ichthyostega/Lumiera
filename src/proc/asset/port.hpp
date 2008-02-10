/*
  PORT.hpp  -  structural asset corresponding to some port generating media output
 
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


#ifndef ASSET_OUTPORT_H
#define ASSET_OUTPORT_H

#include "proc/asset/struct.hpp"
#include "proc/asset/procpatt.hpp"



namespace asset
  {
  
  class Port;
  typedef shared_ptr<Port> PPort;
  
  
  template<>
  class ID<Port> : public ID<Struct>
    {
    public:
      ID (size_t id);
      ID (const Port&);
    };

    
    
  /**
   * structural asset corresponding to some port 
   * for building a processing chain and 
   * generating media output
   */
  class Port : public Struct
    {
      PProcPatt wiringTemplate;
      string portID_;
      
    public:
      wstring shortDesc;
      wstring longDesc;
      
      virtual const ID<Port>& getID()  const    ///< @return ID typed to asset::Port 
        { 
          return static_cast<const ID<Port>& > (Asset::getID()); 
        }

      
    protected:
      Port (PProcPatt& wiring, string portID="", wstring shortDesc =wstring(), wstring longDesc =wstring()) ;
      friend class StructFactory;
      friend class StructFactoryImpl;

    public:
      const string& getPortID()       const { return portID_; }
      const PProcPatt& getProcPatt()  const { return wiringTemplate; }
      
      /** use another wiring template. Triggers complete rebuild of the render engine. */
      void switchProcPatt (PProcPatt& another);
      
      /** convienience shortcut for retrieving default configured ports */
      static PPort query (string properties)  ;
    };
    
    
   // catch up with postponed definition of ID<Struct> ctors...
  //
  inline ID<Port>::ID(size_t id)        : ID<Struct> (id)           {};
  inline ID<Port>::ID(const Port& port) : ID<Struct> (port.getID()) {};

      

    
    
} // namespace asset
#endif
