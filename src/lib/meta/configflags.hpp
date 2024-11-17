/*
  CONFIGFLAGS.hpp  -  Building classes based on configuration cases

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file configflags.hpp
 ** Under some circumstances it is necessary to assemble functionality
 ** out of elementary building blocks. Especially, this assembly can be
 ** expressed as template specialisations directed by a configuration type.
 ** Thus, the purpose of this header is to encode flag-like configurations
 ** as distinct types, which can be used to select such specialisations. 
 ** Each possible configuration can be encoded as a list of flags, which allows
 ** to generate, filter and process those configurations. The final goal is to
 ** automatically generate a factory which is able to deliver objects
 ** configured according to the situation encoded in the flags.
 ** 
 ** @note currently there is an inherent limitation to configurations defined by
 ** a maximum of 5 independent flags. While it is easy to increase this limit,
 ** you should consider that the final goal is to generate template instantiations,
 ** which would lead to more and more code bloat with growing number of possible
 ** combinations.
 ** 
 ** @see steam::engine::config::Strategy usage example
 ** @see steam::engine::config::ConfigSelector 
 ** @see typelist.hpp
 ** 
 */


#ifndef LIB_META_CONFIGFLAGS_H
#define LIB_META_CONFIGFLAGS_H


#include "lib/meta/typelist.hpp"


namespace lib {
namespace meta{
  
  const size_t CONFIG_FLAGS_MAX = 5;
  
  
  template<uint bit> struct Flag    { typedef Flag     ID; };
  template<>         struct Flag<0> { typedef NullType ID; };
  
  
  template< uint f1=0
          , uint f2=0
          , uint f3=0
          , uint f4=0
          , uint f5=0
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
      typedef Tuple List;
    };
  
  
  template< uint f1=0
          , uint f2=0
          , uint f3=0
          , uint f4=0
          , uint f5=0
          >
  struct Config      ///< distinct type representing a configuration
    { 
      typedef typename Flags<f1,f2,f3,f4,f5>::Tuple  Flags;
      typedef Flags List;
    };
  
  
  
  template<uint Fl, class CONF> 
  struct ConfigSetFlag;       ///< set (prepend) the Flag to the given config
  
  template< uint Fl
          , uint f1
          , uint f2
          , uint f3
          , uint f4
          , uint IGN
          >
  struct ConfigSetFlag<Fl, Config<f1,f2,f3,f4,IGN>>
    {
      typedef lib::meta::Config<Fl,f1,f2,f3,f4> Config;
    };
  
  
  
  /** build a configuration type from a list-of-flags */
  template<class FLAGS, class CONF=Config<> >
  struct BuildConfigFromFlags
    {
      typedef CONF Config;
      typedef Config Type;
    };
  template<uint Fl, class FLAGS, class CONF>
  struct BuildConfigFromFlags< Node<Flag<Fl>,FLAGS>, CONF>
    { 
      typedef typename ConfigSetFlag< Fl
                                    , typename BuildConfigFromFlags<FLAGS,CONF>::Config
                                    >::Config Config;
      typedef Config Type;
    };
  
  /** create a configuration type for the given list-of-flags */
  template<class FLAGS>
  struct DefineConfigByFlags : BuildConfigFromFlags<FLAGS> { };
  
  
  namespace {
    /** helper comparing enum values and flags */
    template<uint ii, uint jj>
    struct maxC
    {
      enum{ VAL = ii < jj? jj : ii };
    };
  }
  
  
  /**
   * Helper for calculating values and for
   * invoking runtime code based on a given FlagTuple.
   * Can also be used on a Typelist of several Configs.
   * The latter case is typically used to invoke an operation
   * while enumerating all Flag-Configurations defined in Code.
   * An example would be to build (at runtime) an dispatcher table.
   * Explanation: For the Case covering a List of Configs, we provide
   * a templated visitation function, which can accept a functor object
   * to be invoked on each Configuration.
   */
  template<class FLAGS>
  struct FlagInfo;
  
  template<uint ff, class FLAGS>
  struct FlagInfo<Node<Flag<ff>, FLAGS>>
    {
      enum{ BITS = maxC< ff,  FlagInfo<FLAGS>::BITS> ::VAL
          , CODE =  (1<<ff) | FlagInfo<FLAGS>::CODE
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
  
  template<class CONF, class TAIL>
  struct FlagInfo<Node<CONF, TAIL>>
    {
      typedef typename CONF::Flags ThisFlags;
      enum{ 
            BITS = maxC< FlagInfo<ThisFlags>::BITS, FlagInfo<TAIL>::BITS > ::VAL
          };
      
      template<class FUNC>
      static typename FUNC::Ret
      accept (FUNC& functor)
        {
          functor.template visit<CONF>(FlagInfo<ThisFlags>::CODE);
          return FlagInfo<TAIL>::accept (functor);
        }
    };
  
  
  
}} // namespace lib::meta
#endif
