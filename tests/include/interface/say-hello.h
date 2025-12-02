/** @file say-hello.h
 ** A mock interface to support testing of the interface system
 */

#ifndef TESTS_INCLUDE_SAY_HELLO_H
#define TESTS_INCLUDE_SAY_HELLO_H

#include "common/interface.h"

/** mock interface which can greet in different languages */

LUMIERA_INTERFACE_DECLARE (lumieraorg_testhello, 0,
                           LUMIERA_INTERFACE_SLOT (void, hello, (void)),
                           LUMIERA_INTERFACE_SLOT (void, goodbye, (const char*)),
);

/** mock interface to test opening and closing of dependent interfaces */
LUMIERA_INTERFACE_DECLARE (lumieraorg_testtest, 0,
                           LUMIERA_INTERFACE_SLOT (void, testit, (void)),
);

#endif /*TESTS_INCLUDE_SAY_HELLO_H*/
