/*
  TUPLE-RECORD-INIT.hpp  -  adapter to construct tuple components from GenNode entries

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file tuple-record-init.hpp
 ** Specialised adapter to consume a record of GenNode entries to build a tuple.
 ** This is a complement to lib::meta::TupleConstructor to deal with arguments
 ** passed in our "External Tree Description" form. This is relevant for structural
 ** diff and the invocation of actions driven by messages via the UI-Bus.
 ** 
 ** In those cases, a sequence of arguments will be passed within a run-time sequential
 ** container, as a sequence of GenNode entries. The latter are variant records,
 ** which means they can hold any of a small collection of basic types, like
 ** numbers, strings, time values etc. So we have to face two challenges here.
 ** - the sequence of types in a std::tuple is fixed at compile time, as is
 **   the sequence of constructor arguments to build a given tuple type.
 **   Thus we need a compile-time iteration over a run-time sequence container
 ** - the concrete type of the given initialisation value is opaque, hidden within
 **   the GenNode variant record. And especially this means that the concrete type
 **   is known only at runtime. But we need to generate the construction code at
 **   compile time. The remedy here is to use double dispatch, i.e. to build a
 **   trampoline table with all the basically possible conversion paths for one
 **   specific type within the target tuple. For example, if one component of our
 **   tuple is a `long`, we'll provide a conversion path for the case when the
 **   GenNode holds a `short`, an `int`, and -- depending on the platform -- when
 **   it holds a `int64_t` (please recall that long and int are the same type on
 **   32bit platforms!). When encountering any other of the possible types within
 **   GenNode (e.g. `string` or `TimeSpan`), a type mismatch exception is raised.
 ** 
 ** @see control::CommandDef usage example
 ** @see TupleHelper_test
 ** @see typelist.hpp
 ** @see function.hpp
 ** @see generator.hpp
 ** 
 */


#ifndef LIB_META_TUPLE_RECORD_INIT_H
#define LIB_META_TUPLE_RECORD_INIT_H

#include "lib/meta/tuple-helper.hpp"
#include "lib/meta/typelist-manip.hpp"
#include "lib/meta/typeseq-util.hpp"
#include "lib/meta/generator.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/meta/trait.hpp"



namespace lib {
namespace meta {
  namespace error = lumiera::error;
  
  namespace { // implementation details...
    
    using lib::Variant;
    using lib::diff::Rec;
    using lib::diff::GenNode;
    using lib::diff::DataValues;
    
    
    /** the visitor type our converter is based on */
    using DataCapPredicate = Variant<DataValues>::Predicate;
    
    
    /** @internal
     * specialised Accessor to retrieve a given target type
     * from a run-time typed, opaque GenNode element. Since GenNode
     * may hold some value from a (\ref lib::diff::DataValues small
     * and fixed selection of types), we prepare a converter function
     * for each sensible conversion path to the given `TAR` target type.
     * This converter will placement-construct a target value in a buffer
     * reserved within this Accessor object.
     * @warning template bloat: we get a converter for each possible conversion
     *      for each instantiation of this template, irrespective if we actually
     *      need it or not. The price is payed per distinct type used within some
     *      target tuple; we get a VTable and some conversion functions for each.
     */
    template<typename TAR>
    struct GenNodeAccessor
      : util::NonCopyable
      {
        
        template<typename TY>
        struct allow_Conversion
          : __and_<is_constructible<TAR, TY const&>
                  ,__not_<is_narrowingInit<typename Strip<TY>::TypePlain
                                          ,typename Strip<TAR>::TypePlain>>
                  >
          { };
        
        using SupportedSourceTypes = typename Filter<DataValues::List, allow_Conversion>::List;
        
        
        
        struct ConverterBase
          : DataCapPredicate
          {
            char buffer[sizeof(TAR)];
          };
        
        template<typename TY, class BA>
        class Converter
          : public BA
          {
            virtual bool
            handle(TY const& srcElm)
              {
                new(&(BA::buffer)) TAR{srcElm};
                return true;
              };
          };
        
        
        using ConversionBuffer = InstantiateChained< SupportedSourceTypes  // for each of those types...
                                                   , Converter            //  instantiate the Converter template
                                                   , ConverterBase       //   and use this as common base class.
                                                   >;                   //    RESULT: for each type we get a handle(TY)
        
        ConversionBuffer converter_;
        
        TAR&
        accessTargetElement()
          {
            return *reinterpret_cast<TAR*> (&converter_.buffer);
          }
        
        
      public:
        GenNodeAccessor (GenNode const& node)
          : converter_()
          {
            if (not node.data.accept (converter_))
                throw error::Invalid ("Unable to build «" + util::typeStr<TAR>()
                                     +"» element from " + string(node)
                                     ,error::LUMIERA_ERROR_WRONG_TYPE);
          }
        
        /** @note RAII: when the ctor succeeds, we know the target element was built */
       ~GenNodeAccessor()
          {
            accessTargetElement().~TAR();
          }
        
        
        operator TAR ()
          {
            return accessTargetElement();
          }
      };
      
  }//(End)implementation details
  
  
  
  
  /**
   * Concrete specialisation to build a std::tuple from a sequence
   * of GenNode elements, with run-time type compatibility check.
   * @remarks intention is to use this within (\ref TupleConstructor),
   *    as will happen automatically when invoking (\ref buildTuple() )
   *    with a `Record<GenNode>` as argument. The embedded template
   *    `Access<i>` will be initialised with (a reference) to the
   *    source record, followed by an attempt to convert it to the
   *    individual member type at position i within the target tuple.
   * @throws error::Invalid when the number of types within the target
   *    exceeds the number of children within the source record.
   *    \ref LUMIERA_ERROR_INDEX_BOUNDS
   * @throws error::Invalid when some source GenNode element
   *    can not be sensibly converted to the corresponding
   *    type in the target tuple.
   *    \ref LUMIERA_ERROR_WRONG_TYPE
   */
  template<typename...TYPES>
  struct ElementExtractor<lib::diff::Rec, std::tuple<TYPES...>>
    {
      template<size_t i>
      using TargetType = typename Pick<Types<TYPES...>, i>::Type;
      
      
      template<size_t i>
      struct Access
        {
          Rec const& values;
          
          operator TargetType<i> ()
            {
              using util::toString;
              
              if (values.childSize() <= i)
                throw error::Logic ("Attempt to init the " + toString(sizeof...(TYPES))
                                   +" element " + util::typeStr<std::tuple<TYPES...>>()
                                   +" from an Rec<GenNode> with only " + toString(values.childSize())
                                   +" child elements: " + toString(values)
                                   ,error::LUMIERA_ERROR_WRONG_TYPE);
              
              return GenNodeAccessor<TargetType<i>> (values.child(i));
            }
        };
    };
  
  
  
}} // namespace lib::meta
#endif /*LIB_META_TUPLE_RECORD_INIT_H*/
