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
#include "lib/symbol.hpp"


/** how to retrieve the absolute path of the currently running executable
 *  on a Linux system: read the link provided by the kernel through /proc
 */
#define GET_PATH_TO_EXECUTABLE "/proc/self/exe"



namespace lib {
  
  using std::regex;
  using std::regex_replace;
  
  const regex SearchPathSplitter::EXTRACT_PATHSPEC   ("[^:]+");
  
  
  /** @internal helper to figure out the installation directory,
   *  as given by the absolute path of the currently executing program
   *  @warning this is Linux specific code */
  string
  findExePath()
  {
    static string buff(lib::STRING_MAX_RELEVANT+1, '\0' );
    if (!buff[0])
      {
        ssize_t chars_read = readlink (GET_PATH_TO_EXECUTABLE, &buff[0], lib::STRING_MAX_RELEVANT);
        
        if (0 > chars_read || chars_read == ssize_t(lib::STRING_MAX_RELEVANT))
          throw error::Fatal ("unable to discover path of running executable");
        
        buff.resize(chars_read);
      }
    return buff;
  }
  
  
  /** @internal helper to replace all $ORIGIN prefixes in a given string
   *   by the directory holding the current executable
   *  @note also picks ORIGIN, $ORIGIN/, ORIGIN/ 
   */
  string
  replaceMagicLinkerTokens (string const& src)
  {
    static const regex PICK_ORIGIN_TOKEN ("\\$?ORIGIN/?");
    static const string expandedOriginDir  
      = fsys::path (findExePath()).parent_path().string() + "/";          ///////////TICKET #896
    
    return regex_replace(src, PICK_ORIGIN_TOKEN, expandedOriginDir);
  }
  
  
  
  
  
  string
  resolveModulePath (fsys::path moduleName, string searchPath)
  {
    fsys::path modulePathName (moduleName);
    SearchPathSplitter searchLocation(searchPath);                        ///////////TICKET #896
    
    while (!fsys::exists (modulePathName))
      {
        // try / continue search path
        if (searchLocation.isValid())
          modulePathName = fsys::path() / searchLocation.next() / moduleName;
        else
          throw error::Config ("Module \""+moduleName.string()+"\" not found"   /////TICKET #896
                              + (searchPath.empty()? ".":" in search path: "+searchPath));
      }
    
    TRACE (config, "found module %s", modulePathName.string().c_str());
    return modulePathName.string();                                       ///////////TICKET #896
  }
  
  
  
} // namespace lib
