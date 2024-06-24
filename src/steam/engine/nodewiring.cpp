/*
  NodeWiring  -  Implementation of the node network and operation control

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

* *****************************************************/


/** @file nodewiring.cpp
 ** Implementation of node wiring to build a render node network
 ** @todo unfinished draft from 2009 regarding the render process
 */


//#include "steam/engine/proc-node.hpp"    ///////////////////////////////TODO clarify if required further on
#include "steam/engine/connectivity-obsolete.hpp"
#include "steam/engine/nodewiring-obsolete.hpp"
#include "steam/engine/nodeoperation.hpp"
#include "steam/engine/nodewiring-config.hpp"

#include "lib/meta/typelist-manip.hpp"


namespace steam {
namespace engine {

  namespace config {
    
    using lib::meta::Flags;
    using lib::meta::CombineFlags;
    using lib::meta::DefineConfigByFlags;
    using lib::meta::Apply;
    using lib::meta::Filter;
    
    using lib::AllocationCluster;
    
    
    typedef Flags<CACHING,PROCESS,INPLACE>::Tuple AllFlags;
    
    /** build the list of all possible flag combinations */
    typedef CombineFlags<AllFlags>     AllFlagCombinations;
    
    /** build a configuration type for each of those flag combinations */
    typedef Apply<AllFlagCombinations::List, DefineConfigByFlags> AllConfigs;
              
    /** filter those configurations which actually define a wiring strategy */
    typedef Filter<AllConfigs::List, Instantiation<Strategy>::Test> PossibleConfigs;
    
    
    
    // internal details: setting up a factory for each required configuration
    
    
    
    /**
     * Fabricating a Connectivity descriptor
     * tailored for a specific node wiring situation.
     */
    template<class CONF>
    class WiringDescriptorFactory
      {
        AllocationCluster& alloc_;
        
        /* ==== pick actual wiring code ==== */
        typedef typename SelectBuffProvider<CONF>::Type BuffProvider;
        typedef ActualInvocationProcess<Strategy<CONF>, BuffProvider> InvocationStateType;
        
        // the concrete implementation of the glue code...
        typedef NodeWiring<InvocationStateType> ActualWiring;
        
        
      public:
        WiringDescriptorFactory(AllocationCluster& a) 
        : alloc_(a) {}
        
        Connectivity&
        operator() (WiringSituation const& intendedWiring)  
          { 
            return alloc_.create<ActualWiring> (intendedWiring);
          }
      };
    
    /** invocation signature of the factories */
    typedef Connectivity& (FunctionType)(WiringSituation const&);
    
    /** preconfigured table of all possible factories */
    typedef ConfigSelector< WiringDescriptorFactory  ///< Factory template to instantiate 
                          , FunctionType            ///<  function signature of the Factories
                          , AllocationCluster&     ///<   allocator fed to all factories
                          > WiringSelector;
    
    
    struct WiringFactoryImpl
      {
        WiringSelector selector;
        
        WiringFactoryImpl (AllocationCluster& alloc)
          : selector(config::PossibleConfigs::List(), alloc)
          { }
      };
  
  } // (END) internals (namespace config)
  
  
  
  
  /** As the WiringFactory (and all the embedded factories
   *  for the specific wiring situations) use the AllocationCluster
   *  of the current build process, we need to create a new instance
   *  for each newly built segment of the low-level model.
   *  @note This ctor creates a WiringFactoryImpl instance, thus 
   *        compiling this definition actually drives the necessary
   *        template instantiations for all cases encountered while
   *        building the node network.
   */
  WiringFactory::WiringFactory (lib::AllocationCluster& a)
    : alloc_(a),
      pImpl_(new config::WiringFactoryImpl (alloc_))
    { }
  
  
  WiringFactory::~WiringFactory () {}

      

  
  /** create and configure a concrete wiring descriptor to tie
   *  a ProcNode to its predecessor nodes. This includes selecting
   *  the actual StateAdapter type, configuring it based on operation
   *  control templates (policy classes).
   *  The created WiringDescriptor object is bulk allocated similar to the ProcNode
   *  objects for a given segment of the Timeline. It should be further configured
   *  with the actual predecessor nodes pointers and can then be used to create
   *  the new processing node to be wired up.  
   */
  Connectivity&
  WiringFactory::operator() (WiringSituation const& setup)
  {
    long config = setup.getFlags();
    return pImpl_->selector[config] (setup);
  }
  
  
}} // namespace steam::engine
