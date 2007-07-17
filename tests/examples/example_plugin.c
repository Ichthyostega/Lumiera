#include <stdio.h>

#include "hello_interface.h"

int myopen(void)
{
  printf("opened\n");
}

int myclose(void);
{
  printf("closed\n");
}

int hallo(void)
{
  printf("Hallo Welt!\n");
}

int tschuess(const char* m)
{
  printf("Tschues %s\n", m);
}

int hello(void);
{
  printf("Hello world!\n");
}

int bye(const char* m)
{
  printf("bye %s\n", m);

}

CINELERRA_INTERFACE_IMPLEMENT(hello, 1, german, myopen, myclose,
                              CINELERRA_INTERFACE_FUNC(hello, hallo),
                              CINELERRA_INTERFACE_FUNC(goodbye, tschuess)
                              );

CINELERRA_INTERFACE_IMPLEMENT(hello, 1, english, myopen, myclose,
                              CINELERRA_INTERFACE_FUNC(hello, hello),
                              CINELERRA_INTERFACE_FUNC(goodbye, bye)
                              );
