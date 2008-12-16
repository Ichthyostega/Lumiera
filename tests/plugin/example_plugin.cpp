#include <iostream>

#include "hello_interface.h"

class example_plugin
{
public:
  static int myopen(void)
  {
    std::cout << "opened" << std::endl;
    return 0;
  }

  static int myclose(void)
  {
    std::cout << "closed" << std::endl;
    return 0;
  }
};

class example_plugin_de
  : public example_plugin
{
public:
  static void hello(void)
  {
    std::cout << "Hallo Welt!" << std::endl;
  }

  static void bye(const char* m)
  {
    std::cout << "Tschuess " << m << std::endl;
  }
};


class example_plugin_en
  : public example_plugin
{
public:
  static void hello(void)
  {
    std::cout << "Hello World!" << std::endl;
  }

  static void bye(const char* m)
  {
    std::cout << "Bye " << m << std::endl;
  }
};


LUMIERA_INTERFACE_IMPLEMENT(hello, 1, german, example_plugin_de::myopen, example_plugin_de::myclose,
                            example_plugin_de::hello,
                            example_plugin_de::bye
                           );

LUMIERA_INTERFACE_IMPLEMENT(hello, 1, english, example_plugin_en::myopen, example_plugin_en::myclose,
                            example_plugin_en::hello,
                            example_plugin_en::bye
                           );
