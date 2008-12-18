#include "common/interface.h"

/*
  one interface which can greet in different languages
*/
LUMIERA_INTERFACE_DECLARE (lumieraorg_testhello, 0,
                           LUMIERA_INTERFACE_SLOT (void, hello, (void)),
                           LUMIERA_INTERFACE_SLOT (void, goodbye, (const char*)),
);

/*
  and now one which will be used to test if interfaces can open and call each other
*/
LUMIERA_INTERFACE_DECLARE (lumieraorg_testtest, 0,
                           LUMIERA_INTERFACE_SLOT (void, testit, (void)),
);
