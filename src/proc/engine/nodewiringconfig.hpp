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


#ifndef ENGINE_NODEWIRINGCONFIG_H
#define ENGINE_NODEWIRINGCONFIG_H



//#include <cstddef>
#include <vector>
#include <bitset>
#include <boost/scoped_ptr.hpp>

  /////////////////TODO: library
  
#include "common/typelistutil.hpp"
namespace lumiera {
  namespace typelist {
  
    template<bool, class T, class TAIL>
    struct CondNode
      : TAIL
      { 
        typedef TAIL Next; 
      };
    template<class T, class TAIL>
    struct CondNode<true, T, TAIL>
      : Node<T,TAIL>
      { 
        typedef Node<T,TAIL> Next; 
      };
    
    template<class TYPES, template<class T> class _P_>
    struct Filter;
    
    template<template<class> class _P_>
    struct Filter<NullType,_P_>
      : NullType
      {
        typedef NullType Next;
      };
    template<class TY, class TYPES template<class> class _P_>
    struct Filter<Node<TY,TYPES>,_P_>
      : CondNode<_P_<TY>, TY, TYPES>
      {
        typedef CondNode<_P_<TY>, TY, TYPES> Next;
      };
    
    
    template<class TY1, class TY2>
    struct Append : Types<TY1,TY2>::List
      { };
    template<class TY1>
    struct Append<TY1,NullType>
      : Node<TY1,NullType>
      { };
    template<class TY2>
    struct Append<NullType,TY2>
      : Node<TY2,NullType>
      { };
    template<class TY, class TYPES>
    struct Append<NullType, Node<TY,TYPES> >
      : Node<TY,TYPES>
      { };
    template<class TY, class TYPES>
    struct Append<Node<TY,TYPES>, NullType>
      : Node<TY,TYPES>
      { };
    template<class TY, class TYPES, class TAIL>
    struct Append<Node<TY,TYPES>, TAIL>
      : Node<TY,Append<TYPES, TAIL> >
      { };
    
    template<class T, class TY>
    struct PrefixAll
      : Append<T,TY>
      { };
    template<class T>
    struct PrefixAll<T, NullType>
      : NullType
      { };
    template<class T, class TY, class TYPES>
    struct PrefixAll<T, Node<TY,TYPES> >
      : Node< Append<T,TY>
            , PrefixAll<T,TYPES>
            >
      { };
    
    template<class TY1,class TY2>
    struct Distribute : Append<TY1,TY2>
      { };
    template<class TY, class TYPES, class TAIL>
    struct Distribute<Node<TY,TYPES>, TAIL>
      : Append< PrefixAll<TY,TAIL>
              , Distribute<TYPES,TAIL>
              >
      { };
    template<class X, template<class T> class _PERM_>
    struct Combine : Distribute<_PERM_<X>, NullType>
      { };
    template<class TY, class TYPES, template<class T> class _PERM_>
    struct Combine<Node<TY,TYPES>,_PERM_>
      : Distribute<_PERM_<TY>, Combine<TYPES> >
      { };
    
    template<class F>
    struct FlagOnOff : Types<E>::List
      { };
    
    template<class FLAGS>
    struct CombineFlags
      : Combine<FLAGS, FlagOnOff>
      { };
      
    
    template<char bit>  struct Flag                { };
    template<>          struct Flag<0> : NullType  { };
    template< char f1=0
            , char f2=0
            , char f3=0
            , char f4=0
            , char f5=0
            >
    struct FlagTuple 
      : Types< Flag<f1>
             , Flag<f2>
             , Flag<f3>
             , Flag<f4>
             , Flag<f5>
             >::List
      { };
      
    template< char f1=0
            , char f2=0
            , char f3=0
            , char f4=0
            , char f5=0
            >
    struct Config
      {
        typedef FlagTuple<f1,f2,f3,f4,f5> Flags;
      };
    
    template<class X>
    struct DefineConfigFlags;
    
    template< char f1=0
            , char f2=0
            , char f3=0
            , char f4=0
            , char f5=0
            >
    struct DefineConfigFlags< FlagTuple<f1,f2,f3,f4,f5> >
      : Config<f1,f2,f3,f4,f5>
      { };
    
    
    /** 
     * Helper for calculating values and for
     * invoking runtime code based on a given FlagTuple.
     * Can also be used on a Typelist of several FlagTuples.
     * The latter case is typically used to invoke an operation
     * while ennumerating all Flag-Configurations defined in Code.
     * An example would be to build (at runtime) an dispatcher table.
     * Explanation: the supposed data structure is a list of lists,
     * where "list" here refers to a Typelist (compile time). Each
     * of the inner lists in this structure represents a single
     * Flag-Configuration. For the Case covering the outer List,
     * we provide a templated visitaion function, which can accept
     * a functor object to be invoked on each Flag-Configuration. 
     */
    template<class FLAGS>
    struct FlagInfo;
    
    template<char ff, class FLAGS>
    struct FlagInfo<Node<Flag<ff>, FLAGS> >
      {
        enum{ BITS = max(ff,   FlagInfo<FLAGS>::BITS)
            , CODE = (1<<ff) | FlagInfo<FLAGS>::CODE
            }; 
      };
    template<>
    struct FlagInfo<NullType>
      {
        enum{ BITS = 0
            , CODE = 0
            };
        
        template<class FUNC>
        static FUNC::Ret
        accept (FUNC& functor)
          {
            return functor.done();
          }
      };
    template<char ff, class FLAGS, class TAIL>
    struct FlagInfo<Node< Node<Flag<ff>, FLAGS>
                        , TAIL
                   >    >
      {
        typedef Node<Flag<ff>, FLAGS> CurrConfig;
        enum{ BITS = max(FlagInfo<CurrConfig>::BITS, FlagInfo<TAIL>::BITS)
            };
        
        template<class FUNC>
        static FUNC::Ret
        accept (FUNC& functor)
          {
            functor.template visit<CurrConfig>(FlagInfo<CurrConfig>::CODE);
            return FlagInfo<TAIL>::accept (functor);
          }
      };
    
    
    
    
      
  } // namespace typelist
  
} // namespace lumiera
  
  /////////////////TODO: library(end)
  


namespace engine {


  class WiringFactory;
  
  ///////TODO
  using lumiera::typelist::Config;
  using lumiera::typelist::FlagInfo;
  ///////TODO
  
  
  enum Cases
    { 
      CACHING,
      PROCESS,
      INPLACE,
      
      NUM_Cases
    };
  typedef std::bitset<NUM_Cases> Bits;

  
  //////////////////////////////////TODO: noch nötig??
  template<class CONF>
  Bits
  getCaseRepresentation (CONF config)
    {
      return Bits (FlagInfo<CONF::Flags>::CODE);
    }
  
  
  
  /**
   * Helper type used to describe the \em situation on which to base
   * the selection of the wiring and configuration of a ProcNode.
   * It is based on a tuple of elementary parameters (here of type bool);
   * a combination of these describes the situation and is used to select
   * a strategy, which in turn determines what concret operations to use
   * while pulling the node, and how to combine these operations.
   * @see engine::Strategy ///TODO namespace.... 
   */
//  template<bool caching, bool process, bool inplace>
//  struct Config
//    {
//      static Bits getRepr () { return getCaseRepresentation(caching,process,inplace); }
//    };
  
  
  /**
   * Helper for fabricating ProcNode Wiring configurations.
   * This object builds a table of factories, one for each 
   * possible node configuration. Provided with the desired
   * configuration encoded as bits, the related factory can
   * be invoked, thus producing a product object for the
   * given configuration.
   * \par implementation notes
   * The actual factory class is templated, so it will be defined
   * at the use site of ConfigSelecgtor. Moreover, this factory
   * usually expects an ctor argument, which will be fed through
   * when creating the ConfigSelector instance. This is one of
   * the reasons why we go through all these complicated factory
   * building: this ctor argument usually brings in a reference
   * to the actual memory allocator. Thus we have to rebuild the
   * ConfigSelector each time we switch and rebuild the ProcNode
   * factories, which in turn happens each time we use a new
   * bulk allocation memory block -- typically for each separate
   * segment of the Timeline and processing node graph.
   * The selection of the possible flag configurations, for which
   * Factory instances are created in the table, is governed by
   * the type parameter of the ConfigSelector ctor. This type
   * parameter needs to be a Typelist of Typelists, each 
   * representing a flag configuration. The intention is to
   * use template metaprogramming to extract all currently defined
   * configurations for StateProxy objects to drive this selection.
   */
  template<template<class CONF> class Factory>
  class ConfigSelector
    {
      typedef typename Factory::Ret Ret;
      typedef typename Factory::Param Param;
      
      struct FacFunctor
        {
          virtual ~FacFunctor() {}
          virtual Ret invoke()  =0;
        };
      template<class FAC>
      struct FactoryHolder : FacFunctor
        {
          FAC factory_;
          FactroyHolder(Param p) : factory_(p) {}
          
          virtual Ret invoke ()  { return factory_(); }
        };
      
      
      typedef boost::scoped_ptr<FacFunctor> PFunc;
      typedef std::map<Bits, PFunc> ConfigTable;
      
      ConfigTable possibleConfig_;
      
      /** Helper: a visitor usable with FlagInfo */
      struct FactoryTableBuilder
        {
          Param ctor_param_;
          ConfigTable& factories_;
          
          FactoryTableBuilder (ConfigTable& tab, Param p)
            : ctor_param_(p), 
              factories_(tab) { }
          
          
          /* === visitation interface === */
          
          typedef void Ret;
          
          template<class FLAGS>
          void
          visit (ulong code)
            {
              typedef typename DefineConfigFlags<FLAGS> Config;
              factories_[code].reset (new FactoryHolder<Factory<Config> > (ctor_param_));
            }
          
          void done()  {}
        };
      
    public:
      template<class CONFS>
      ConfigSelector(Param factory_ctor_param)
        {
          FactoryTableBuilder buildTable(this->possibleConfig_,
                                         factory_ctor_param );
          // store a new Factory instance
          // for each possible Flag-Configuration  
          FlagInfo<CONFS>::accept (buildTable);
        }
      
      Ret
      operator() (Bits configFlags) ///< invoke the factory corresponding to the given config
        {
          return possibleConfig_[configFlags]->invoke();
        }
    };
  

  
} // namespace engine
#endif
