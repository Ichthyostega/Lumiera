/*
  interface.c  -  Lumiera interface api

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

#include "lib/mutex.h"
#include "lib/safeclib.h"

#include "backend/interface.h"

#include "backend/interfaceregistry.h"

#include <nobug.h>

/**
 * @file
 * From a programmers perspective interfaces only need to be opened when needed and closed
 * when finished with them. There is no difference if the interface is internally provided
 * by the core or provided by an external plugin.
 * Interfaces can be opened multiple times and cross reference each other.
 */


static LumieraInterfacenode
lumiera_interface_open_interfacenode (LumieraInterfacenode self);

static void
lumiera_interfacenode_close (LumieraInterfacenode self);


LumieraInterface
lumiera_interface_open (const char* interface, unsigned version, size_t minminorversion, const char* name)
{
  LumieraInterfacenode self = NULL;
  TRACE (interface, "%s", name);

  LUMIERA_RECMUTEX_SECTION (interfaceregistry, &lumiera_interface_mutex)
    {
      self = lumiera_interfaceregistry_interfacenode_find (interface, version, name);

      if (!self)
        {
          UNIMPLEMENTED ("query plugindb and load plugin if exists");
        }

      if (self)
        {
          if (minminorversion > self->interface->size)
            {
              UNIMPLEMENTED ("set error");
              self = NULL;
            }
          else
            {
              self = lumiera_interface_open_interfacenode (self);
            }
        }
    }
  return self->interface;
}


static void
push_dependency (LumieraInterfacenode parent, LumieraInterfacenode child)
{
  /* push a dependency on the dependency array, allcoate or resize it on demand */
  TRACE (interface, "%s %s", parent->interface->name, child->interface->name);

  /* no dependencies recorded yet, alloc a first block for 4 pointers */
  if (!parent->deps_size)
    parent->deps = lumiera_calloc (parent->deps_size = 4, sizeof (LumieraInterfacenode));

  size_t sz = parent->deps_size;
  LumieraInterfacenode* itr = parent->deps;

  while (*itr)
    {
      --sz;
      ++itr;
      if (sz == 1)
        {
          /* block to small, realloc it with twice its size, we keep the block NULL terminated */
          sz = parent->deps_size + 1;
          parent->deps_size *= 2;
          parent->deps = lumiera_realloc (parent->deps, parent->deps_size * sizeof (LumieraInterface));
          itr = parent->deps + sz - 2;
          memset (itr, 0, sz * sizeof (LumieraInterface));
        }
    }

  /* found free element, store self in dependencies */
  *itr = child;
}


static void
depwalk (LumieraInterfacenode self, LumieraInterfacenode* stack)
{
  /* increment refcount for all non-cyclic dependencies recursively */
  if (self->deps)
    {
      TRACE (interface, "%s %d", self->interface->name, self->refcnt);
      for (LumieraInterfacenode* dep = self->deps; *dep; ++dep)
        {
          TRACE (interface, "loop %s", (*dep)->interface->name);
          int cycle = 0;
          for (LumieraInterfacenode itr = *stack; itr; itr = itr->lnk)
            {
              if (itr == *dep)
                {
                  TRACE (interface, "CYCLE");
                  cycle = 1;
                  break;
                }
            }

          if (!cycle)
            {
              ++(*dep)->refcnt;

              (*dep)->lnk = *stack;
              *stack = *dep;

              depwalk (*dep, stack);

              *stack = (*dep)->lnk;
              (*dep)->lnk = NULL;
            }
        }
    }
}


static LumieraInterfacenode
lumiera_interface_open_interfacenode (LumieraInterfacenode self)
{
  static unsigned collect_dependencies = 0;
  static LumieraInterfacenode stack = NULL;

  /*
    Ok, this got little more complicated than it should be,
    but finally it handles any kind of cross dependencies between interfaces gracefully
   */

  if (self)
    {
      TRACE (interface, "%s %d (%s)", self->interface->name, self->refcnt, stack?stack->interface->name:"");

      LUMIERA_RECMUTEX_SECTION (interfaceregistry, &lumiera_interface_mutex)
        {
          /* discover cycles, cycles don't refcount! */
          int cycle = 0;

          for (LumieraInterfacenode itr = stack; itr; itr = itr->lnk)
            {
              if (itr == self)
                {
                  TRACE (interface, "CYCLE");
                  cycle = 1;
                  break;
                }
            }

          /* 'stack' is ensured to be !NULL here because only a parent call can switch collect_dependencies on */
          if (collect_dependencies)
            push_dependency (stack, self);

          if (!cycle)
            {
              ++self->refcnt;
              self->lnk = stack;
              stack = self;
              int collect_dependencies_bak = collect_dependencies;

              if (self->refcnt == 1)
                {
                  /* first opening, run acquire, recursive opening shall record its dependencies here */
                  if (self->interface->acquire)
                    {
                      TRACE (interface, "Acquire %s", self->interface->name);
                      collect_dependencies = self->deps?0:1;
                      self->interface = self->interface->acquire (self->interface);
                    }
                }
              else
                {
                  /* opening again recurse dependencies */
                  collect_dependencies = 0;
                  depwalk (self, &stack);
                }

              collect_dependencies = collect_dependencies_bak;
              stack = self->lnk;
              self->lnk = NULL;
            }
        }
    }

  return self;
}


void
lumiera_interface_close (LumieraInterface self)
{
  TRACE (interface);

  LUMIERA_RECMUTEX_SECTION (interfaceregistry, &lumiera_interface_mutex)
    {
      lumiera_interfacenode_close ((LumieraInterfacenode)psplay_find (lumiera_interfaceregistry, self, 100));
    }
}


/* internal function, does no locking! */
static void
lumiera_interfacenode_close (LumieraInterfacenode self)
{
  static LumieraInterfacenode stack = NULL;

  if (!self)
    return;

  TRACE (interface, "%s %d (%s)", self->interface->name, self->refcnt, stack?stack->interface->name:"");

  REQUIRE (self->refcnt);

  int cycle = 0;

  for (LumieraInterfacenode itr = stack; itr; itr = itr->lnk)
    {
      if (itr == self)
        {
          TRACE (interface, "CYCLE");
          cycle = 1;
          break;
        }
    }

  if (!cycle)
    {
      self->lnk = stack;
      stack = self;

      if (self->refcnt == 1)
        {
          if (self->interface->release)
            {
              TRACE (interface, "Release %s", self->interface->name);
              self->interface->release (self->interface);
            }
        }
      else
        {
          if (self->deps)
            {
              TRACE (interface, "Recurse %s %d", self->interface->name, self->refcnt);

              for (LumieraInterfacenode* dep = self->deps; *dep; ++dep)
                lumiera_interfacenode_close (*dep);
            }
        }

      stack = self->lnk;
      self->lnk = NULL;
      --self->refcnt;
    }
}


/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
