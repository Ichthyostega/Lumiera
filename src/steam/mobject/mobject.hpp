/*
  MOBJECT.hpp  -  Key Abstraction: A Media Object in the Session

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/


/** @file mobject.hpp
 ** Core abstraction of the Session model: a media object.
 ** The contents of the edit and thus of the session are modelled as a collection
 ** of MObject entities, each attached by a Placement. Together these form what can
 ** be conceived as an _instance_ of the object. The Placement constitutes the relative
 ** and contextual information, while the MObject provides the identity and the link
 ** to actual media content or plug-in implementations.
 ** 
 ** @todo as of 2016, this core part of the model still needs to be actually implemented.
 **       Most of the code in this hierarchy is preliminary or placeholder code, since we
 **       prefer to build the system from ground up, not top-down. It is very likely, that
 **       MObject will be just a data tuple in the end.
 */


#ifndef STEAM_MOBJECT_MOBJECT_H
#define STEAM_MOBJECT_MOBJECT_H

#include "steam/common.hpp"
#include "steam/mobject/builder/buildertool.hpp"
#include "steam/mobject/placement.hpp"
#include "steam/asset.hpp"                    //TODO finally not needed?
#include "lib/p.hpp"
#include "lib/time/timevalue.hpp"

#include <boost/operators.hpp>
#include <string>



#include "steam/assetmanager.hpp"
using proc_interface::IDA;                   //TODO finally not needed?
//using proc_interface::PAsset;              //TODO: only temporarily
using proc_interface::AssetManager;

namespace steam {
namespace mobject {
  
  using std::string;
  
  //NOBUG_DECLARE_FLAG (mobjectmem);
  
  
  namespace session {
    class MObjectFactory;
  }
  
  
  /**
   * MObject is the interface class for all "Media Objects".
   * All the contents and elements that can be placed and 
   * manipulated and finally rendered within Lumiera's
   * high-level model and Session are MObjects.
   * @ingroup session
   */
  class MObject 
    : public Buildable,
      util::NonCopyable,
      boost::equality_comparable< MObject >
    {
    protected:
      typedef lib::time::Duration Duration;
      
      Duration length_;
      
      mutable string shortID_;
      
      MObject() ;
      virtual ~MObject() ;
      
      friend class session::MObjectFactory;
      
      
    public:
      static session::MObjectFactory create;
      
      /** a short readable ID as a single name-token,
       *  denoting both the kind of MObject and some sort of
       *  instance identity. Not necessarily unique but should
       *  be reasonable unique in most cases */
      string const& shortID()  const;
      
      /** MObject self-test (usable for asserting) */
      virtual bool isValid()  const =0;
      
      virtual Duration& getLength() =0;                                           ////////////////////TICKET #448
            
      virtual bool operator== (const MObject& oo)  const =0;  ///< needed for handling by lumiera::P
      
    protected:
      
      virtual string initShortID()  const =0;
      
    };
  
  
  
  typedef Placement<MObject> PMO;


}} // namespace steam::mobject
#endif /*STEAM_MOBJECT_MOBJECT_H*/
