/*
  TRANSIENTLY.hpp  -  temporary manipulations undone when leaving scope

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file transiently.hpp
 ** Test helper to perform temporary manipulations within a test scope.
 ** Such _safe manipulations_ can be achieved by tying the clean-up to
 ** the destructor of a token object in local scope. In the simple form,
 ** a reference to the original and the original value are captured;
 ** alternatively, both manipulation and clean-up can be given as Lambdas.
 ** @see TestHelper_test::checkLocalManipulation
 ** @see scheduler-service-test.cpp
 **
 */


#ifndef LIB_TEST_TRANSIENTLY_H
#define LIB_TEST_TRANSIENTLY_H


#include "lib/nocopy.hpp"
#include "lib/meta/function.hpp"
#include "lib/ppmpl.h"

#include <utility>

namespace lib {
namespace test{
  
  /**
   * Token to capture a value
   * and restore original when leaving scope
   */
  template<typename TAR>
  class Transiently
    : util::NonCopyable
    {
      TAR originalVal_;
      TAR& manipulated_;
      
    public:
      Transiently(TAR& target)
        : originalVal_{target}
        , manipulated_{target}
        { }
      
     ~Transiently()
        {
          manipulated_ = std::move (originalVal_);
        }
      
      template<typename X>
      void
      operator= (X&& x)
        {
          manipulated_ = std::forward<X> (x);
        }
    };
  
  
  /** Variation where manipulation is done by λ */
  template<>
  class Transiently<void(void)>
    : util::NonCopyable
    {
      using Manipulator = std::function<void(void)>;
      
      Manipulator doIt_;
      Manipulator undoIt_;
      
    public:
      Transiently (Manipulator manipulation)
        : doIt_{std::move (manipulation)}
        , undoIt_{}
        { }
      
     ~Transiently()
        {
          CHECK (undoIt_, "REJECT Manipulation -- "
                          "Failed to provide a way "
                          "to undo the manipulation.");
          undoIt_();
        }
      
      void
      cleanUp (Manipulator cleanUp)
        {
          undoIt_ = std::move (cleanUp);
          doIt_(); //  actually perform the manipulation
        }
    };
  
  /** deduction guide:
   *  use λ for manipulation and clean-up
   * @remark activated when given a function with signature <void(void)>
   */
  template<typename FUN,                 typename=lib::meta::enable_if<lib::meta::has_Sig<FUN, void(void)>>>
  Transiently (FUN&&) -> Transiently<void(void)>;
  
}} // namespace lib::test




/* === test helper macros === */

/**
 * Macro to simplify capturing assignments.
 * @remark use as `TRANSIENTLY(blah) = moo;`
 */
#define TRANSIENTLY(_OO_) \
  lib::test::Transiently PPMPL_CAT(transientlyManipulated_,__LINE__)(_OO_); PPMPL_CAT(transientlyManipulated_,__LINE__)


#endif /*LIB_TEST_TRANSIENTLY_H*/
