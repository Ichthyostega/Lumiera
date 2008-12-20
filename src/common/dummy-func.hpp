/*
  dummy-func.hpp  -  placeholder with dummy functions to demonstrate building shared modules
 
* ******************************************************************************************/


#include "include/nobugcfg.h"


namespace lumiera {
  
  /** this is a function located in the liblumieracore.so,
   *  which attempts to load the "pseudo-gui" as shared module
   *  and invoke the gui-main. The sole purpose of this function
   *  is to demonstarte that the SCons build system is working.
   */
  void loadDummyGui();
  
  
} // namespace lumiera
