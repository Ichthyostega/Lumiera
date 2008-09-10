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
  printf("Tschuess %s\n", m);
}

void hello(void)
{
  printf("Hello World!\n");
}

void bye(const char* m)
{
  printf("Bye %s\n", m);
}

LUMIERA_INTERFACE_IMPLEMENT(hello, 1, german, myopen, myclose,
                            hallo,
                            tschuess
                            );

LUMIERA_INTERFACE_IMPLEMENT(hello, 1, english, myopen, myclose,
                            hello,
                            bye
                            );
