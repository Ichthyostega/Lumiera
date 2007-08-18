#include <stdio.h>

#include "hello_interface.h"

int myopen(void)
{
  printf("opened\n");
  return 0;
}

int myclose(void)
{
  printf("closed\n");
  return 0;
}

void hallo(void)
{
  printf("Hallo Welt!\n");
}

void tschuess(const char* m)
{
  printf("Tschues %s\n", m);
}

void hello(void)
{
  printf("Hello world!\n");
}

void bye(const char* m)
{
  printf("bye %s\n", m);
}

CINELERRA_INTERFACE_IMPLEMENT(hello, 1, german, myopen, myclose,
                              hallo,
                              tschuess
                              );

CINELERRA_INTERFACE_IMPLEMENT(hello, 1, english, myopen, myclose,
                              hello,
                              bye
                              );
