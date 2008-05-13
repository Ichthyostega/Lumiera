/*
  plugin.c  -  Lumiera Plugin loader

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include <stdio.h>
#include <unistd.h>
#include <search.h>
#include <string.h>
#include <dlfcn.h>
#include <pthread.h>
#include <time.h>
#include <limits.h>

#include "plugin.h"
#include "safeclib.h"

/**
 * @file Plugin loader.
 */


/* TODO should be set by the build system to the actual plugin path */
#define LUMIERA_PLUGIN_PATH "~/.lumiera/plugins:/usr/local/lib/lumiera/plugins:.libs"

NOBUG_DEFINE_FLAG (lumiera_plugin);

/* errors */
LUMIERA_ERROR_DEFINE(PLUGIN_DLOPEN, "Could not open plugin");
LUMIERA_ERROR_DEFINE(PLUGIN_HOOK, "Hook function failed");
LUMIERA_ERROR_DEFINE(PLUGIN_NFILE, "No such plugin");
LUMIERA_ERROR_DEFINE(PLUGIN_NIFACE, "No such interface");
LUMIERA_ERROR_DEFINE(PLUGIN_REVISION, "Interface revision too old");

/*
  supported (planned) plugin types and their file extensions
*/

enum lumiera_plugin_type
  {
    LUMIERA_PLUGIN_NULL,
    LUMIERA_PLUGIN_DYNLIB,
    LUMIERA_PLUGIN_CSOURCE
  };

static const struct
{
  const char* const ext;
  enum lumiera_plugin_type type;
} lumiera_plugin_extensions [] =
  {
    {"so",       LUMIERA_PLUGIN_DYNLIB},
    {"o",        LUMIERA_PLUGIN_DYNLIB},
    {"c",        LUMIERA_PLUGIN_CSOURCE},
    /* extend here */
    {NULL,       LUMIERA_PLUGIN_NULL}
  };


struct lumiera_plugin
{
  /* short name as queried ("effects/audio/normalize") used for sorting/finding */
  const char* name;

  /* long names as looked up ("/usr/local/lib/lumiera/plugins/effects/audio/normalize.so") */
  const char* pathname;

  /* use count for all interfaces of this plugin */
  unsigned use_count;

  /* time when the last open or close action happened */
  time_t last;

  /* kind of plugin */
  enum lumiera_plugin_type type;

  /* dlopen handle */
  void* handle;
};


/* global plugin registry */
void* lumiera_plugin_registry = NULL;

/* plugin operations are protected by one big mutex */
pthread_mutex_t lumiera_plugin_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * the compare function for the registry tree.
 * Compares the names of two struct lumiera_plugin.
 * @return 0 if a and b are equal, just like strcmp. */
static int
lumiera_plugin_name_cmp (const void* a, const void* b)
{
  return strcmp (((struct lumiera_plugin*) a)->name, ((struct lumiera_plugin*) b)->name);
}

/**
 * Initialize the plugin system.
 * always succeeds or aborts
 */
void
lumiera_init_plugin (void)
{
  NOBUG_INIT_FLAG (lumiera_plugin);
}

/**
 * Find and set pathname for the plugin.
 * Searches through given path for given plugin, trying to find the file's location in the filesystem.
 * If found, self->pathname will be set to the found plugin file.
 * @param self The lumiera_plugin to open look for.
 * @param path The path to search trough (paths seperated by ":")
 * @return 0 on success. -1 on error, or if plugin not found in path.
 */
int
lumiera_plugin_lookup (struct lumiera_plugin* self, const char* path)
{
  if (!path)
    return -1;

  if (strlen(path) > 1023)
    return -1;        /*TODO error handling*/

  char tpath[1024];
  TODO("dunno if PATH_MAX may be undefined (in case arbitary lengths are supported), I check that later");
  char pathname[1024] = {0};
  char* tmp;

  strcpy(tpath, path);

  /*for each in path*/
  for (char* tok = strtok_r (tpath, ":", &tmp); tok; tok = strtok_r (NULL, ":", &tmp))
    {
      /*for each extension*/
      for (int i = 0; lumiera_plugin_extensions[i].ext; ++i)
        {
          /* path/name.extension */
          int r = snprintf(pathname, 1024, "%s/%s.%s", tok, self->name, lumiera_plugin_extensions[i].ext);
          if (r >= 1024)
            return -1; /*TODO error handling, name too long*/

          TRACE (lumiera_plugin, "trying %s", pathname);

          if (!access(pathname, R_OK))
            {
              /* got it */
              TRACE (lumiera_plugin, "found %s", pathname);
              self->pathname = lumiera_strndup (pathname, PATH_MAX);

              self->type = lumiera_plugin_extensions[i].type;
              return 0;
            }
        }
    }
  return -1; /* plugin not found */
}


/**
 * Make an interface available.
 * To use an interface provided by a plugin it must be opened first. It is allowed to open an interface 
 * more than once. Each open must be paired with a close.
 * @param name name of the plugin to use.
 * @param interface name of the interface to open.
 * @param min_revision the size of the interface structure is used as measure of a minimal required 
 * revision (new functions are appended at the end)
 * @return handle to the interface or NULL in case of a error. The application shall cast this handle to
 * the actual interface type.
 */
struct lumiera_interface*
lumiera_interface_open (const char* name, const char* interface, size_t min_revision)
{
  //REQUIRE (min_revision > sizeof(struct lumiera_interface), "try to use an empty interface eh?");
  REQUIRE (interface, "interface name must be given");

  pthread_mutex_lock (&lumiera_plugin_mutex);

  struct lumiera_plugin plugin;
  struct lumiera_plugin** found;

  plugin.name = name; /* for searching */

  found = tsearch (&plugin, &lumiera_plugin_registry, lumiera_plugin_name_cmp);
  if (!found)
    LUMIERA_DIE (NO_MEMORY);

  if (*found == &plugin)
    {
      NOTICE (lumiera_plugin, "new plugin");

      /* now really create new item */
      *found = lumiera_malloc (sizeof (struct lumiera_plugin));

      if (name) /* NULL is main app, no lookup needed */
        {
          /*lookup for $LUMIERA_PLUGIN_PATH*/
          (*found)->name = lumiera_strndup (name, PATH_MAX);

          if (!!lumiera_plugin_lookup (*found, getenv("LUMIERA_PLUGIN_PATH"))
#ifdef LUMIERA_PLUGIN_PATH
              /* else lookup for -DLUMIERA_PLUGIN_PATH */
              && !!lumiera_plugin_lookup (*found, LUMIERA_PLUGIN_PATH)
#endif
              )
            {
              LUMIERA_ERROR_SET (lumiera_plugin, PLUGIN_NFILE);
              goto elookup;
            }
        }
      else
        {
          (*found)->name = NULL;
          (*found)->pathname = NULL;
        }

      (*found)->use_count = 0;

      PLANNED("if .so like then dlopen; else if .c like tcc compile");
      TODO("factor dlopen and dlsym out");

      TRACE(lumiera_plugin, "trying to open %s", (*found)->pathname);

      (*found)->handle = dlopen ((*found)->pathname, RTLD_LAZY|RTLD_LOCAL);
      if (!(*found)->handle)
        {
          ERROR (lumiera_plugin, "dlopen failed: %s", dlerror());
          LUMIERA_ERROR_SET (lumiera_plugin, PLUGIN_DLOPEN);
          goto edlopen;
        }

      /* if the plugin defines a 'lumiera_plugin_init' function, we call it, must return 0 on success */
      int (*init)(void) = dlsym((*found)->handle, "lumiera_plugin_init");
      if (init && init())
        {
          //ERROR (lumiera_plugin, "lumiera_plugin_init failed: %s: %s", name, interface);
          LUMIERA_ERROR_SET (lumiera_plugin, PLUGIN_HOOK);
          goto einit;
        }
    }
  /* we have the plugin, now get the interface descriptor */
  struct lumiera_interface* ret;

  dlerror();
  ret = dlsym ((*found)->handle, interface);

  const char *dlerr = dlerror();
  TRACE(lumiera_plugin, "%s", dlerr);
  TODO ("need some way to tell 'interface not provided by plugin', maybe lumiera_plugin_error()?");
  if (dlerr)
    {
      //ERROR (lumiera_plugin, "plugin %s doesnt provide interface %s", name, interface);
      LUMIERA_ERROR_SET (lumiera_plugin, PLUGIN_NIFACE);
      goto edlsym;
    }

  /* is the interface older than required? */
  if (ret->size < min_revision)
    {
      ERROR (lumiera_plugin, "plugin %s provides older interface %s revision than required", name, interface);
      LUMIERA_ERROR_SET (lumiera_plugin, PLUGIN_REVISION);
      goto erevision;
    }

  ret->plugin = *found;

  /* if the interface provides a 'open' function, call it now, must return 0 on success */
  if (ret->open && ret->open())
    {
      ERROR (lumiera_plugin, "open hook indicated an error");
      LUMIERA_ERROR_SET (lumiera_plugin, PLUGIN_HOOK);
      goto eopen;
    }

  (*found)->use_count++;
  (*found)->last = time (NULL);
  ret->use_count++;

  pthread_mutex_unlock (&lumiera_plugin_mutex);
  return ret;

  /* Error cleanup */
 einit:
  dlclose((*found)->handle);
 eopen:
 erevision:
 edlsym:
 edlopen:
 elookup:
  free ((char*)(*found)->name);
  free (*found);
  *found = &plugin;
  tdelete (&plugin, &lumiera_plugin_registry, lumiera_plugin_name_cmp);
  pthread_mutex_unlock (&lumiera_plugin_mutex);
  return NULL;
}

/**
 * Close an interface. Does not free associated resources
 * Calling this function with self==NULL is legal. Every interface handle must be closed only once.
 * @param ptr interface to be closed
 */
void
lumiera_interface_close (void* ptr)
{
  TRACE (lumiera_plugin, "%p", ptr);
  if(!ptr)
    return;

  struct lumiera_interface* self = (struct lumiera_interface*) ptr;

  pthread_mutex_lock (&lumiera_plugin_mutex);

  struct lumiera_plugin* plugin = self->plugin;
  
  plugin->use_count--;
  self->use_count--;

  /* if the interface provides a 'close' function, call it now */
  if (self->close)
    self->close();

  /* plugin not longer in use, unload it */
  if (!plugin->use_count)
    {
      TODO ("we dont want to close here, instead store time of recent use and make a expire run, already planned in my head");

      /* if the plugin defines a 'lumiera_plugin_destroy' function, we call it */
      int (*destroy)(void) = dlsym(plugin->handle, "lumiera_plugin_destroy");
      if (destroy)
        destroy();

      /* and now cleanup */
      tdelete (plugin, &lumiera_plugin_registry, lumiera_plugin_name_cmp);
      free ((char*)plugin->name);
      dlclose(plugin->handle);
      free (plugin);
    }
  pthread_mutex_unlock (&lumiera_plugin_mutex);
}
