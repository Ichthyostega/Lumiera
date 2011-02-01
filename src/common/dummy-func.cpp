/*
  dummy-func.cpp  -  placeholder with dummy functions to demonstrate building/loading shared modules
 
* *************************************************************************************************/



#ifndef LUMIERA_PLUGIN_PATH
#error please define the plugin search path as -DLUMIERA_PLUGIN_PATH, e.g. as $INSTALL_PREFIX/lib/lumiera
#endif


#include "common/dummy-func.hpp"

extern "C" {
#include <unistd.h>
}
#include <dlfcn.h>
#include <nobug.h>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <string>


namespace lumiera {
  
  using std::string;
  using boost::regex;
  using boost::smatch;
  using boost::regex_search;
  
  namespace fsys = boost::filesystem;
  
  namespace { // Implementation helpers
    
    const size_t STRING_MAX_RELEVANT = 1000;
    
    const char * const GUI_MODULE_TO_LOAD = "gtk_gui";
    const char * const GET_PATH_TO_EXECUTABLE = "/proc/self/exe";
    
    regex EXTRACT_RELATIVE_PATHSPEC ("\\$?ORIGIN/([^:]+)");
    
    
    /** the real application would throw a custom exception... */
    void
    dieHard (string msg)
    {
      NOBUG_ERROR (NOBUG_ON, "Fatal Error: %s ", msg.c_str());
      abort();
    }
    
    
    /** figure out the absolute path
     *  of the currently running executable
     */
    string
    catchMyself ()
    {
      string buff(STRING_MAX_RELEVANT+1, '\0' );
      ssize_t chars_read = readlink (GET_PATH_TO_EXECUTABLE, &buff[0], STRING_MAX_RELEVANT);
      
      if (0 > chars_read || chars_read == ssize_t(STRING_MAX_RELEVANT))
        dieHard ("unable to discover path of running executable");
      
      buff.resize(chars_read);
      return buff;
    }
    
    
    /** extract from the PLUGIN_PATH a path specification
     *  given relative to the location of the executable,
     *  as denoted by the 'ORIGIN' token
     */
    string
    getRelativeModuleLocation()
    {
      smatch match;
      if (regex_search (string(LUMIERA_PLUGIN_PATH), match, EXTRACT_RELATIVE_PATHSPEC))
        return (match[1]);
      else
        dieHard ("no valid module loading location relative to executable defined in LUMIERA_PLUGIN_PATH");
    }
  }//(End) implementation helpers
  
  
  
  
  
  
  
  void
  loadDummyGui()
  {
    string moduleLocation = resolveModulePath (GUI_MODULE_TO_LOAD);
    
    void* handle = dlopen (moduleLocation.c_str(), RTLD_LAZY|RTLD_LOCAL);
    if (handle)
      {
        typedef void (*VoidFunc)(void);
        
        VoidFunc entryPoint = (VoidFunc) dlsym (handle, "start_dummy_gui");
        
        if (!entryPoint)
          dieHard ("unable to resolve the entry point symbol after loading the GUI module.");
        
        else
          (*entryPoint) ();  // activate loaded module
      }
    else
      dieHard ("unable to load "+moduleLocation);
  }
  
  
  string
  resolveModulePath (string moduleName)
  {
    fsys::path exePathName (catchMyself());
    fsys::path modPathName (exePathName.remove_leaf() / getRelativeModuleLocation() / (moduleName+".lum") );
    
    if (!fsys::exists (modPathName))
      dieHard ("Module "+modPathName.string()+" doesn't exist.");
    
    INFO (config, "found module %s", modPathName.string().c_str());
    return modPathName.string();
  }
  
  
  
} // namespace lumiera
