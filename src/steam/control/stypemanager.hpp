/*
  STYPEMANAGER.hpp  -  entry point for dealing with media stream types

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


/** @file stypemanager.hpp
 ** Interface to the stream type system
 ** @todo very crucial part of the design,
 **       unfortunately development in this area is stalled since 2012
 */


#ifndef PROC_CONTROL_STYPEMANAGER_H
#define PROC_CONTROL_STYPEMANAGER_H


#include "steam/streamtype.hpp"
#include "lib/depend.hpp"

#include <memory>


namespace proc {
namespace control {
  
  using lib::Symbol;
  
  
  class STypeManager
    {
      
      class Registry;
      std::unique_ptr<Registry> reg_;
      
    public:
      static lib::Depend<STypeManager> instance;
      
      typedef StreamType::ImplFacade ImplFacade;
      
      
      /** (re)-access a media stream type using
       *  just a symbolic ID. Effectively this queries a default */
      StreamType const& getType (Symbol sTypeID) ;
      
      StreamType const& getType (StreamType::ID stID) ;
      
      /** build or retrieve a complete StreamType implementing the given Prototype */
      StreamType const& getType (StreamType::Prototype const& protoType) ;
      
      /** build or retrieve a complete StreamType incorporating the given implementation type */
      StreamType const& getType (StreamType::ImplFacade const& implType) ;
      
      /** build or retrieve an implementation (facade)
       *  utilizing a specific MediaImplLib and implementing the given Prototype.
       *  @todo find out if this one is really necessary, because it is especially tricky */
      ImplFacade const& getImpl (Symbol libID, StreamType::Prototype const& protoType) ;
      
      /** build or retrieve an implementation (facade)
       *  wrapping up the actual implementation as designated by the rawType tag,
       *  which needs to be an implementation type of the mentioned MediaImplLib */
      template<class TY>
      ImplFacade const& getImpl (Symbol libID, TY& rawType) ;
      
      
      
      ////////////////TODO: design a mechanism allowing to add MediaImplLib implementations by plugin......
    protected:
      STypeManager() ;
      ~STypeManager();
      
      friend class lib::DependencyFactory<STypeManager>;
      
      /** Lifecycle: reset all type registration information
       *  to the <i>generic pristine default</i> state. This includes
       *  hard wired defaults and defaults provided by type plugins, but
       *  excludes everything added by the session
       */
      void reset() ;
      
    private:
      ImplFacade const& fetchImpl (StreamType::ImplFacade::TypeTag);
    };
  
  extern const char* ON_STREAMTYPES_RESET;  ///< triggered to load the generic pristine default
  
  
  template<class TY>
  StreamType::ImplFacade const&
  STypeManager::getImpl (Symbol libID, TY& rawType)
  {
    return fetchImpl (ImplFacade::TypeTag (libID,rawType));
  }
  
  
}} // namespace proc::control


namespace proc_interface {
  
  using proc::control::STypeManager;


} // namespace proc_interface


extern "C" { //TODO provide a separate header if some C code or plugin happens to need this...
  
  /** any stream type implementation, which needs to be present on the
   *  pristine default level (without any session specific configuration),
   *  should register a setup function, which will be called on each
   *  STypemanager::reset()
   */
  void
  lumiera_StreamType_registerInitFunction (void setupFun(void));
  
  // TODO provide a C interface usable from such a setupFun to access the STypeManager registration functions.
}


#endif
