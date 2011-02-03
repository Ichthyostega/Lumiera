/*
  dummy-func.hpp  -  placeholder with dummy functions to demonstrate building shared modules
 
* ******************************************************************************************/


#ifndef COMMON_DUMMY_FUNC_H
#define COMMON_DUMMY_FUNC_H

#include "include/nobugcfg.h"

#include <string>


namespace lumiera {
  
  using std::string;
  
  /** this is a function located in the liblumieracore.so,
   *  which attempts to load the "pseudo-gui" as shared module
   *  and invoke the gui-main. The sole purpose of this function
   *  is to demonstrate that the SCons build system is working.
   */
  void loadDummyGui();
  
  
} // namespace lumiera
#endif
