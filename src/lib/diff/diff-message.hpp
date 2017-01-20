/*
  DIFF-MESSAGE.hpp  -  message to cause changes to generic model elements

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file diff-message.hpp
 ** Generic Message with an embedded diff, to describe changes to model elements.
 ** The ability to create and apply such messages to describe and effect changes,
 ** without actually knowing much about the target to receive the diff, relies on the
 ** [diff framework](\ref diff-language.hpp).
 ** 
 ** The challenging part with this task is the fact that we need to pass such messages
 ** over abstraction barriers and even schedule them into another thread (the UI event thread),
 ** but diff application actually is a _pull operation_ and thus indicates that there must
 ** be a callback actually to retrieve the diff content.
 ** 
 ** @todo as of 1/2017 this is placeholder code and we need a concept //////////////////////////////////////////TICKET #1066 : how to pass diff messages
 ** 
 ** @see [AbstractTangible_test]
 ** @see mutation-message.hpp
 ** 
 */


#ifndef LIB_DIFF_DIFF_MESSAGE_H
#define LIB_DIFF_DIFF_MESSAGE_H


#include "lib/error.hpp"

#include <boost/noncopyable.hpp>
#include <string>


namespace lib {
namespace diff{
  
  using std::string;
  
 
  
  
  
  /**
   * @todo placeholder
   */
  class DiffMessage
    : boost::noncopyable
    {
    public:
      
      void
      magically_extract_MutationMessage()
        {
          UNIMPLEMENTED ("miracle No #1066");                         //////////////////////////////////////////TICKET #1066 : how to pass diff messages
        }
    };
  
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_DIFF_MESSAGE_H*/
