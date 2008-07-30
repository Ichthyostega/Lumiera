/*
  NODEWIRINGCONFIG.hpp  -  Helper for representing and selecting the wiring case
 
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
/** @file nodewiringconfig.hpp
 ** Sometimes we need to coose a different implementation for dealing with
 ** some special cases. While for simple cases, just testing a flag or using a
 ** switch statement will do the job, matters get more difficult when we have to
 ** employ a completely different execution path for each of the different cases,
 ** while using a set of common building blocks. 
 ** \par
 ** In the Lumiera render engine, right on the critical path, we need some glue code
 ** for invoking the predecessor nodes when pulling a given processing node. The actual
 ** sequence is quite dependant on the specific situation each node is wired up, regarding
 ** buffer allocation, cache querying and the possible support for GPU processing and 
 ** render farms. The solution is to define specialisations of a Strategy template
 ** using the specific configuration as template argument. Based on these, we can 
 ** create a collection of factories, which in turn will build the internal wiring
 ** for the individual ProcNode instances in accordance to the situation determined
 ** for this node, expressed as a set of flags. As a net result, each node has an 
 ** indivudual configuration (as opposed to creating a lot of hand-written individual
 ** ProcNode subclasses), but parts of this configuration assembly is done already at
 ** compile time, allowing for additional checks by typing and for possible optimisation.
 ** 
 ** @see proc::engine::config::Strategy
 ** @see nodewiring.hpp
 ** @see configflags.hpp
 ** 
 */
#ifndef ENGINE_NODEWIRINGCONFIG_H
#define ENGINE_NODEWIRINGCONFIG_H


#include "common/util.hpp"
#include "common/meta/configflags.hpp"

#include <boost/lexical_cast.hpp>
#include <map>


namespace engine {

  using lumiera::typelist::FlagInfo;
  
  using boost::lexical_cast;
  using util::contains;
  
  
  /**
   * Helper for fabricating ProcNode Wiring configurations.
   * This object builds a table of factories, holding one factory
   * for each possible node configuration. Provided with the desired
   * configuration encoded as bits, the related factory can be invoked,
   * thus producing a product object for the given configuration.
   * 
   * \par implementation notes
   * The actual factory class is templated, so it will be defined
   * at the use site of ConfigSelector. Moreover, this factory
   * usually expects an ctor argument, which will be fed through
   * when creating the ConfigSelector instance. This is one of
   * the reasons why we go through all these complicated factory
   * building: this ctor argument usually brings in a reference
   * to the actual memory allocator. Thus we have to rebuild the
   * ConfigSelector each time we switch and rebuild the ProcNode
   * factories, which in turn happens each time we use a new
   * bulk allocation memory block -- typically for each separate
   * segment of the Timeline and processing node graph.
   * 
   * Now the selection of the possible flag configurations, for which
   * Factory instances are created in the table, is governed by the
   * type parameter of the ConfigSelector ctor. This type parameter
   * needs to be a Typelist of Typelists, each representing a flag
   * configuration. The intention is to to drive this selection by
   * the use of template metaprogramming for extracting all
   * currently defined StateProxy object configurations.
   * @todo as the facories live only within the enclosed table (map)
   *       we could allocate them in-place. Unfortunately this is 
   *       non-trivial, because the stl containers employ
   *       value semantics and thus do a copy even on insert.
   *       Thus, for now we use a shared_ptr to hold the factory
   *       heap allocated.
   */
  template< template<class CONF> class Factory
          , typename PAR        ///< ctor parameter of the Factories
          , typename RET       ///<  common base class of the Factory's products
          >
  class ConfigSelector
    {
      
      struct FacFunctor
        {
          virtual ~FacFunctor() {}
          virtual RET invoke()  =0;
        };
      template<class FAC>
      struct FactoryHolder : FacFunctor
        {
          FAC factory_;
          FactoryHolder(PAR p) : factory_(p) {}
          
          virtual RET invoke ()  { return factory_(); }
        };
      
      
      typedef std::tr1::shared_ptr<FacFunctor> PFunc;
      typedef std::map<size_t, PFunc> ConfigTable;
      
      ConfigTable possibleConfig_; ///< Table of factories
      
      /** Helper: a visitor usable with FlagInfo */
      struct FactoryTableBuilder
        {
          PAR ctor_param_;
          ConfigTable& factories_;
          
          FactoryTableBuilder (ConfigTable& tab, PAR p)
            : ctor_param_(p), 
              factories_(tab) { }
          
          
          /* === visitation interface === */
          
          typedef void Ret;
          
          template<class CONF>
          void
          visit (size_t code)
            {
              PFunc pFactory (new FactoryHolder<Factory<CONF> > (ctor_param_));
              factories_[code] = pFactory;
            }
          
          void done()  {}
        };
      
    public:
      template<class CONFS>
      ConfigSelector(CONFS const&, PAR factory_ctor_param)
        {
          FactoryTableBuilder buildTable(this->possibleConfig_,
                                         factory_ctor_param );
          // store a new Factory instance
          // for each possible Flag-Configuration  
          FlagInfo<CONFS>::accept (buildTable);
        }
      
      RET
      operator() (size_t configFlags) ///< invoke the factory corresponding to the given config
        {
          if (contains (possibleConfig_, configFlags))
            return possibleConfig_[configFlags]->invoke();
          else
            throw lumiera::error::Invalid("ConfigSelector: No preconfigured factory for config-bits="+lexical_cast<std::string>(configFlags));
        }
    };
  

  
} // namespace engine
#endif
