/*
 * hello.c - demonstrates how to build a standalone tool (C source) 
 *           integrated into the SCons based build system of Cinelerra
 */

#include <stdio.h>

int main(int argc, char* argv[])
  {
    printf("hello cinelerra world");
    return 0;
  }
