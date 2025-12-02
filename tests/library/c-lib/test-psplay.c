/*
  TEST-PSPLAY  -  test the probabilistic splay tree

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file test-psplay.c
 ** C unit test to cover a probabilistic splay tree library implementation.
 ** These tree data structures are used as key-value store in several of the
 ** management facilities of the vault
 ** @see psplay.h
 */


#include <time.h>
#include <stdlib.h>
#include <nobug.h>

#include "lib/psplay.h"
#include "lib/test/test.h"

struct testitem
{
  psplaynode node;
  char* key;
};
typedef struct testitem* TestItem;

TestItem
testitem_new (const char* str)
{
  TestItem self = malloc (sizeof *self);
  psplaynode_init (&self->node);
  self->key = strdup (str);
  return self;
}

void
testitem_delete (TestItem self)
{
  free (self->key);
  free (self);
}



static int
cmp_fn (const void*, const void*);

static const void*
key_fn (const PSplaynode);

static void
delete_fn (PSplaynode);

//static psplay_delete_fn
//action_fn (PSplaynode node, const enum psplay_order_e which, int level, void* data);

static int
fcmp_fn (const void*, const void*);

static const void*
fkey_fn (const PSplaynode);

static void
fdelete_fn (PSplaynode);

//static psplay_delete_fn
//action_fn (PSplaynode node, const enum psplay_order_e which, int level, void* data);


psplay_delete_fn
testitem_print_node (PSplaynode node, const enum psplay_order_enum which, int level, void* data)
{
  FILE* fh = data;
  static char* sp = "                                        ";
  if (level>40)
    {
      if (which == PSPLAY_PREORDER)
        fprintf (fh, "%s ...\n", sp);
      return PSPLAY_CONT;
    }

  switch (which)
    {
    case PSPLAY_PREORDER:
      fprintf (fh, "%s%p '%s'\n", sp+40-level, node, ((TestItem)node)->key);
      if (node->left) fprintf (fh, "%sleft %p '%s'\n", sp+40-level, node->left, ((TestItem)node->left)->key);
      break;
    case PSPLAY_INORDER:
      if (node->right) fprintf (fh, "%sright %p '%s'\n", sp+40-level, node->right, ((TestItem)node->right)->key);
      break;
    case PSPLAY_POSTORDER:
      break;
    }

  return PSPLAY_CONT;
}

void
testitem_dump (PSplay self, FILE* dest)
{
  fprintf (dest, "root %p '%s'\n", self->tree, self->tree?((TestItem)self->tree)->key:"EMPTY");
  psplay_walk (self, NULL, testitem_print_node, 0, dest);
  fprintf (dest, "\n");
}

psplay_delete_fn
testitem_graphvizprint_node (PSplaynode node, const enum psplay_order_enum which, int level, void* data)
{
  FILE* fh = data;

  switch (which)
    {
    case PSPLAY_PREORDER:
      if (node->left)
          fprintf (fh, "\t\"%p:%s\":sw -> \"%p:%s\":ne;\n",
                   node,
                   ((TestItem)node)->key,
                   node->left,
                   ((TestItem)node->left)->key);
      break;
    case PSPLAY_INORDER:
      if (node->right)
          fprintf (fh, "\t\"%p:%s\":se -> \"%p:%s\":nw;\n",
                   node,
                   ((TestItem)node)->key,
                   node->right,
                   ((TestItem)node->right)->key);
      break;
    case PSPLAY_POSTORDER:
      break;
    }

  return PSPLAY_CONT;
}



void
testitem_graphvizdump (PSplay self, FILE* dest)
{
  static int cnt = 0;
  if (!cnt) cnt = (time(NULL) % 1000) * 100;
  char cmd[256];

  sprintf(cmd,"dot -Tps >/var/tmp/dbg%d.ps; gv /var/tmp/dbg%d.ps",cnt,cnt);
  FILE * graph = popen(cmd, "w");

  fprintf(graph, "digraph \"%s\" { center=true; size=\"6,6\"; node [color=lightblue2, style=filled];", "psplay");
  ++cnt;

  fprintf(graph, "\t\"root\":s -> \"%p:%s\":n;\n",
          self->tree, self->tree?((TestItem)self->tree)->key:"EMPTY");

  psplay_walk (self, NULL, testitem_graphvizprint_node, 0, graph);

  fprintf(graph, "}");

  pclose(graph);
}



TESTS_BEGIN

TEST ("basic")
{
  psplay splay_tree;
  psplay_init (&splay_tree, cmp_fn, key_fn, delete_fn);

  psplay_dump (&splay_tree, stdout);

  psplay_destroy (&splay_tree);
}


TEST ("basic_insert_dump")
{
  CHECK (argv[2]);

  psplay splay_tree;
  psplay_init (&splay_tree, cmp_fn, key_fn, delete_fn);

  int end = atoi (argv[2]);

  char key[1024];

  for (int i = 1; i <= end; ++i)
    {
      sprintf (key, "%d", i);
      ECHO ("insert %s", key);
      psplay_insert (&splay_tree, (PSplaynode)testitem_new (key), 100);
    }

  psplay_dump (&splay_tree, stderr);

#if 0
  for (int i = 1; i <= end; ++i)
    {
      sprintf (key, "%d", i);
      ECHO ("insert %s", key);
      psplay_remove_key (&splay_tree, key);
      psplay_dump (&splay_tree, stderr);
    }
  for (int i = end; i; --i)
    {
      sprintf (key, "%d", i);
      ECHO ("insert %s", key);
      psplay_remove_key (&splay_tree, key);
      psplay_dump (&splay_tree, stderr);
    }
#endif

  psplay_destroy (&splay_tree);
  printf ("done\n");
}


TEST ("insert_find")
{
  psplay splay_tree;
  psplay_init (&splay_tree, cmp_fn, key_fn, delete_fn);

  psplay_insert (&splay_tree, (PSplaynode)testitem_new ("foo"), 100);
  psplay_insert (&splay_tree, (PSplaynode)testitem_new ("bar"), 100);
  psplay_insert (&splay_tree, (PSplaynode)testitem_new ("baz"), 100);
  psplay_insert (&splay_tree, (PSplaynode)testitem_new ("test"), 100);
  psplay_insert (&splay_tree, (PSplaynode)testitem_new ("pap"), 100);
  psplay_insert (&splay_tree, (PSplaynode)testitem_new ("qux"), 100);

  testitem_graphvizdump (&splay_tree, stdout);
  psplay_dump (&splay_tree, stdout);

  //TestItem f = (TestItem) psplay_find (&splay_tree, "baz", 100);
  TestItem f = (TestItem) psplay_find (&splay_tree, "baz", 100);
  CHECK (f);
  printf ("found %p (%.4s)\n", &f->node, f->key);
  psplay_dump (&splay_tree, stdout);

  f = (TestItem) psplay_find (&splay_tree, "test", 100);
  CHECK (f);
  printf ("found %p (%.4s)\n", &f->node, f->key);
  psplay_dump (&splay_tree, stdout);

  f = (TestItem) psplay_find (&splay_tree, "test", 100);
  CHECK (f);
  printf ("found %p (%.4s)\n", &f->node, f->key);
  psplay_dump (&splay_tree, stdout);

  f = (TestItem) psplay_find (&splay_tree, "foo", 100);
  CHECK (f);
  printf ("found %p (%.4s)\n", &f->node, f->key);
  psplay_dump (&splay_tree, stdout);

#if 0
  psplay_delete (psplay_remove (&splay_tree, root.tree));
  psplay_dump (&splay_tree, stdout);

  psplay_delete (psplay_remove (&splay_tree, root.tree));
  psplay_dump (&splay_tree, stdout);

  psplay_delete (psplay_remove (&splay_tree, root.tree));
  psplay_dump (&splay_tree, stdout);
#endif
  printf ("destroying\n");
  psplay_destroy (&splay_tree);
  psplay_dump (&splay_tree, stdout);
#if 0
  psplay_delete (psplay_remove (&splay_tree, root.tree));
  psplay_dump (&splay_tree, stdout);

  psplay_delete (psplay_remove (&splay_tree, root.tree));
  psplay_dump (&splay_tree, stdout);

  psplay_delete (psplay_remove (&splay_tree, root.tree));
  psplay_dump (&splay_tree, stdout);
#endif
  return 0;
}

TEST ("basic_insert_splay")
{
  CHECK (argv[2]);

  psplay splay_tree;
  psplay_init (&splay_tree, cmp_fn, key_fn, delete_fn);

  int end = atoi (argv[2]);

  char key[1024];

  for (int i = 1; i <= end; ++i)
    {
      sprintf (key, "%d", i);
      ECHO ("insert %s", key);
      psplay_insert (&splay_tree, (PSplaynode)testitem_new (key), 100);
    }

  for (int i = end/2; i <= end; ++i)
    {
      psplay_dump (&splay_tree, stderr);
      sprintf (key, "%d", i);
      psplay_find (&splay_tree, key, 100);
    }
  psplay_destroy (&splay_tree);
  printf ("done\n");
}


TEST ("basic_rand_insert_dump")
{
  CHECK (argv[2]);

  psplay splay_tree;
  psplay_init (&splay_tree, cmp_fn, key_fn, delete_fn);

  int end = atoi (argv[2]);

  char key[1024];

  for (int i = 1; i <= end; ++i)
    {
      sprintf (key, "%d", i /*rand()*/);
      psplay_insert (&splay_tree, (PSplaynode)testitem_new (key), 100);
    }

  testitem_graphvizdump (&splay_tree, stdout);
  //testitem_dump (&splay_tree, stdout);

  psplay_destroy (&splay_tree);
  printf ("done\n");
}


TEST ("fast_insert")
{
  CHECK (argv[2]);

  psplay splay_tree;
  psplay_init (&splay_tree, fcmp_fn, fkey_fn, fdelete_fn);

  int end = atoi (argv[2]);

  char key[1024];

  for (int i = 1; i <= end; ++i)
    {
      sprintf (key, "%d", i);
      psplay_insert (&splay_tree, (PSplaynode)testitem_new (key), 100);
    }

  psplay_destroy (&splay_tree);
  printf ("done\n");
}




TEST ("nonexistant")
{
  CHECK (argv[2]);

}


TEST ("insert")
{
  CHECK (argv[2]);

}



TEST ("insert_rand")
{
  CHECK (argv[2]);

}



TEST ("insert_fastcheck")
{
  CHECK (argv[2]);

}



TESTS_END




/* === PSplay support functions === */

static int
cmp_fn (const void* a, const void* b)
{
  CHECK (a);
  CHECK (b);
  return strcmp (a, b);
}

static const void*
key_fn (const PSplaynode node)
{
  CHECK (node);
  CHECK (((TestItem)node)->key);

  return ((TestItem)node)->key;
}

static void
delete_fn (PSplaynode node)
{
  CHECK (node);
  testitem_delete ((TestItem) node);
}




static int
fcmp_fn (const void* a, const void* b)
{
  return strcmp (a, b);
}

static const void*
fkey_fn (const PSplaynode node)
{
  return ((TestItem)node)->key;
}

static void
fdelete_fn (PSplaynode node)
{
  testitem_delete ((TestItem) node);
}







/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
