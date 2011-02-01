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
   *  is to demonstarte that the SCons build system is working.
   */
  void loadDummyGui();
  
  /** helper to establish the location to search for loadable modules.
   *  This is a simple demonstration of the basic technique used in the
   *  real application source to establish a plugin search path, based
   *  on the actual executable position plus compiled in and configured
   *  relative and absolute path specifications.
   */
  string resolveModulePath (string moduleName); 
  
} // namespace lumiera
#endif
