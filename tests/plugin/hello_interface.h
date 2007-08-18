#include "lib/plugin.h"

CINELERRA_INTERFACE(hello, 1,
                    CINELERRA_INTERFACE_PROTO(void, hello, (void))
                    CINELERRA_INTERFACE_PROTO(void, goodbye, (const char*))
                    );
