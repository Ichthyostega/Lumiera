/*
  NODEWIRING-CONFIG.hpp  -  Helper for representing and selecting the wiring case

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

/** @file nodewiring-config.hpp
 ** Sometimes we need to choose a different implementation for dealing with
 ** some special cases. While for simple cases, just testing a flag or using a
 ** switch statement will do the job, matters get more difficult when we have to
 ** employ a completely different execution path for each of the different cases,
 ** while using a set of common building blocks.
 ** \par
 ** In the Lumiera render engine, right on the critical path, we need some glue code
 ** for invoking the predecessor nodes when pulling a given processing node. The actual
 ** sequence is quite dependent on the specific situation each node is wired up, regarding
 ** buffer allocation, cache querying and the possible support for GPU processing and
 ** render farms. The solution is to define specialisations of a Strategy template
 ** using the specific configuration as template argument. Based on these, we can 
 ** create a collection of factories, which in turn will build the internal wiring
 ** for the individual ProcNode instances in accordance to the situation determined
 ** for this node, expressed as a set of flags. As a net result, each node has an 
 ** individual configuration (as opposed to creating a lot of hand-written individual
 ** ProcNode subclasses), but parts of this configuration assembly is done already at
 ** compile time, allowing for additional checks by typing and for possible optimisation.
 ** 
 ** @see proc::engine::config::Strategy
 ** @see nodewiring.hpp
 ** @see configflags.hpp
 ** @see config-flags-test.cpp
 ** 
 */


#ifndef ENGINE_NODEWIRING_CONFIG_H
#define ENGINE_NODEWIRING_CONFIG_H


#include "lib/meta/configflags.hpp"
#include "lib/meta/util.hpp"
#include "lib/util.hpp"

#include <functional>
#include <bitset>
#include <map>


namespace proc {
namespace engine {
namespace config {
  
  using util::contains;
  using lib::meta::FlagInfo;
  
  using lib::meta::CONFIG_FLAGS_MAX;
  
  typedef size_t IxID;                 ///////////////////////////////TICKET #863
  
  
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
   * @todo as the factories live only within the enclosed table (map)
   *       we could allocate them in-place. Unfortunately this is
   *       non-trivial, because the STL containers employ
   *       value semantics and thus do a copy even on insert.
   *       Thus, for now we use a shared_ptr to hold the factory
   *       heap allocated. ---> see Ticket #248
   */
  template< template<class CONF> class Factory
          , typename FUNC       ///< common function type of all Factory instances
          , typename PAR       ///<  ctor parameter of the Factories
          >
  class ConfigSelector
    {
      typedef std::function<FUNC> FacFunction;
      
      template<class FAC>
      struct FactoryHolder ///< impl type erasure
        : private FAC,
          public FacFunction
        {
          FactoryHolder(PAR p) 
            : FAC(p),
              FacFunction (static_cast<FAC&> (*this))
            { }  
        };
      
      
      typedef std::shared_ptr<FacFunction> PFunc;
      typedef std::map<IxID, PFunc> ConfigTable;
      
      ConfigTable possibleConfig_; ///< Table of factories
      
      
      /** Helper: a visitor usable with FlagInfo.
       *  Used to create a factory for each config */
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
          visit (IxID code)
            {
              PFunc pFactory (new FactoryHolder<Factory<CONF>> (ctor_param_));
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
      
      FacFunction&
      operator[] (IxID configFlags) ///< retrieve the factory corresponding to the given config
        {
          if (contains (possibleConfig_, configFlags))
            return *possibleConfig_[configFlags];
          else
            throw lumiera::error::Invalid("ConfigSelector: No preconfigured factory for config-bits="
                                         +std::bitset<CONFIG_FLAGS_MAX>(configFlags).to_string());
        }
    };
  
  
  
  
  using lib::meta::Yes_t;
  using lib::meta::No_t;
  
  /**
   * Helper template for semi-automatic detection if instantiation is possible.
   * Requires help by the template to be tested, which needs to define a type member
   * `is_defined`. The embedded metafunction Test can be used as a predicate for
   * filtering types which may yield a valid instantiation of the candidate template
   * in question.
   * @todo 1/2016  is there no better way to achieve this, based on new language features    /////////////TICKET #986
   *       Basically we want a SFINAE helper not only to check if a specific instantiation
   *       can be formed (which would be trivial), but rather, if a specific instantiation
   *       has _already been defined_. An automated solution for this problem seems questionable
   *       by theoretic reasons; such would endanger the "One Definition Rule", since the state
   *       of definedness of any type may change during the course of a translation unit from
   *       "unknown" to "declared", "partially defined" to "fully defined". To hinge the existence
   *       of another type on this transitory state would introduce a dangerous statefulness into
   *       the meta-language, which is assumed to be stateless.
   * @todo what _could_ be done though is to detect if a given template can be _default constructed_,
   *       which, by logical weakening, implies it has be defined at all. Would that satisfy our
   *       purpose here?
   * @todo 1/2016 also I'm not happy with the name "Instantiation". It should be something like `is_XYZ`
   */
  template<template<class> class _CandidateTemplate_>
  struct Instantiation
    {
      
      template<class X>
      class Test
        {
          typedef _CandidateTemplate_<X> Instance;
          
          template<class U>
          static Yes_t check(typename U::is_defined *);
          template<class U>
          static No_t  check(...);
          
        public:
          static const bool value = (sizeof(Yes_t)==sizeof(check<Instance>(0)));
        };
    };
  
  
  
}}} // namespace proc::engine::config
#endif
