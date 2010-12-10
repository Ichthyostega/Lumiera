/*
  PIPE.hpp  -  structural asset denoting a processing pipe generating media output
 
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


#ifndef ASSET_PIPE_H
#define ASSET_PIPE_H

#include "proc/asset/struct.hpp"
#include "proc/asset/procpatt.hpp"

#include <string>



namespace asset {
  
  using lumiera::P;
  using std::string;
  
  class Pipe;
  typedef P<Pipe> PPipe;
  
  
  template<>
  class ID<Pipe> : public ID<Struct>
    {
    public:
      ID (HashVal id);
      ID (Pipe const&);
      ID (PPipe const&);
      
      /** allows a Pipe-ID to stand in for a full Pipe Asset
       *  @throw error::Invalid when there is no corresponding Pipe
       */
      operator PPipe()  const;
    };
  
  
  
  /**
   * structural asset corresponding to the part
   * of the model forming a processing pipe
   * for generating media output
   */
  class Pipe
    : public Struct
    {
      PProcPatt wiringTemplate_;
      const string streamID_;                   ///< @todo just a placeholder for now 10/10
      
    public:
      string shortDesc;
      string longDesc;
      
      virtual const ID<Pipe>& getID()  const    ///< @return ID typed to asset::Pipe
        { 
          return static_cast<const ID<Pipe>& > (Asset::getID()); 
        }
      
      
    protected:
      Pipe (Asset::Ident const&, string const& streamID, PProcPatt& wiring, string shortName ="", string longName ="") ;
      friend class StructFactory;
      friend class StructFactoryImpl;
      
    public:
      string const& getPipeID()       const { return ident.name;      }
      string const& getStreamID()     const { return streamID_;       }
      PProcPatt const& getProcPatt()  const { return wiringTemplate_; }
      
      
      /** use another wiring template. Triggers complete rebuild of the render engine. */
      void switchProcPatt (PProcPatt& another);
      
      /** convenience shortcut for retrieving default configured pipes */
      static PPipe query (string properties);
      
      /** convenience shortcut for lookup by id */
      static PPipe lookup (ID<Pipe> id);
    };
    
    
   // catch up with postponed definition of ID<Struct> ctors...
  //
  inline ID<Pipe>::ID(HashVal id)        : ID<Struct> (id)            {};
  inline ID<Pipe>::ID(Pipe const& pipe)  : ID<Struct> (pipe.getID())  {};
  inline ID<Pipe>::ID(PPipe const& pipe) : ID<Struct> (pipe->getID()) {};
  inline ID<Pipe>::operator PPipe() const { return Pipe::lookup(*this); }
  
  
  
  
  
} // namespace asset
#endif
