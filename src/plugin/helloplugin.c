/*
 * hello.c - demonstrates how to build a standalone tool (C source) 
 *           integrated into the SCons based build system of Lumiera
 */

#include <stdio.h>

int main(int argc, char* argv[])
  {
    printf("hello lumiera world");
    return 0;
  }
