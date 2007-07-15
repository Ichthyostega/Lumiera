#include <stdio.h>
#include <search.h>
#include <string.h>
#include <dlfcn.h>

#include "plugin.h"

/* TODO should be set by the build system to the actual plugin path */
#define CINELERRA_PLUGIN_PATH "~/.cinelerra3/plugins:/usr/local/lib/cinelerra3/plugins"

/* recognized extensions */
#define CINELERRA_PLUGIN_EXT "plugin:so"

/*
  For future:
  #define CINELERRA_PLUGIN_EXT "plugin:so:cplugin:c"
  imagine using C source plugins compiled with libtcc on the fly
*/

NOBUG_DEFINE_FLAG (cinelerra_plugin);

/* errors */
const char* CINELERRA_PLUGIN_SUCCESS = NULL;
const char* CINELERRA_PLUGIN_EALLOC = "Memory allocation failure";
const char* CINELERRA_PLUGIN_ENFOUND = "No such plugin";
const char* CINELERRA_PLUGIN_ENIFACE = "No such interface";
const char* CINELERRA_PLUGIN_EREVISION = "Interface revision too old";

struct cinelerra_plugin
{
  /* short name as queried ("effects/audio/normalize") used for sorting/finding */
  const char* name;

  /* long names as looked up ("/usr/local/lib/cinelerra3/plugins/effects/audio/normalize.so") */
  const char* pathname;

  /* pointer to the extension part of 'pathname' */
  const char* ext;

  /* use count for all interfaces of this plugin */
  unsigned use_count;

  /* time when the last open or close action happened */
  time_t last;

  /* dlopen handle */
  void* handle;
};

/* global plugin registry */
void* cinelerra_plugin_registry = NULL;

/* plugin operations are protected by one big mutex */
pthread_mutex_t cinelerra_plugin_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Thread local storage, for now only the error state */
static pthread_key_t cinelerra_plugin_tls_error;
static pthread_once_t cinelerra_plugin_initialized = PTHREAD_ONCE_INIT;

void
cinelerra_plugin_tls_init (void)
{
  pthread_key_create (&cinelerra_plugin_tls_error, NULL);
}

/* the compare function for the registry tree */
static int
cinelerra_plugin_name_cmp (const void* a, const void* b)
{
  return strcmp (((struct cinelerra_plugin*) a)->name, ((struct cinelerra_plugin*) b)->name);
}

const char*
cinelerra_plugin_lookup (struct cinelerra_plugin* self, const char* name, const char* path, const char* ext)
{
  /*for each in path*/
  /*for each extension*/
  /*try to find name*/

}

struct cinelerra_interface*
cinelerra_interface_open (const char* name, const char* interface, size_t min_revision)
{
  REQUIRE (min_revision > sizeof(cinelerra_interface), "try to use an empty interface eh?");
  REQUIRE (interface, "interface name must be given");

  pthread_mutex_lock (&cinelerra_plugin_mutex);

  pthread_once (&cinelerra_plugin_initialized, cinelerra_plugin_tls_init);

  struct cinelerra_plugin plugin;
  struct cinelerra_plugin** found;

  // if (name) (NULL means main app)

  /*lookup for $CINELERRA_PLUGIN_PATH*/

  /* else lookup for -DCINELERRA_PLUGIN_PATH */
#ifdef CINELERRA_PLUGIN_PATH
#endif
  /* else fail */

  plugin.name = name;

  found = tsearch (&plugin, &cinelerra_plugin_registry, cinelerra_plugin_name_cmp);
  if (!found)
    goto ealloc0;

  if (*found == &plugin)
    {
      NOTICE (cinelerra_plugin, "new plugin");

      /* create new item */
      *found = malloc (sizeof (struct cinelerra_plugin));
      if (!*found)
        goto ealloc1;

      (*found)->name = strdup (name);
      if (!(*found)->name)
        goto ealloc2;

      (*found)->use_count = 0;

      (*found)->handle = dlopen (name, RTLD_NOW|RTLD_LOCAL);
      if (!(*found)->handle)
        {
          ERROR (cinelerra_plugin, "dlopen failed: %s", dlerror());
          goto edlopen;
        }

      /* if the plugin defines a 'cinelerra_plugin_init' function, we call it, must return !0 on success */
      int (*init)(void) = dlsym((*found)->handle, "cinelerra_plugin_init");
      if (init && init())
        {
          ERROR (cinelerra_plugin, "cinelerra_plugin_init failed: %s: %s", name, interface);
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
      goto edlsym;
    }

  /* is the interface older than required? */
  if (ret->size < min_revision)
    {
      ERROR (cinelerra_plugin, "plugin %s provides older interface %s revision than required", name, interface);
      goto erevision;
    }

  ret->plugin = *found;

  /* if the interface provides a 'open' function, call it now, must return !0 on success */
  if (ret->open && ret->open())
    {
      ERROR (cinelerra_plugin, "open hook indicated an error");
      goto eopen;
    }

  (*found)->use_count++;
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
  free ((char*)(*found)->name);
 ealloc2:
  free (*found);
 ealloc1:
  *found = &plugin;
  tdelete (&plugin, &cinelerra_plugin_registry, cinelerra_plugin_name_cmp);
 ealloc0:
  pthread_mutex_lock (&cinelerra_plugin_mutex);
  return NULL;
}

void
cinelerra_interface_close (struct cinelerra_interface* self)
{
  if(!self)
    return;

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
