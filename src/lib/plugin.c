#include <stdio.h>
#include <search.h>
#include <string.h>
#include <dlfcn.h>

#include "plugin.h"

NOBUG_DEFINE_FLAG (cinelerra_plugin);


struct cinelerra_plugin
{
  const char* name; /*TODO store full and short name, sort by shortname?*/

  /* use count for all interfaces of this plugin */
  unsigned use_count;

  time_t last;

  /* dlopen handle */
  void* handle;
};

/* global plugin registry */
void* cinelerra_plugin_registry = NULL;


static int
cmp_plugin_name (const void* a, const void* b)
{
  return strcmp (((struct cinelerra_plugin*) a)->name, ((struct cinelerra_plugin*) b)->name);
}

struct cinelerra_interface*
cinelerra_interface_open (const char* name, const char* interface, size_t min_revision)
{
  REQUIRE (min_revision > sizeof(cinelerra_interface), "try to use an empty interface eh?");
  REQUIRE (interface, "interface name must be defined");

  struct cinelerra_plugin plugin;
  struct cinelerra_plugin** found;

  // TODO cook somewhere (pluginpath+name+.so)
  plugin.name = name;

  found = tsearch (&plugin, &cinelerra_plugin_registry, cmp_plugin_name);
  if (!found)
    return NULL;

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
  tdelete (&plugin, &cinelerra_plugin_registry, cmp_plugin_name);
  return NULL;
}

void
cinelerra_interface_close (struct cinelerra_interface* self)
{
  if(!self)
    return;

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
      tdelete (plugin, &cinelerra_plugin_registry, cmp_plugin_name);
      free ((char*)plugin->name);
      dlclose(plugin->handle);
      free (plugin);
    }
}
