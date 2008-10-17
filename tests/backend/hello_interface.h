#include "lib/plugin.h"

LUMIERA_INTERFACE(hello, 1,
                  LUMIERA_INTERFACE_PROTO(void, hello, (void))
                  LUMIERA_INTERFACE_PROTO(void, goodbye, (const char*))
                  );
