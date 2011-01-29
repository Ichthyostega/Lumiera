/*
  dummy-func.cpp  -  placeholder with dummy functions to demonstrate building shared modules
 
* ******************************************************************************************/


#include "common/dummy-func.hpp"

#include <dlfcn.h>
#include <nobug.h>





namespace lumiera {

  const char * const GUI_MODULE_NAME = "modules/gtk_gui.lum";
  
  typedef void (*VoidFunc)(void);
  
  
  void
  loadDummyGui()
  {
    void* handle = dlopen (GUI_MODULE_NAME, RTLD_LAZY|RTLD_LOCAL);
    if (handle)
      {
        VoidFunc entryPoint = (VoidFunc) dlsym (handle, "start_dummy_gui");
        
        if (!entryPoint)
          ERROR (lumiera, "unable to resolve the entry point symbol after loading the GUI module.");
        
        else
          (*entryPoint) ();
      }
    else
      ERROR (lumiera, "unable to load %s", GUI_MODULE_NAME);    
  }
  
  
} // namespace lumiera
