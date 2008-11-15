#include "backend/interface.h"

LUMIERA_INTERFACE_DECLARE (lumieraorg_testhello, 0,
                           LUMIERA_INTERFACE_SLOT (void, hello, (void)),
                           LUMIERA_INTERFACE_SLOT (void, goodbye, (const char*)),
);
