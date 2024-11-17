/*
  logging.h  -  Configure basic nobug logging

   Copyright (C)
     2008, 2009       Christian Thaeter <ct@pipapo.org>
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

#ifndef LUMIERA_LOGGING_H
#define LUMIERA_LOGGING_H

/** @file logging.h
 ** This header is for including and configuring NoBug.
 ** The idea is that configuration and some commonly used flag
 ** declarations are to be kept in one central location. Subsystems
 ** are free to define and use additional flags for local use. Typically,
 ** this header will be included via some of the basic headers like error.hpp,
 ** which in turn gets included e.g. by steam/common.hpp
 ** 
 ** This header can thus be assumed to be effectively global. It should contain
 ** only declarations of global relevance, as any change causes the whole project
 ** to be rebuilt. All flags defined here are initialised automatically.
 ** 
 ** We use the 'NOBUG_DECLARATIONS_ONLY' magic to generate declarations and
 ** definitions only out of this header.
 ** 
 ** # Logging configuration
 ** 
 ** By default, logging is configured such as to emit a small number of informative
 ** messages on the starting terminal and to report fatal errors. But besides the
 ** usual fine-grained tracing messages, we define a small number of distinct
 ** thematic <b>Logging Channels</b> providing a consistent high-level view of
 ** what is going on with regards to a specific aspect of the application
 ** - `progress` documents a high-level overall view of what the application _does_
 ** - `render` focuses on the working of the render engine (without logging each frame)
 ** - `config` shows anything of relevance regarding the configured state of App and session
 ** - `memory` allows to diagnose a high-level view of memory management
 ** 
 ** Any log level can be overridden by an environment variable, for example
 ** `NOBUG_LOG='progress:INFO' ./lumiera`
 **
 ** @todo logging to files?
 ** \verbatim
 ** `NOBUG_LOG='progress:INFO@file(name=filename)'` api to set this statically up by the program will follow
 ** \endverbatim
 **   --cehteh
 */


#include <nobug.h>

/// magic to generate NoBug definitions
#ifndef LUMIERA_NOBUG_INIT_CPP
#undef NOBUG_DECLARE_ONLY
#define NOBUG_DECLARE_ONLY 1
#endif

/** the root switch for all logging */
NOBUG_CPP_DEFINE_FLAG           (all);


/** debug logging */
NOBUG_CPP_DEFINE_FLAG_PARENT    ( debugging,                    all);
/** debug logging for the main application starter */
NOBUG_CPP_DEFINE_FLAG_PARENT    (  main_dbg,                    debugging);
/** base of debug logging for the vault layer */
NOBUG_CPP_DEFINE_FLAG_PARENT    (  backend_dbg,                 debugging);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   file_dbg,                   backend_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   filehandle_dbg,             backend_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   filehandlecache_dbg,        backend_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   filedescriptor_dbg,         backend_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   mmap_dbg,                   backend_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   mmapcache_dbg,              backend_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   mmapings_dbg,               backend_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   thread_dbg,                 backend_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    threads_dbg,               thread_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    threadpool_dbg,            thread_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   fileheader_dbg,             backend_dbg);

/** base of debug logging for the steam layer */
NOBUG_CPP_DEFINE_FLAG_PARENT    (  proc_dbg,                    debugging);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   command_dbg,                proc_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   session_dbg,                proc_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   player_dbg,                 proc_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   engine_dbg,                 proc_dbg);
/** base of debug logging for the stage layer */
NOBUG_CPP_DEFINE_FLAG_PARENT    (  gui_dbg,                     debugging);
/** base if debug logging for the support library */
NOBUG_CPP_DEFINE_FLAG_PARENT    (  library_dbg,                 debugging);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   mpool_dbg,                  library_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   psplay_dbg,                 library_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   priqueue,                   library_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   resourcecollector_dbg,      library_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   mutex_dbg,                  library_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   cond_dbg,                   library_dbg);
/** base of debug logging for the common library */
NOBUG_CPP_DEFINE_FLAG_PARENT    (  common_dbg,                  debugging);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   config_dbg,                 common_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    configfile_dbg,            config_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    configitem_dbg,            config_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    configtyped_dbg,           config_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    configlookup_dbg,          config_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   interface_dbg,              common_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    interfaceregistry_dbg,     interface_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   pluginloader_dbg,           common_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (  plugins_dbg,                 debugging);


/** base of runtime logging always available */
NOBUG_CPP_DEFINE_FLAG_PARENT    ( logging,                      all);
/** general application progress base */
NOBUG_CPP_DEFINE_FLAG_PARENT    (  progress,                    logging);
/** progress log for the main starter */
NOBUG_CPP_DEFINE_FLAG_PARENT    (   main,                       progress);
/** progress log for the vault layer */
NOBUG_CPP_DEFINE_FLAG_PARENT    (   vault,                      progress);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    mmap,                      vault);         //mmap errors
NOBUG_CPP_DEFINE_FLAG_PARENT    (    thread,                    vault);         //starting/stopping threads
NOBUG_CPP_DEFINE_FLAG_PARENT    (     threads,                  thread);
NOBUG_CPP_DEFINE_FLAG_PARENT    (     threadpool,               thread);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    fileheader,                vault);
/** progress log for the steam layer */
NOBUG_CPP_DEFINE_FLAG_PARENT    (   steam,                      progress);
/** progress log for steam-layer command dispatch */
NOBUG_CPP_DEFINE_FLAG_PARENT    (    command,                   steam);
/** progress log for session datastructure */
NOBUG_CPP_DEFINE_FLAG_PARENT    (    session,                   steam);
/** progress log for the builder and build process */
NOBUG_CPP_DEFINE_FLAG_PARENT    (    builder,                   steam);
/** progress log for running the engine */
NOBUG_CPP_DEFINE_FLAG_PARENT    (    engine,                    steam);
/** progress log for play- and render subsystem */
NOBUG_CPP_DEFINE_FLAG_PARENT    (    play,                      steam);
/** progress log for the stage layer (GUI) */
NOBUG_CPP_DEFINE_FLAG_PARENT    (   stage,                      progress);
/** progress log for the support lib */
NOBUG_CPP_DEFINE_FLAG_PARENT    (   library,                    progress);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    filesys,                   library);       //filesystem operations, opening/closing files...
/** progress log for the common lib */
NOBUG_CPP_DEFINE_FLAG_PARENT    (   common,                     progress);
/** progress log, config subsystem */
NOBUG_CPP_DEFINE_FLAG_PARENT    (    configsys,                 common);        //TODO: here seems to be an ambiguity weather "config" should denote the global config channel or the config-loder internals
NOBUG_CPP_DEFINE_FLAG_PARENT    (     configfiles,              configsys);     //reading, writing, lookup configfiles
NOBUG_CPP_DEFINE_FLAG_PARENT    (     configtyped,              configsys);     //values queried, errors
/** progress log, "External Tree Description" data exchange */
NOBUG_CPP_DEFINE_FLAG_PARENT    (    etd,                       common);
NOBUG_CPP_DEFINE_FLAG_PARENT    (     diff,                     etd);
/** progress log, interfaces */
NOBUG_CPP_DEFINE_FLAG_PARENT    (    interface,                 common);
NOBUG_CPP_DEFINE_FLAG_PARENT    (     interfaceregistry,        common);        //interfaces which get registered/removed
NOBUG_CPP_DEFINE_FLAG_PARENT    (    guifacade,                 common);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    subsystem,                 common);
/** progress log, plugin loader*/
NOBUG_CPP_DEFINE_FLAG_PARENT    (    pluginloader,              common);        //plugins loaded/unloaded/errors
/** progress log, external plugins*/
NOBUG_CPP_DEFINE_FLAG_PARENT    (   plugins,                    progress);
/** base channel flag to track overall working of the render engine */
NOBUG_CPP_DEFINE_FLAG_PARENT    (  render,                      logging);
NOBUG_CPP_DEFINE_FLAG_PARENT    (  config,                      logging);       //TODO: here seems to be an ambiguity weather "config" should denote the global config channel or the config-loder internals
NOBUG_CPP_DEFINE_FLAG_PARENT    (   rules,                      config);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   query,                      config);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   resolver,                   config);

/** base flag for software testing */
NOBUG_CPP_DEFINE_FLAG_PARENT    (  test,                        logging);
/** base flag for synchronisation logging */
NOBUG_CPP_DEFINE_FLAG_PARENT    (  sync,                        logging);       // do we need subsections here? backend_mutex_sync proc_mutex_sync etc?
NOBUG_CPP_DEFINE_FLAG_PARENT    (   mutex_sync,                 sync);          //locking/unlocking mutexes
NOBUG_CPP_DEFINE_FLAG_PARENT    (   cond_sync,                  sync);          //waiting and signalling condition vars
/** base flag for memory related logging */
NOBUG_CPP_DEFINE_FLAG_PARENT    (  memory,                      logging);
/** steam layer memory handling */
NOBUG_CPP_DEFINE_FLAG_PARENT    (   proc_mem,                   memory);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    mobject_mem,               proc_mem);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    builder_mem,               proc_mem);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    asset_mem,                 proc_mem);


/** event which drive the application are separately logged to reconstruct what happened/yielded to a problem */
NOBUG_CPP_DEFINE_FLAG_PARENT    ( events,                       all);
/** caveat joel, you need to implement this */
NOBUG_CPP_DEFINE_FLAG_PARENT    (  gui_event,                   all);



#ifndef LUMIERA_NOBUG_INIT_CPP
#undef NOBUG_DECLARE_ONLY
#define NOBUG_DECLARE_ONLY 0
#endif

#endif
