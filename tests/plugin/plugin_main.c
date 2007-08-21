#include <string.h>

#include "lib/plugin.h"
#include "hello_interface.h"


int
main(int argc, char** argv)
{
  NOBUG_INIT;

  if (argc < 2)
    return -1;

  cinelerra_init_plugin ();
  /*
    we have a plugin 'hello_1' which provides us 2 hello interfaces, one for english and one for german output,
    open both try them, close them.
  */

  TODO("macros, doing casting and typing");

  if( !strcmp(argv[1],"C"))
    {
      CINELERRA_INTERFACE_TYPE(hello, 1)* hello_de =
        (CINELERRA_INTERFACE_TYPE(hello, 1)*) cinelerra_interface_open ("example_plugin", "german_1", sizeof(CINELERRA_INTERFACE_TYPE(hello, 1)));

      if (!hello_de) CINELERRA_DIE;

      hello_de->hello();
      hello_de->goodbye(argv[1]);

      CINELERRA_INTERFACE_TYPE(hello, 1)* hello_en =
        (CINELERRA_INTERFACE_TYPE(hello, 1)*) cinelerra_interface_open ("example_plugin", "english_1", sizeof(CINELERRA_INTERFACE_TYPE(hello, 1)));

      if (!hello_en) CINELERRA_DIE;

      hello_en->hello();
      hello_en->goodbye(argv[1]);

      cinelerra_interface_close (hello_en);
      cinelerra_interface_close (hello_de);
    }

  if( !strcmp(argv[1],"C++"))
    {
      /* same again for a plugin written in C++ */
      CINELERRA_INTERFACE_TYPE(hello, 1)* hello_de =
        (CINELERRA_INTERFACE_TYPE(hello, 1)*) cinelerra_interface_open ("example_plugin_cpp", "german_1", sizeof(CINELERRA_INTERFACE_TYPE(hello, 1)));

      if (!hello_de) CINELERRA_DIE;

      hello_de->hello();
      hello_de->goodbye(argv[1]);

      CINELERRA_INTERFACE_TYPE(hello, 1)* hello_en =
        (CINELERRA_INTERFACE_TYPE(hello, 1)*) cinelerra_interface_open ("example_plugin_cpp", "english_1", sizeof(CINELERRA_INTERFACE_TYPE(hello, 1)));

      if (!hello_en) CINELERRA_DIE;

      hello_en->hello();
      hello_en->goodbye(argv[1]);

      cinelerra_interface_close (hello_en);
      cinelerra_interface_close (hello_de);
    }

  return 0;
}
