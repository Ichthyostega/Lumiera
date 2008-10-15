#include <string.h>

#include "lib/plugin.h"
#include "hello_interface.h"

LUMIERA_ERROR_DEFINE(FAILURE, "test failure");

int
main(int argc, char** argv)
{
  NOBUG_INIT;

  if (argc < 2)
    return -1;

  lumiera_init_plugin ();
  /*
    we have a plugin 'hello_1' which provides us 2 hello interfaces, one for english and one for german output,
    open both try them, close them.
  */

  TODO("macros, doing casting and typing");

  if( !strcmp(argv[1],"C"))
    {
      LUMIERA_INTERFACE_TYPE(hello, 1)* hello_de =
        (LUMIERA_INTERFACE_TYPE(hello, 1)*) lumiera_interface_open ("example_plugin", "german_1", sizeof(LUMIERA_INTERFACE_TYPE(hello, 1)));

      if (!hello_de) LUMIERA_DIE (FAILURE);

      hello_de->hello();
      hello_de->goodbye(argv[1]);

      LUMIERA_INTERFACE_TYPE(hello, 1)* hello_en =
        (LUMIERA_INTERFACE_TYPE(hello, 1)*) lumiera_interface_open ("example_plugin", "english_1", sizeof(LUMIERA_INTERFACE_TYPE(hello, 1)));

      if (!hello_en) LUMIERA_DIE (FAILURE);

      hello_en->hello();
      hello_en->goodbye(argv[1]);

      lumiera_interface_close (hello_en);
      lumiera_interface_close (hello_de);
    }

  if( !strcmp(argv[1],"C++"))
    {
      /* same again for a plugin written in C++ */
      LUMIERA_INTERFACE_TYPE(hello, 1)* hello_de =
        (LUMIERA_INTERFACE_TYPE(hello, 1)*) lumiera_interface_open ("example_plugin_cpp", "german_1", sizeof(LUMIERA_INTERFACE_TYPE(hello, 1)));

      if (!hello_de) LUMIERA_DIE (FAILURE);

      hello_de->hello();
      hello_de->goodbye(argv[1]);

      LUMIERA_INTERFACE_TYPE(hello, 1)* hello_en =
        (LUMIERA_INTERFACE_TYPE(hello, 1)*) lumiera_interface_open ("example_plugin_cpp", "english_1", sizeof(LUMIERA_INTERFACE_TYPE(hello, 1)));

      if (!hello_en) LUMIERA_DIE (FAILURE);

      hello_en->hello();
      hello_en->goodbye(argv[1]);

      lumiera_interface_close (hello_en);
      lumiera_interface_close (hello_de);
    }

  return 0;
}
