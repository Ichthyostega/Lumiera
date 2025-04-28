/*
  Searchpath  -  helpers for searching directory lists and locating modules

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file searchpath.cpp
 ** Implementation of helpers to handle directory search paths.
 */


#include "lib/error.hpp"
#include "lib/searchpath.hpp"
#include "lib/format-string.hpp"
#include "lib/symbol.hpp"


/** how to retrieve the absolute path of the currently running executable
 *  on a Linux system: read the link provided by the kernel through /proc
 */
#define GET_PATH_TO_EXECUTABLE "/proc/self/exe"



namespace lib {
  
  using util::_Fmt;
  using std::regex;
  using std::regex_replace;
  
  const regex SearchPathSplitter::ACCEPT_PATHELEMENT{"(^|:)\\s*([^:]+)", regex::optimize};
  
  
  /** @internal helper to figure out the installation directory,
   *  as given by the absolute path of the currently executing program
   *  @warning this is Linux specific code */
  fs::path
  findExePath()
  {
    fs::path selfExe{GET_PATH_TO_EXECUTABLE};
    if (not fs::exists (selfExe))
      throw error::Fatal ("unable to discover path of running executable");
    return fs::canonical (selfExe);
  }
  
  
  /** @internal helper to replace all $ORIGIN prefixes in a given string
   *   by the directory holding the current executable
   *  @note also picks ORIGIN, $ORIGIN/, ORIGIN/ 
   */
  string
  replaceMagicLinkerTokens (string const& src)
  {
    static const regex PICK_ORIGIN_TOKEN{"\\$?ORIGIN/?", regex::optimize};
    static const string expandedOriginDir  
      = findExePath().parent_path().generic_string() + "/";
    
    return regex_replace (src, PICK_ORIGIN_TOKEN, expandedOriginDir);
  }
  
  
  
  
  
  string
  resolveModulePath (fs::path moduleName, string searchPath)
  {
    fs::path modulePathName (moduleName);
    SearchPathSplitter searchLocation(searchPath);
    
    while (not fs::exists (modulePathName))
      {
        // try / continue search path
        if (not searchLocation)
          throw error::Config{_Fmt{"Module %s not found%s"}
                                  % moduleName
                                  %(searchPath.empty()? ".":" in search path: "+searchPath)};
        modulePathName = *searchLocation / moduleName;
        ++searchLocation;
      }
    
    TRACE (config, "found module %s", cStr(modulePathName.generic_string()));
    return modulePathName.generic_string();
  }
  
  
  
} // namespace lib
