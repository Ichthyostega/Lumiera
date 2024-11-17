/*
  LIMITS.hpp  -  hard wired safety limits 

   Copyright (C)
     2012,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file limits.hpp
 ** hard wired safety limits.
 ** Whenever some task is attempted until success, or in response to external input,
 ** an arbitrary fixed constraint is imposed for indexing, counting, searching
 ** and similar ordinal operations. These limits are a protection against ordinal
 ** and index numbers insanely out-of dimension, like e.g. a symbolic ID with more
 ** than 1000 characters. Whenever an actual allocation is based on such ordinal values,
 ** a tighter and more specific limitation will be enforced on a case by case base.
 ** 
 ** @see symbol-impl.cpp 
 ** @see util::uNum
 */


#ifndef LUMIERA_LIMITS_H
#define LUMIERA_LIMITS_H


#define LUMIERA_IDSTRING_MAX_RELEVANT 1000
#define LUMIERA_MAX_ORDINAL_NUMBER    1000 
#define LUMIERA_MAX_COMPETITION        100

#endif /*LUMIERA_LIMITS_H*/
