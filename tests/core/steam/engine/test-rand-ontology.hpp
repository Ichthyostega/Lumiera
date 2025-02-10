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
    
    const Literal OID{"Test"};                  ///< classificatory prefix for functionality provided by this „library“
    const Literal TYPE_TESTFRAME{"TestFrame"};  ///< a stream implementation type with a frame of reproducible random data

    /** a dummy value */
    extern int dummyNum;
    
    /** the simplest possible function,
     *  which can be bound as Render Node */
    inline void
    dummyOp (int* num_in_buff)
    {
      CHECK (num_in_buff != nullptr);
      *num_in_buff = dummyNum++;
    }
    const Literal DUMMY_NODE_ID{"Test:dummy"};
    const Literal DUMMY_PROC_ID{"op(int)"};
    
    
    /** produce sequences of frames with (reproducible) random data */
    void generateFrame (TestFrame* buff, FraNo frameNr =0, Flavr flavour =0);
    
    /** produce planar multi channel output of random data frames */
    void generateMultichan (TestFrame* buffArry, ChaNo chanCnt, FraNo frameNr =0, Flavr flavour =0);
    
    /** create an identical clone copy of the planar multi channel frame array */
    void duplicateMultichan (TestFrame* outArry, TestFrame* inArry, ChaNo chanCnt);
    
    /** »process« a planar multi channel array of data frames in-place */
    void manipulateMultichan (TestFrame* buffArry, ChaNo chanCnt, Param param);
    
    /** »process« random frame date by hash-chaining with a parameter */
    void manipulateFrame (TestFrame* out, TestFrame const* in, Param param);
    
    /** mix two random data frames by a parameter-controlled proportion */
    void combineFrames (TestFrame* out, TestFrame const* srcA, TestFrame const* srcB, Factr mix);
    
  }//(End)namespace ont
  
  
  
  
  
  /**
   * A fake **Domain Ontology** to describe mocked »render operations« on dummy data frames
   * filled with random numbers. It provides a _builder notation_ to configure a _processing-functor_
   * for some operations working on such data frames.
   * @see TestFrame_test
   * @see NodeDevel_test
   * @see NodeLink_test
   */
  class TestRandOntology
    {
      
    public:
      struct Spec;
      
      template<class CONF>
      class Builder;
      
     ~TestRandOntology()  = default;
      TestRandOntology()  = default;
      
      auto setupGenerator();
      auto setupManipulator();
      auto setupCombinator();
    private:
    };
  
  
  
  
  struct TestRandOntology::Spec
    : util::Cloneable
    {
      const string PROTO;
      const string FUNC_ID;
      const string BASE_TYPE;
      
      Spec (Literal kind
           ,Literal type
           )
        : PROTO{_Fmt{"%s-%s"} % kind % type}
        , FUNC_ID{kind}
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
      
      auto   makeFun() { return conf_->binding(); }
      string procID()  { return conf_->procSpec(); }
      string nodeID()  { return string{ont::OID}+":"+FUNC_ID; }
    };
  
  
  namespace ont {
    using Spec = TestRandOntology::Spec;
    
    /** extended config for Generator operations */
    struct ConfGen
      {
        using Param = tuple<FraNo, Flavr>;
        
        Flavr fOff = 0;
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
    
    /** extended config for Manipulator/Filter operations */
    struct ConfMan
      {
        
        Param filter = 0;
        string streamType;
        
        ConfMan(Spec const& spec)
          : streamType{spec.BASE_TYPE}
          { }
        
        auto
        binding()
          {
            return [offset = filter]
                   (Param par, TestFrame const* in, TestFrame* out)
                      {
                        manipulateFrame (out, in, par);
                      };
          }
        
        string
        procSpec()
          {
            return _Fmt{"%s(%s)"}
                       % (filter? util::showHash(filter):"")
                       % streamType;
          }
      };
    
    /** extended config for combining/mixing operations */
    struct ConfMix
      {
        using InFeed = std::array<TestFrame const*, 2>;
        string streamType;
        
        ConfMix(Spec const& spec)
          : streamType{spec.BASE_TYPE}
          { }
        
        auto
        binding()
          {
            return []
                   (Factr mix, InFeed inChan, TestFrame* out)
                      {
                        combineFrames (out, inChan[0],inChan[1], mix);
                      };
          }
        
        string
        procSpec()
          {
            return _Fmt{"(%s/2)"}
                       % streamType;
          }
      };
  }//(End)namespace ont
  
  
  /**
   * Initiate configuration of a generator-node to produce TestFrame(s)
   */
  inline auto
  TestRandOntology::setupGenerator()
  {
    Spec spec{"generate", ont::TYPE_TESTFRAME};
    Builder<ont::ConfGen> builder{spec};
    return builder;
  }
  
  /**
   * Initiate configuration of a filter-node to manipulate TestFrame(s)
   */
  inline auto
  TestRandOntology::setupManipulator()
  {
    Spec spec{"manipulate", ont::TYPE_TESTFRAME};
    Builder<ont::ConfMan> builder{spec};
    return builder;
  }
  
  /**
   * Initiate configuration for a mixing-node to combine TestFrame(s)
   */
  inline auto
  TestRandOntology::setupCombinator()
  {
    Spec spec{"combine", ont::TYPE_TESTFRAME};
    Builder<ont::ConfMix> builder{spec};
    return builder;
  }
  
  /** Singleton accessor */
  extern lib::Depend<TestRandOntology> testRand;
  
  
  
}}} // namespace steam::engine::test
#endif
