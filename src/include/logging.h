/*
  logging.h  -  Configure basic nobug logging

  Copyright (C)         Lumiera.org
    2008, 2009          Christian Thaeter <ct@pipapo.org>
                        Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef LUMIERA_LOGGING_H
#define LUMIERA_LOGGING_H

/** @file logging.h
 ** This header is for including and configuring NoBug.
 ** The idea is that configuration and some commonly used flag
 ** declarations are to be kept in one central location. Subsystems
 ** are free to define and use additional flags for local use. Typically,
 ** this header will be included via some of the basic headers like error.hpp,
 ** which in turn gets included e.g. by proc/common.hpp
 **
 ** This header can thus be assumed to be effectively global. It should contain
 ** only declarations of global relevance, as any change causes the whole project
 ** to be rebuilt. All flags defined here are initialised automatically.
 **
 ** We use the 'NOBUG_DECLARATIONS_ONLY' magic to generate declarations and
 ** definitions only out of this header.
 **
 ** @par Logging configuration
 ** By default, logging is configured such as to emit a small number of informative
 ** messages on the starting terminal and to report fatal errors. But besides the
 ** usual fine-grained tracing messages, we define a small number of distinct
 ** thematic <b>Logging Channels</b> providing a consistent high-level view of
 ** what is going on with regards to a specific aspect of the application
 ** - \c progress documents a high-level overall view of what the application \em does
 ** - \c render focuses on the working of the render engine (without logging each frame)
 ** - \c config shows anything of relevance regarding the configured state of App and session
 ** - \c memory allows to diagnose a high-level view of memory management
 **
 ** Any log level can be overridden by an environment variable, for example
 ** \code NOBUG_LOG='progress:INFO' ./lumiera \endcode
 **
 ** @todo logging to files?    NOBUG_LOG='progress:INFO@file(name=filename)' api to set this statically up by the program will follow  --cehteh
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
/** base of debug logging for the backend */
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

/** base of debug logging for the proc layer */
NOBUG_CPP_DEFINE_FLAG_PARENT    (  proc_dbg,                    debugging);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   command_dbg,                proc_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   session_dbg,                proc_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   player_dbg,                 proc_dbg);
NOBUG_CPP_DEFINE_FLAG_PARENT    (   engine_dbg,                 proc_dbg);
/** base of debug logging for the gui */
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
/** progress log for the backend */
NOBUG_CPP_DEFINE_FLAG_PARENT    (   backend,                    progress);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    file,                      backend);       //opening/closing files etc
NOBUG_CPP_DEFINE_FLAG_PARENT    (    mmap,                      backend);       //mmap errors
NOBUG_CPP_DEFINE_FLAG_PARENT    (    thread,                    backend);       //starting/stopping threads
NOBUG_CPP_DEFINE_FLAG_PARENT    (     threads,                  thread);
NOBUG_CPP_DEFINE_FLAG_PARENT    (     threadpool,               thread);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    fileheader,                backend);
/** progress log for the proc layer */
NOBUG_CPP_DEFINE_FLAG_PARENT    (   proc,                       progress);
/** progress log for proc-layer command dispatch */
NOBUG_CPP_DEFINE_FLAG_PARENT    (    command,                   proc);
/** progress log for session datastructure */
NOBUG_CPP_DEFINE_FLAG_PARENT    (    session,                   proc);
/** progress log for the builder and build process */
NOBUG_CPP_DEFINE_FLAG_PARENT    (    builder,                   proc);
/** progress log for running the engine */
NOBUG_CPP_DEFINE_FLAG_PARENT    (    engine,                    proc);
/** progress log for play- and render subsystem */
NOBUG_CPP_DEFINE_FLAG_PARENT    (    play,                      proc);
/** progress log for the gui */
NOBUG_CPP_DEFINE_FLAG_PARENT    (   gui,                        progress);
/** progress log for the support lib */
NOBUG_CPP_DEFINE_FLAG_PARENT    (   library,                    progress);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    resourcecollector,         library);
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
/** proc layer memory handling */
NOBUG_CPP_DEFINE_FLAG_PARENT    (   proc_mem,                   memory);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    mobject_mem,               proc_mem);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    builder_mem,               proc_mem);
NOBUG_CPP_DEFINE_FLAG_PARENT    (    asset_mem,                 proc_mem);


/** event which drive the application are separately logged to reconstruct what happend/yielded to a problem */
NOBUG_CPP_DEFINE_FLAG_PARENT    ( events,                       all);
/** caveat joel, you need to implement this */
NOBUG_CPP_DEFINE_FLAG_PARENT    (  gui_event,                   all);



#ifndef LUMIERA_NOBUG_INIT_CPP
#undef NOBUG_DECLARE_ONLY
#define NOBUG_DECLARE_ONLY 0
#endif

#endif
