/*
  EFFECT.hpp  -  Effect or media processing component

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file asset/effect.hpp
 ** Definition of an Asset representation for any kind of _effect_ or media processor.
 */


#ifndef ASSET_EFFECT_H
#define ASSET_EFFECT_H

#include "steam/asset/proc.hpp"



namespace steam {
namespace asset {
  
  /**
   * Effect or media processing component
   */
  class Effect : public Proc
    {
    
    public:
      
      
    private:
      /** implementation of how to resolve the corresponding plugin
       *  to yield an actual media data processing function. */
      ProcFunc*
      resolveProcessor()  const;
      
      
    };
  
  
  
}} // namespace steam::asset
#endif
