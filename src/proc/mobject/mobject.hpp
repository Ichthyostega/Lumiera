/*
  MOBJECT.hpp  -  Key Abstraction: A Media Object in the Session
 
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


#ifndef MOBJECT_MOBJECT_H
#define MOBJECT_MOBJECT_H

#include "pre.hpp"


#include "proc/common.hpp"
#include "proc/mobject/builder/buildertool.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/asset.hpp"                    //TODO finally not needed?

#include <boost/noncopyable.hpp>
#include <boost/operators.hpp>
#include <list>


using std::list;

#include "proc/assetmanager.hpp"
using proc_interface::IDA;                   //TODO finally not needed?
//using proc_interface::PAsset;              //TODO: only temporarily
using proc_interface::AssetManager;

namespace mobject {
  
  using lumiera::P;
  
  //NOBUG_DECLARE_FLAG (mobjectmem);
  
  
  namespace session {
    class MObjectFactory;
  }
  
  
  /**
   * MObject is the interface class for all "Media Objects".
   * All the contents and elements that can be placed and 
   * manipulated and finally rendered within Lumiera's EDL 
   * are MObjects.
   */
  class MObject 
    : public Buildable,
      boost::noncopyable,
      boost::equality_comparable< MObject >
    {
    protected:
      typedef lumiera::Time Time;
      
      // TODO: how to represent time intervals best?
      Time length;
      
      MObject() {}
      virtual ~MObject() {};
      
      friend class session::MObjectFactory;
      
      
    public:
      static session::MObjectFactory create;
      
      /** MObject self-test (usable for asserting) */
      virtual bool isValid()  const =0;
      
      virtual Time& getLength() =0; ///< @todo how to deal with the time/length field?? ////TICKET #448
      
      virtual bool operator== (const MObject& oo)  const =0;
      
    };
  
  
  
  typedef Placement<MObject> PMO;


} // namespace mobject
#endif
/*
// Local Variables:
// mode: C++
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
