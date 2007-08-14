/*
  plugin.c  -  Cinelerra Plugin loader

  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>

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

#include "plugin.h"

/* TODO should be set by the build system to the actual plugin path */
#define CINELERRA_PLUGIN_PATH "~/.cinelerra3/plugins:/usr/local/lib/cinelerra3/plugins"
#define CINELERRA_PLUGIN_PREFIX "lib"

NOBUG_DEFINE_FLAG (cinelerra_plugin);

/* errors */
CINELERRA_ERROR_DEFINE(PLUGIN_DLOPEN, "Could not open plugin");
CINELERRA_ERROR_DEFINE(PLUGIN_HOOK, "Hook function failed");
CINELERRA_ERROR_DEFINE(PLUGIN_NFILE, "No such plugin");
CINELERRA_ERROR_DEFINE(PLUGIN_NIFACE, "No such interface");
CINELERRA_ERROR_DEFINE(PLUGIN_REVISION, "Interface revision too old");

/*
  supported (planned) plugin types and their file extensions
*/

enum cinelerra_plugin_type
  {
    CINELERRA_PLUGIN_NULL,
    CINELERRA_PLUGIN_DYNLIB,
    CINELERRA_PLUGIN_CSOURCE
  };

static const struct
{
  const char* const ext;
  enum cinelerra_plugin_type type;
} cinelerra_plugin_extensions [] =
  {
    {"plugin",   CINELERRA_PLUGIN_DYNLIB},
    {"so",       CINELERRA_PLUGIN_DYNLIB},
    {"o",        CINELERRA_PLUGIN_DYNLIB},
    {"tcc",      CINELERRA_PLUGIN_CSOURCE},
    {"c",        CINELERRA_PLUGIN_CSOURCE},
    /* extend here */
    {NULL,       CINELERRA_PLUGIN_NULL}
  };


struct cinelerra_plugin
{
  /* short name as queried ("effects/audio/normalize") used for sorting/finding */
  const char* name;

  /* long names as looked up ("/usr/local/lib/cinelerra3/plugins/effects/audio/normalize.so") */
  const char* pathname;

  /* use count for all interfaces of this plugin */
  unsigned use_count;

  /* time when the last open or close action happened */
  time_t last;

  /* kind of plugin */
  enum cinelerra_plugin_type type;

  /* dlopen handle */
  void* handle;
};


/* global plugin registry */
void* cinelerra_plugin_registry = NULL;

/* plugin operations are protected by one big mutex */
pthread_mutex_t cinelerra_plugin_mutex = PTHREAD_MUTEX_INITIALIZER;

/* the compare function for the registry tree */
static int
cinelerra_plugin_name_cmp (const void* a, const void* b)
{
  return strcmp (((struct cinelerra_plugin*) a)->name, ((struct cinelerra_plugin*) b)->name);
}


int
cinelerra_plugin_lookup (struct cinelerra_plugin* self, const char* path)
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
      for (int i = 0; cinelerra_plugin_extensions[i].ext; ++i)
        {
          /* path/name.extension */
          int r = snprintf(pathname, 1024, CINELERRA_PLUGIN_PREFIX "%s/%s.%s", tok, self->name, cinelerra_plugin_extensions[i].ext);
          if (r >= 1024)
            return -1; /*TODO error handling, name too long*/

          TRACE (cinelerra_plugin, "trying %s", pathname);

          if (!access(pathname, R_OK))
            {
              /* got it */
              self->pathname = strdup (pathname);
              if (!self->pathname) CINELERRA_DIE("out of memory");
              self->type = cinelerra_plugin_extensions[i].type;
              return 0;
            }
        }
    }
  return -1; /* plugin not found */
}

struct cinelerra_interface*
cinelerra_interface_open (const char* name, const char* interface, size_t min_revision)
{
  //REQUIRE (min_revision > sizeof(struct cinelerra_interface), "try to use an empty interface eh?");
  REQUIRE (interface, "interface name must be given");

  pthread_mutex_lock (&cinelerra_plugin_mutex);

  struct cinelerra_plugin plugin;
  struct cinelerra_plugin** found;

  plugin.name = name; /* for searching */

  found = tsearch (&plugin, &cinelerra_plugin_registry, cinelerra_plugin_name_cmp);
  if (!found) CINELERRA_DIE("out of memory");

  if (*found == &plugin)
    {
      NOTICE (cinelerra_plugin, "new plugin");

      /* now really create new item */
      *found = malloc (sizeof (struct cinelerra_plugin));
      if (!*found) CINELERRA_DIE("out of memory");

      if (name) /* NULL is main app, no lookup needed */
        {
          /*lookup for $CINELERRA_PLUGIN_PATH*/
          (*found)->name = strdup (name);
          if (!(*found)->name) CINELERRA_DIE("out of memory");

          if (!cinelerra_plugin_lookup (*found, getenv("CINELERRA_PLUGIN_PATH"))
#ifdef CINELERRA_PLUGIN_PATH
              /* else lookup for -DCINELERRA_PLUGIN_PATH */
              || !cinelerra_plugin_lookup (&plugin, CINELERRA_PLUGIN_PATH)
#endif
              )
            {
              cinelerra_error_set (CINELERRA_ERROR_PLUGIN_NFILE);
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

      (*found)->handle = dlopen ((*found)->pathname, RTLD_NOW|RTLD_LOCAL);
      if (!(*found)->handle)
        {
          ERROR (cinelerra_plugin, "dlopen failed: %s", dlerror());
          cinelerra_error_set (CINELERRA_ERROR_PLUGIN_DLOPEN);
          goto edlopen;
        }

      /* if the plugin defines a 'cinelerra_plugin_init' function, we call it, must return 0 on success */
      int (*init)(void) = dlsym((*found)->handle, "cinelerra_plugin_init");
      if (init && init())
        {
          ERROR (cinelerra_plugin, "cinelerra_plugin_init failed: %s: %s", name, interface);
          cinelerra_error_set (CINELERRA_ERROR_PLUGIN_HOOK);
          goto einit;
        }
    }
  /* we have the plugin, now get the interface descriptor */
  struct cinelerra_interface* ret;

  ret = dlsym ((*found)->handle, interface);
  TODO ("need some way to tell 'interface not provided by plugin', maybe cinelerra_plugin_error()?");
  if (!ret)
    {
      ERROR (cinelerra_plugin, "plugin %s doesnt provide interface %s", name, interface);
      cinelerra_error_set (CINELERRA_ERROR_PLUGIN_NIFACE);
      goto edlsym;
    }

  /* is the interface older than required? */
  if (ret->size < min_revision)
    {
      ERROR (cinelerra_plugin, "plugin %s provides older interface %s revision than required", name, interface);
      cinelerra_error_set (CINELERRA_ERROR_PLUGIN_REVISION);
      goto erevision;
    }

  ret->plugin = *found;

  /* if the interface provides a 'open' function, call it now, must return 0 on success */
  if (ret->open && ret->open())
    {
      ERROR (cinelerra_plugin, "open hook indicated an error");
      cinelerra_error_set (CINELERRA_ERROR_PLUGIN_HOOK);
      goto eopen;
    }

  (*found)->use_count++;
  (*found)->last = time (NULL);
  ret->use_count++;

  pthread_mutex_unlock (&cinelerra_plugin_mutex);
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
  tdelete (&plugin, &cinelerra_plugin_registry, cinelerra_plugin_name_cmp);
  pthread_mutex_lock (&cinelerra_plugin_mutex);
  return NULL;
}

void
cinelerra_interface_close (void* ptr)
{
  if(!ptr)
    return;

  struct cinelerra_interface* self = (struct cinelerra_interface*) ptr;

  pthread_mutex_lock (&cinelerra_plugin_mutex);

  struct cinelerra_plugin* plugin = self->plugin;
  
  plugin->use_count--;
  self->use_count--;

  /* if the interface provides a 'close' function, call it now */
  if (self->close)
    self->close();

  /* plugin not longer in use, unload it */
  if (!plugin->use_count)
    {
      TODO ("we dont want to close here, instead store time of recent use and make a expire run, already planned in my head");

      /* if the plugin defines a 'cinelerra_plugin_destroy' function, we call it */
      int (*destroy)(void) = dlsym(plugin->handle, "cinelerra_plugin_destroy");
      if (destroy)
        destroy();

      /* and now cleanup */
      tdelete (plugin, &cinelerra_plugin_registry, cinelerra_plugin_name_cmp);
      free ((char*)plugin->name);
      dlclose(plugin->handle);
      free (plugin);
    }
  pthread_mutex_unlock (&cinelerra_plugin_mutex);
}
