/*
 * hello.c - demonstrates how to build a standalone tool (C source) 
 *           integrated into the SCons based build system of Lumiera
 */


/** @file hello.c
 ** Example file to demonstrate integration of stand-alone tools into the build process
 */

#include <stdio.h>

int
main (int argc, char* argv[])
  {
    (void)argc;
    (void)argv;
    printf("hello lumiera world\n");
    return 0;
  }
