/*
  Diff  -  common helpers for the diff handling framework

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


/** @file diff.cpp
 ** Diff handling framework support code.
 ** This translation unit emits code used by the implementation
 ** of generic functionality
 ** 
 ** @see diff-language.hpp
 ** 
 */


#include "lib/error.hpp"
#include "lib/format-util.hpp"
#include "lib/diff/diff-language.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/variant.hpp"


namespace lib {
namespace diff{
  
  LUMIERA_ERROR_DEFINE(DIFF_CONFLICT, "Collision in diff application: contents of target not as expected.");
  
  /* symbolic marker ID references
   * used within the tree diff language
   * to mark specific scopes
   */
  Ref Ref::END    ("_END_");
  Ref Ref::THIS   ("_THIS_");
  Ref Ref::CHILD  ("_CHILD_");
  Ref Ref::ATTRIBS("_ATTRIBS_");
  
  
  
  
  /** Implementation of content equality test
   * @throws error::Logic when the given other DataCap
   *         does not hold a value of the same type than
   *         this DataCap.
   * @remarks since the GenNode::ID is generated including a type hash,
   *         the equality operator of GenNode ensures this content test
   *         is only called on a compatible DataCap.
   */
  bool
  DataCap::operator== (DataCap const& o)  const
  {
    class EqualityTest
      : public Variant<DataValues>::Predicate
      {
        DataCap const& o_;

#define DERIVE_EQUALITY(_TY_) \
        virtual bool handle  (_TY_ const& val) override { return (o_.get<_TY_>() == val); }
        
        DERIVE_EQUALITY (int)
        DERIVE_EQUALITY (int64_t)
        DERIVE_EQUALITY (short)
        DERIVE_EQUALITY (char)
        DERIVE_EQUALITY (bool)
        DERIVE_EQUALITY (double)
        DERIVE_EQUALITY (string)
        DERIVE_EQUALITY (time::Time)
        DERIVE_EQUALITY (time::Offset)
        DERIVE_EQUALITY (time::Duration)
        DERIVE_EQUALITY (time::TimeSpan)
        DERIVE_EQUALITY (hash::LuidH)
        DERIVE_EQUALITY (RecRef)
        DERIVE_EQUALITY (Rec)
        
      public:
        EqualityTest(DataCap const& o)
          : o_(o)
          { }
      };
    
    EqualityTest visitor(o);
    return accept(visitor);
  }
  
  DataCap::operator string()  const
  {
    return "DataCap|"+string(this->buffer());
  }
  
  
}} // namespace lib::diff
