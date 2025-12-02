/*
  Diff  -  common helpers for the diff handling framework

   Copyright (C)
     2015,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file diff.cpp
 ** Diff handling framework support code.
 ** This translation unit emits code used by the implementation
 ** of generic functionality
 ** 
 ** @see diff-language.hpp
 ** 
 */


#include "lib/error.hpp"
#include "lib/diff/diff-language.hpp"


namespace lumiera {
namespace error {
  LUMIERA_ERROR_DEFINE(DIFF_STRUCTURE, "Invalid diff structure: implicit rules and assumptions violated.");
  LUMIERA_ERROR_DEFINE(DIFF_CONFLICT, "Collision in diff application: contents of target not as expected.");
}}
  
namespace lib {
namespace diff{
  
  
}} // namespace lib::diff
