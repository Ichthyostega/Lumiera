/*
  NULL-DISPLAYER.hpp  -  fallback video displayer to not display video at all

   Copyright (C)
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file null-displayer.hpp
 ** Passive deactivated video displayer.
 ** 
 ** @deprecated obsolete since GTK-3
 ** @todo WIP as of 5/2025 attempt to accommodate to GTK-3   ////////////////////////////////////////////////TICKET #1403
 ** @see displayer.hpp
 */

#ifndef STAGE_OUTPUT_NULL_DISPLAYER_H
#define STAGE_OUTPUT_NULL_DISPLAYER_H

#include "stage/gtk-base.hpp"
#include "stage/output/displayer.hpp"

namespace Gtk {
  class Widget;
}

namespace stage {
namespace output {

/**
 * NullDisplayer implements the Displayer interface without any actual display.
 * @todo WIP as of 5/2025 attempt to accommodate to GTK-3   /////////////////////////////////////////////////TICKET #1403
 */
class NullDisplayer
  : public Displayer
  {
  public:
    
    NullDisplayer (Gtk::Widget& drawing_area, int width, int height );
    
    /** NULL-implementation: accept anything, do nothing */
    void put (void* const image) override;
    
  protected:
    
    /** NullDisplayer is always „usable“. */
    bool usable() override { return true; }
    
  private:
    
    Gtk::Widget& drawingArea_;
  };
  
  
  
}}   // namespace stage::output
#endif /*STAGE_OUTPUT_NULL_DISPLAYER_H*/
