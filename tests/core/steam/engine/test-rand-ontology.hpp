/*
  TEST-RAND-ONTOLOGY.hpp  -  placeholder for a domain-ontology working on dummy data frames

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file test-rand-ontology.hpp
 ** A faked »media calculation« environment to validate the render node network.
 */


#ifndef STEAM_ENGINE_TEST_RAND_ONTOLOGY_H
#define STEAM_ENGINE_TEST_RAND_ONTOLOGY_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/depend.hpp"
#include "lib/nocopy.hpp"
#include "lib/format-obj.hpp"
#include "lib/format-string.hpp"
#include "steam/engine/testframe.hpp"

#include <array>
#include <tuple>
#include <string>
#include <memory>
#include <utility>


namespace steam {
namespace engine{
namespace test  {
  
  using std::tuple;
  using std::string;
  using lib::Literal;
  using std::shared_ptr;
  using std::make_shared;
  using std::forward;
  using std::move;
  using util::_Fmt;
  using util::toString;
  
  /**
   * Test-Rand-Ontology : definition structures similar to a media-library
   */
  namespace ont {
    
    using FraNo = size_t;
    using ChaNo = uint;
    using Flavr = uint;
    using Factr = double;
    using Param = uint64_t;
    
    const Literal TYPE_TESTFRAME{"TestFrame"};  ///< a stream implementation type with a frame of reproducible random data
  }

  
  /** produce sequences of frames with (reproducible) random data */
  void generateFrame (TestFrame* buff, ont::FraNo frameNr =0, ont::Flavr flavour =0);
  
  /** produce planar multi channel output of random data frames */
  void generateMultichan (TestFrame* buffArry, ont::ChaNo chanCnt, ont::FraNo frameNr =0, ont::Flavr flavour =0);
  
  /** create an identical clone copy of the planar multi channel frame array */
  void duplicateMultichan (TestFrame* outArry, TestFrame* inArry, ont::ChaNo chanCnt);

  /** »process« a planar multi channel array of data frames in-place */
  void manipulateMultichan (TestFrame* buffArry, ont::ChaNo chanCnt, ont::Param param);
  
  /** »process« random frame date by hash-chaining with a parameter */
  void manipulateFrame (TestFrame* out, TestFrame const* in, ont::Param param);
  
  /** mix two random data frames by a parameter-controlled proportion */
  void combineFrames (TestFrame* out, TestFrame const* srcA, TestFrame const* srcB, ont::Factr mix);
  


/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Dummy / Placeholder
  using NoArg   = std::array<char*, 0>;
  using SoloArg = std::array<char*, 1>;
  
  extern const string DUMMY_FUN_ID;
  
  /** @todo a placeholder operation to wire a prototypical render node
   */
  inline void
  dummyOp (NoArg in, SoloArg out)
  {
    UNIMPLEMENTED ("a sincerely nonsensical operation");
  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Dummy / Placeholder
  
  /**
   * A fake _Domain Ontology_ to describe mocked »render operations« on
   * dummy data frames filled with random numbers.
   * 
   * @see TestFrame_test
   * @see NodeDevel_test
   * @see NodeLink_test
   * 
   */
  class TestRandOntology
    {
      
    public:
      struct Spec;
      
      template<class CONF>
      class Builder;
      
     ~TestRandOntology()  = default;
      TestRandOntology()  = default;
      
      auto setupGenerator (string qual ="");
    private:
    };
  
  
  struct TestRandOntology::Spec
    : util::Cloneable
    {
      const string PROTO;
      const string BASE_TYPE;
      
      Spec (Literal kind
           ,Literal type
           )
        : PROTO{_Fmt{"%s-%s"} % kind % type}
        , BASE_TYPE{type}
        { }
    };
  
  template<class CONF>
  class TestRandOntology::Builder
    : public Spec
    {
      shared_ptr<CONF> conf_;
      
    public:
      template<typename...INIT>
      Builder (Spec spec, INIT&& ...init)
        : Spec{move(spec)}
        , conf_{make_shared<CONF> (spec, forward<INIT> (init)...)}
        { }
      
      auto
      makeFun()
        {
          return conf_->binding();
        }
      
      string
      describe()
        {
          return conf_->procSpec();
        }
    };
  
  
  namespace ont {
    using Spec = TestRandOntology::Spec;
    
    /** extended config for Generator operations */
    struct ConfGen
      {
        using Param = tuple<ont::FraNo, ont::Flavr>;
        
        ont::Flavr fOff = 0;
        string streamType;
        
        ConfGen(Spec const& spec)
          : streamType{spec.BASE_TYPE}
          { }
        
        auto
        binding()
          {
            return [offset = fOff]
                   (Param par, TestFrame* out)
                      {
                        auto [frameNr,flavour] = par;
                        generateFrame (out, frameNr, flavour+offset);
                      };
          }
        
        string
        procSpec()
          {
            return _Fmt{"%s(%s)"}
                       % (fOff? toString(fOff):"")
                       % streamType;
          }
      };
  }
  
  inline auto
  TestRandOntology::setupGenerator (string qual)
  {
    Spec spec{"generate", ont::TYPE_TESTFRAME};
    Builder<ont::ConfGen> builder{spec};
    return builder;
  }
  
  /** Singleton accessor */
  extern lib::Depend<TestRandOntology> testRand;
  
  
  
}}} // namespace steam::engine::test
#endif
