/*
  OUTPUT-DIRECTOR.hpp  -  handling all the real external output connections

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file output-director.hpp
 ** A global service to coordinate and handle all external output activities.
 ** The OutputDirector is an application internal singleton service for
 ** coordinating and controlling all actual input/output- and rendering
 ** capabilities, exposing distinct lifecycle functions to connect, bring
 ** up and shut down what can be considered the "Player/Output" subsystem. 
 **
 ** @see output-manager-test.cpp  ////TODO
 */


#ifndef STEAM_PLAY_OUTPUT_DIRECTOR_H
#define STEAM_PLAY_OUTPUT_DIRECTOR_H


#include "lib/error.hpp"
#include "lib/depend.hpp"
#include "lib/depend-inject.hpp"
#include "steam/play/output-manager.hpp"
#include "common/subsys.hpp"
#include "lib/nocopy.hpp"
#include "lib/sync.hpp"

#include <memory>


namespace lumiera {
  class Play;
}
namespace steam {
namespace play {
  
//using std::string;
//using std::vector;
//using std::shared_ptr;
  using std::unique_ptr;
  
  class PlayService;
  
  
  
  /**************************************************//**
   * Management of external Output connections.
   * 
   * @todo write Type comment
   */
  class OutputDirector
    : util::NonCopyable
    , public lib::Sync<>
    {
      using SigTerm = lumiera::Subsys::SigTerm;
      using PlayServiceHandle = lib::DependInject<lumiera::Play>::ServiceInstance<PlayService>;
      
      PlayServiceHandle player_;
      ///////TODO more components and connections to manage here...
      
      bool shutdown_initiated_ = false;   /////TODO probably need a way more elaborate lifecylce management
      
    public:
      static lib::Depend<OutputDirector> instance;
      
      bool connectUp() ;
      void triggerDisconnect(SigTerm)  noexcept;
      
      bool isOperational()  const;
      
    private:
      OutputDirector() ;
     ~OutputDirector() ;
      friend class lib::DependencyFactory<OutputDirector>;
      
      
      void bringDown (SigTerm completedSignal);
    };
  
  
  
}} // namespace steam::play
#endif
