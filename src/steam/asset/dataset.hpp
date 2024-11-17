/*
  DATASET.hpp  -  meta asset describing a collection of control data

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file dataset.hpp
 ** Definition of an Asset to represent a set of control data
 ** An example would be captured animation data from some kind of external controller.
 ** @warning as of 2016, this is a design placeholder. The concept seems reasonable though.
 */


#ifndef ASSET_DATASET_H
#define ASSET_DATASET_H

#include "steam/asset/meta.hpp"



namespace steam {
namespace asset {
  
  
  /**
   * meta asset describing a collection of control data
   */
  class Dataset : public Meta
    {};
  
}} // namespace steam::asset
#endif
