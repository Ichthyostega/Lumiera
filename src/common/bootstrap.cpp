/*
  dummy-func.cpp  -  placeholder with dummy functions to demonstrate building/loading shared modules
 
* *************************************************************************************************/




#include "lib/error.hpp"
#include "common/bootstrap.hpp"

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
  
  typedef smatch::value_type const& SubMatch;
  
  
  
  namespace fsys = boost::filesystem;
  namespace opt = boost::program_options;
  
  
  namespace { // Implementation helpers
    
    const size_t STRING_MAX_RELEVANT = 1000;
    
    const char * const BOOTSTRAP_INI = "$ORIGIN/config/setup.ini";
    const char * const GET_PATH_TO_EXECUTABLE = "/proc/self/exe";
    
    regex EXTRACT_PATHSPEC ("(\\$?ORIGIN/)?([^:]+)");
    
    
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
      if (!buff[0])
        {
          ssize_t chars_read = readlink (GET_PATH_TO_EXECUTABLE, &buff[0], STRING_MAX_RELEVANT);
          
          if (0 > chars_read || chars_read == ssize_t(STRING_MAX_RELEVANT))
            dieHard ("unable to discover path of running executable");
          
          buff.resize(chars_read);
        }
      return buff;
    }
    
    
    
    /**
     * Helper: Access a path Specification as a sequence of filesystem Paths.
     * This iterator class dissects a ':'-separated path list. The individual
     * components may use the symbol \c $ORIGIN to denote the directory holding
     * the current executable. After resolving this symbol, a valid absolute or
     * relative filesystem path should result, which must not denote an existing
     * file (directory is OK).
     * @note #fetch picks the current component and advances the iteration. 
     */
    class SearchPathSplitter
      : boost::noncopyable
      {
        sregex_iterator pos_,
                        end_;
        
      public:
        SearchPathSplitter (string const& searchPath)
          : pos_(searchPath.begin(),searchPath.end(), EXTRACT_PATHSPEC)
          , end_()
          { }
        
        bool
        isValid()  const
          {
            return pos_ != end_;
          }
        
        string
        fetch ()
          {
            if (!isValid())
              dieHard ("Search path exhausted.");
            
            string currentPathElement = resolveRelative();
            ++pos_;
            return currentPathElement;
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
        
        SubMatch found(int group=0) { return (*pos_)[group]; }
        
        bool containsORIGINToken() { return found(1).matched; }
        string getRelativePath()  { return found(2);  }
        string getFullPath()     { return found(); }
        
        string
        asAbsolutePath()
          {
            fsys::path exePathName (catchMyself());
            fsys::path modPathName (exePathName.remove_leaf() / getRelativePath());
            
            if (fsys::exists(modPathName) && !fsys::is_directory (modPathName))
              dieHard ("Error in search path: component \""+modPathName.string()+"\" is not a directory");
            
            return modPathName.directory_string();
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
          if (searchLocation.isValid())
            modulePathName = fsys::path() / searchLocation.fetch() / moduleName;
          else
            dieHard ("Module \""+moduleName+"\" not found"
                     + (searchPath.empty()? ".":" in search path: "+searchPath));
    }   }
    
    
    
    /**
     * Encapsulate an INI-style configuration file.
     * The acceptable settings are defined in the ctor.
     * Implementation based on boost::program_options
     */
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
            
            ifstream configIn (resolve(bootstrapIni).c_str());
            
            
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
        
      private:
        string
        resolve (fsys::path iniSpec)
          {
            string file = iniSpec.leaf();
            string searchpath = iniSpec.branch_path().string();
            return resolveModulePath (file, searchpath);
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
