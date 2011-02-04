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
   *  
   *  \par requirements
   *  While this isn't the actual implementation used in Lumiera,
   *  we try to mimic or demonstrate the techniques used to resolve
   *  the actual module to be loaded. So there are some requirements
   *  - \c $ORIGIN/config/setup.ini exists and defines...
   *  - a section [BuildsystemDemo], which holds
   *  - a setting gui = gtk_gui.lum
   *  - a module search path, typically: modulepath = $ORIGIN/modules
   *  Here the token \c $ORIGIN is automatically resolved to the directory
   *  holding the current executable, by reading the symlink \c /proc/self/exe
   */
  void loadDummyGui();
  
  
} // namespace lumiera
#endif
