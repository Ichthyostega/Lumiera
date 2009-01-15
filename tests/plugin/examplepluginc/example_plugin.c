/*
  example_plugin  -  dummy example plugin
 
* *****************************************************/


#include <stdio.h>


void hallo (void)
{
  printf ("Hallo Welt!\n");
}

void tschuess (const char* m)
{
  printf ("Tschuess %s\n", m);
}

void hello (void)
{
  printf ("Hello World!\n");
}

void bye (const char* m)
{
  printf ("Bye %s\n", m);
}

