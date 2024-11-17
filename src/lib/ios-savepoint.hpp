/*
  IOS-SAVEPOINT.hpp  -  capture and restore std::ostream format settings

   Copyright (C)
     2022,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file ios-savepoint.hpp
 ** Capture previous settings of an `std::ostream` and restore them when leaving scope.
 ** The stream manipulators of the IOS-Framework from the C++ standard lib allow to adjust
 ** various aspects of output formatting for following data fed to the output stream.
 ** Unfortunately many of these settings are _sticky_ and thus may "taint" common
 ** output streams like `std::cout`. By planting this RAII capsule into a local scope,
 ** the internal settings of an output stream can be recorded and restored automatically,
 ** once control flow leaves the scope.
 ** @remark based on this [stackoverflow] by «[qbert220]», 2013
 ** 
 ** @see IosSavepoint_test
 ** 
 ** [qbert220]: https://stackoverflow.com/users/617617/qbert220
 ** [stackoverflow]: https://stackoverflow.com/a/18822888 "Restore state of `std::cout` after manipulating it"
 ** 
 ** 
 */


#ifndef LIB_IOS_SAVEPOINT_H
#define LIB_IOS_SAVEPOINT_H

#include "lib/nocopy.hpp"

#include <ostream>



namespace util {
  
  /**
   * RAII helper to capture and restore
   * output stream format settings.
   */
  class IosSavepoint
    : MoveOnly
    {
      std::ostream& theStream_;
      std::ios_base::fmtflags prevFlags_;
      char fillChar_;
      
    public:
      explicit
      IosSavepoint(std::ostream& stream_to_capture)
        : theStream_{stream_to_capture}
        , prevFlags_{theStream_.flags()}
        , fillChar_{theStream_.fill()}
      { }
      
     ~IosSavepoint()
        {
          theStream_.flags(prevFlags_);
          theStream_.fill(fillChar_);
        }
    };
  
  
} // namespace util
#endif /*LIB_IOS_SAVEPOINT_H*/
