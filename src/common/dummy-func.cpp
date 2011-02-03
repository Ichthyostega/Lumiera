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
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/utility.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <fstream>
#include <string>


namespace lumiera {
  
  using std::string;
  using std::ifstream;
  using boost::regex;
  using boost::smatch;
  using boost::regex_search;
  using boost::sregex_iterator;
  
  namespace fsys = boost::filesystem;
  namespace opt = boost::program_options;

  namespace { // Implementation helpers
    
    const size_t STRING_MAX_RELEVANT = 1000;
    
    const char * const BOOTSTRAP_INI = "$ORIGIN/setup.ini";
    const char * const GUI_MODULE_TO_LOAD = "gtk_gui.lum";
    const char * const GET_PATH_TO_EXECUTABLE = "/proc/self/exe";
    
    regex EXTRACT_RELATIVE_PATHSPEC ("\\$?ORIGIN/([^:]+)");
    regex EXTRACT_PATHSPEC ("(\\$?ORIGIN)?([^:]+)");
    
    
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
      static string buff(STRING_MAX_RELEVANT+1, '\0' );
      if (!buff.size())
        {
          ssize_t chars_read = readlink (GET_PATH_TO_EXECUTABLE, &buff[0], STRING_MAX_RELEVANT);
          
          if (0 > chars_read || chars_read == ssize_t(STRING_MAX_RELEVANT))
            dieHard ("unable to discover path of running executable");
          
          buff.resize(chars_read);
        }
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
    
    class SearchPathSplitter
      : boost::noncopyable
      {
        sregex_iterator pos_,
                        end_;
        
      public:
        SearchPathSplitter (string searchPath)
          : pos_(searchPath.begin(),searchPath.end(), EXTRACT_PATHSPEC)
          , end_()
          { }
        
        bool
        hasNext()
          {
            return pos_ != end_;
          }
        
        string
        next()
          {
            ++pos_;
            if (!hasNext())
              dieHard ("Search path exhausted.");
            
            return resolveRelative();
          }
        
      private:
        /** maybe resolve a path spec given relative to
         *  the current Executable location ($ORIGIN) */
        string
        resolveRelative ()
          {
            if (containsORIGINToken())
              return asAbsolutePath();
            else
              return getFullPath();
          }
        
        smatch::value_type found(int group=0) { return (*pos_)[group]; }
        
        bool containsORIGINToken() { return found(1).matched; }
        string getRelativePath()  { return found(2);  }
        string getFullPath()     { return found(); }
        
        string
        asAbsolutePath()
          {
            fsys::path exePathName (catchMyself());
            fsys::path modPathName (exePathName.remove_leaf() / getRelativePath());
            
            if (fsys::exists(modPathName) && !fsys::is_directory (modPathName))
              dieHard ("Error in search path: component"+modPathName.string()+"is not an existing directory");
            
            return modPathName.string();
          }
      };
    
  
    /** helper to establish the location to search for loadable modules.
     *  This is a simple demonstration of the basic technique used in the
     *  real application source to establish a plugin search path, based
     *  on the actual executable position plus compiled in and configured
     *  relative and absolute path specifications.
     */
    string
    resolveModulePath (string moduleName, string searchPath = "")
    {
      fsys::path modulePathName (moduleName);
      SearchPathSplitter searchLocation(searchPath);
      
      while (true)
        {
          if (fsys::exists (modulePathName))
            {
              INFO (config, "found module %s", modulePathName.string().c_str());
              return modulePathName.string();
            }
          
          // try / continue search path
          if (searchLocation.hasNext())
            modulePathName = fsys::path() / searchLocation.next() / moduleName;
          else
            dieHard ("Module \""+moduleName+"\" not found"
                     + (searchPath.empty()? ".":" in search path: "+searchPath));
        }
    }
    
    
    
    class Config
      : boost::noncopyable
      {
        opt::options_description syntax;
        opt::variables_map settings;

      public:
        Config (string bootstrapIni)
          : syntax("Lumiera installation and platform configuration")
          , settings()
          {
            syntax.add_options()
              ("BuildsystemDemo.gui",        opt::value<string>(),
                                             "name of the Lumiera GUI plugin to load")
              ("BuildsystemDemo.modulepath", opt::value<string>(),
                                             "search path for loadable modules. "
                                             "May us $ORIGIN to refer to the EXE location")
              ;
            
            ifstream configIn (resolveModulePath (bootstrapIni).c_str());

            
            opt::parsed_options parsed = 
              opt::parse_config_file (configIn, syntax);
            
            opt::store (parsed, settings);
            opt::notify(settings);   
          }
        
        string
        operator[] (const string key)  const
          {
            return settings[key].as<string>();
          }
      };
    
  }//(End) implementation helpers
  
  
  
  
  
  
  
  void
  loadDummyGui()
  {
    Config appConfig(BOOTSTRAP_INI);
    string guiModule = appConfig["BuildsystemDemo.gui"];
    string moduleSearch = appConfig["BuildsystemDemo.modulepath"];                             
    string moduleLocation = resolveModulePath (guiModule, moduleSearch);
    
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
  
  
  
  
} // namespace lumiera
