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


#include "common/util.hpp"

//#include <cstddef>
#include <algorithm>
#include <vector>
#include <bitset>
#include <boost/scoped_ptr.hpp>

  /////////////////TODO: library
  
#include "common/typelistutil.hpp"
namespace lumiera {
  namespace typelist {
  
    template<bool, class T, class TAIL>
    struct CondNode                        { typedef TAIL  Next; };
    
    template<class T, class TAIL>
    struct CondNode<true, T, TAIL>         { typedef Node<T,TAIL>  Next; };
    
    
    template< class TYPES
            , template<class> class _P_
            >
    struct Filter;
    
    template<template<class> class _P_>
    struct Filter<NullType,_P_>            { typedef NullType  List; };
    
    template< class TY, class TYPES
            , template<class> class _P_
            >
    struct Filter<Node<TY,TYPES>,_P_>      { typedef typename CondNode<_P_<TY>::value
                                                                      , TY
                                                                      , typename Filter<TYPES,_P_>::List
                                                                      >::Next      
                                                                      List; };
    
    
    
    template<class TY1, class TY2>
    struct Append                          { typedef Node<TY1, typename Append<TY2,NullType>::List>  List; };
    
    template< class TY, class TYPES
            , class TAIL
            >
    struct Append<Node<TY,TYPES>, TAIL>    { typedef Node<TY,  typename Append<TYPES,  TAIL>::List>  List; };
    
    template<class TY, class TYPES>
    struct Append<NullType, Node<TY,TYPES> >   { typedef Node<TY,TYPES>   List; };
    
    template<class TY, class TYPES>
    struct Append<Node<TY,TYPES>, NullType>    { typedef Node<TY,TYPES>   List; };
    
    template<class TY1>
    struct Append<TY1,NullType>            { typedef Node<TY1,NullType>   List; };
    
    template<class TY2>
    struct Append<NullType,TY2>            { typedef Node<TY2,NullType>   List; };
    
    template<>
    struct Append<NullType,NullType>       { typedef NullType             List; };
    
    
    
    
    template<class T, class TY>
    struct PrefixAll                       { typedef typename Append<T,TY>::List  List; };
    
    template<class T>
    struct PrefixAll<T, NullType>          { typedef NullType  List; };
    
    template< class T
            , class TY, class TYPES
            >
    struct PrefixAll<T, Node<TY,TYPES> >   { typedef Node< typename Append<T,TY>::List
                                                         , typename PrefixAll<T,TYPES>::List
                                                         >     List; };
    
    
    
    
    template<class TY1,class TY2>
    struct Distribute                      { typedef typename Append<TY1,TY2>::List  List; };
    
    template< class TY, class TYPES
            , class TAIL
            >
    struct Distribute<Node<TY,TYPES>,TAIL> { typedef typename Append< typename PrefixAll<TY,TAIL>::List
                                                                    , typename Distribute<TYPES,TAIL>::List
                                                                    >::List                    
                                                                    List; };
    
    
    
    
    template< class X
            , template<class> class _PERMU_>
    struct Combine                         { typedef typename Distribute< typename _PERMU_<X>::List
                                                                        , NullType
                                                                        >::List  List; };
    
    template< class TY, class TYPES
            , template<class> class _PERMU_>
    struct Combine<Node<TY,TYPES>,_PERMU_> { typedef typename Distribute< typename _PERMU_<TY>::List
                                                                        , typename Combine<TYPES,_PERMU_>::List
                                                                        >::List  List; };
    
    
    template<class F>
    struct FlagOnOff
      { 
        typedef typename Types<F>::List  List;
      };
    
    template<class FLAGS>
    struct CombineFlags
      { 
        typedef typename Combine<FLAGS, FlagOnOff>::List  List; 
      };
    
    
    
    using std::max;
    
    
    template<char bit> struct Flag    { typedef Flag     ID; };
    template<>         struct Flag<0> { typedef NullType ID; };
    
    
    template< char f1=0
            , char f2=0
            , char f3=0
            , char f4=0
            , char f5=0
            >
    struct Flags        
      { 
        typedef typename Types< typename Flag<f1>::ID
                              , typename Flag<f2>::ID
                              , typename Flag<f3>::ID
                              , typename Flag<f4>::ID
                              , typename Flag<f5>::ID
                              >::List       
                              Tuple;
      };
    
    
    template< char f1=0
            , char f2=0
            , char f3=0
            , char f4=0
            , char f5=0
            >
    struct Config 
      { 
        typedef typename Flags<f1,f2,f3,f4,f5>::Tuple  Flags; 
      };
    
    
    template<class X>
    struct DefineConfigByFlags;
    
    template< char f1
            , char f2
            , char f3
            , char f4
            , char f5
            >
    struct DefineConfigByFlags< typename Flags<f1,f2,f3,f4,f5>::Tuple >
      { 
        typedef Config<f1,f2,f3,f4,f5> Config; 
      };
    
    
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
        enum{ BITS = MAX(ff,   FlagInfo<FLAGS>::BITS)
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
        static typename FUNC::Ret
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
        typedef Node<Flag<ff>, FLAGS> ThisConfig;
        enum{ BITS = MAX (FlagInfo<ThisConfig>::BITS, FlagInfo<TAIL>::BITS)
            };
        
        template<class FUNC>
        static typename FUNC::Ret
        accept (FUNC& functor)
          {
            functor.template visit<ThisConfig>(FlagInfo<ThisConfig>::CODE);
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
  using lumiera::typelist::DefineConfigByFlags;
  ///////TODO
  
  
  enum Cases
    { 
      CACHING = 1,
      PROCESS,
      INPLACE,
      
      NOT_SET   = 0,
      NUM_Cases = INPLACE
    };
  typedef std::bitset<NUM_Cases> Bits;

  
  
  
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
   * NOw the selection of the possible flag configurations, for which
   * Factory instances are created in the table, is governed by the
   * type parameter of the ConfigSelector ctor. This type parameter
   * needs to be a Typelist of Typelists, each representing a flag
   * configuration. The intention is to to drive this selection by
   * the use of template metaprogramming for extracting all
   * currently defined StateProxy object configurations.
   */
  template< template<class CONF> class Factory
          , class PAR            ///< ctor parameter of the Factories
          , class RET           ///<  common base class of the Factory's products
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
      
      
      typedef boost::scoped_ptr<FacFunctor> PFunc;
      typedef std::map<Bits, PFunc> ConfigTable;
      
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
          
          template<class FLAGS>
          void
          visit (ulong code)
            {
              typedef typename DefineConfigByFlags<FLAGS>::Config Config;
              factories_[code].reset (new FactoryHolder<Factory<Config> > (ctor_param_));
            }
          
          void done()  {}
        };
      
    public:
      template<class CONFS>
      ConfigSelector(PAR factory_ctor_param)
        {
          FactoryTableBuilder buildTable(this->possibleConfig_,
                                         factory_ctor_param );
          // store a new Factory instance
          // for each possible Flag-Configuration  
          FlagInfo<CONFS>::accept (buildTable);
        }
      
      RET
      operator() (Bits configFlags) ///< invoke the factory corresponding to the given config
        {
          return possibleConfig_[configFlags]->invoke();
        }
    };
  

  
} // namespace engine
#endif
