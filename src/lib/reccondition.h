/*
  reccondition.h  -  condition variables, w/ recursive mutex

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef LUMIERA_RECCONDITION_H
#define LUMIERA_RECCONDITION_H

#include "lib/error.h"
#include "lib/mutex.h"


/**
 * @file
 * Reccondition variables. Same as Conditon variables but using a recursive mutex for locking.
 */


/**
 * Reccondition section.
 * Locks the reccondition mutex, one can then use LUMIERA_RECCONDITION_WAIT to wait for signals or
 * LUMIERA_RECCONDITION_SIGNAL or LUMIERA_RECCONDITION_BROADCAST to wake waiting threads
 * @param nobugflag NoBug flag used to log actions on the reccondition
 * @param rcnd Reccondition variable to be locked
 */
#define LUMIERA_RECCONDITION_SECTION(nobugflag, rcnd)                                                           \
  for (lumiera_recconditionacquirer NOBUG_CLEANUP(lumiera_recconditionacquirer_ensureunlocked)                  \
         lumiera_reccondition_section_ = {(LumieraReccondition)1};                                              \
       lumiera_reccondition_section_.reccondition;)                                                             \
    for (                                                                                                       \
         ({                                                                                                     \
           lumiera_reccondition_section_.reccondition = (rcnd);                                                 \
           NOBUG_IF(NOBUG_MODE_ALPHA, lumiera_reccondition_section_.flag = &NOBUG_FLAG(nobugflag));             \
           NOBUG_RESOURCE_HANDLE_INIT (lumiera_reccondition_section_.rh);                                       \
           RESOURCE_ENTER (nobugflag, (rcnd)->rh, "acquire reccondition", &lumiera_reccondition_section_,       \
                           NOBUG_RESOURCE_RECURSIVE, lumiera_reccondition_section_.rh);                         \
           if (pthread_mutex_lock (&(rcnd)->mutex)) LUMIERA_DIE (MUTEX_LOCK);                                   \
         });                                                                                                    \
         lumiera_reccondition_section_.reccondition;                                                            \
         ({                                                                                                     \
           LUMIERA_RECCONDITION_UNLOCK(nobugflag)                                                               \
         }))


/**
 * Explicit mutex unlock for a reccondition variable
 * One can early unlock the mutex of a condition variable prior leaving a RECCONDITION_SECTION.
 * The RECCONDITION_WAIT, RECCONDITION_SIGNAL and RECCONDITION_BROADCAST macros must not be used after the mutex
 * got unlocked.
 * @param nobugflag NoBug flag used to log actions on the reccondition
 */
#define LUMIERA_RECCONDITION_UNLOCK                                                     \
  if (lumiera_reccondition_section_.reccondition)                                       \
    {                                                                                   \
      pthread_mutex_unlock (&lumiera_reccondition_section_.reccondition->mutex);        \
      lumiera_reccondition_section_.reccondition = NULL;                                \
      RESOURCE_LEAVE(nobugflag, lumiera_reccondition_section_.rh);                      \
    }


/**
 * Wait for a reccondition.
 * Must be used inside a RECCONDITION_SECTION.
 * @param expr Recconditon which must become true, else the reccondition variable goes back into sleep
 */
#define LUMIERA_RECCONDITION_WAIT(expr)                                                                                         \
  do {                                                                                                                          \
    ENSURE (lumiera_reccondition_section_.reccondition, "Reccondition mutex not locked");                                       \
    NOBUG_RESOURCE_STATE_RAW (lumiera_reccondition_section_.flag, lumiera_reccondition_section_.rh, NOBUG_RESOURCE_WAITING);    \
    pthread_cond_wait (&lumiera_reccondition_section_.reccondition->cond, &lumiera_reccondition_section_.reccondition->mutex);  \
    NOBUG_RESOURCE_STATE_RAW (lumiera_reccondition_section_.flag, lumiera_reccondition_section_.rh, NOBUG_RESOURCE_RECURSIVE);  \
  } while (!(expr))


/**
 * Signal a reccondition variable
 * Must be used inside a RECCONDITION_SECTION.
 * Wakes one thread waiting on the condition variable
 */
#define LUMIERA_RECCONDITION_SIGNAL                                                     \
do {                                                                                    \
  ENSURE (lumiera_reccondition_section_.reccondition, "Reccondition mutex not locked"); \
  NOBUG_IF(NOBUG_MODE_ALPHA, TRACE(lumiera_reccondition_section_.flag, "Signaling"));   \
  pthread_cond_signal (&lumiera_reccondition_section_.reccondition->cond);              \
} while (0)


/**
 * Broadcast a reccondition variable
 * Must be used inside a RECCONDITION_SECTION.
 * Wakes all threads waiting on the reccondition variable
 */
#define LUMIERA_RECCONDITION_BROADCAST                                                  \
do {                                                                                    \
  ENSURE (lumiera_reccondition_section_.reccondition, "Reccondition mutex not locked"); \
  NOBUG_IF(NOBUG_MODE_ALPHA, TRACE(lumiera_reccondition_section_.flag, "Broadcasting"));\
  pthread_cond_broadcast (&lumiera_reccondition_section_.reccondition->cond);           \
} while (0)



/**
 * Reccondition variables.
 *
 */
struct lumiera_reccondition_struct
{
  pthread_cond_t cond;
  pthread_mutex_t mutex;
  RESOURCE_HANDLE (rh);
};
typedef struct lumiera_reccondition_struct lumiera_reccondition;
typedef lumiera_reccondition* LumieraReccondition;


/**
 * Initialize a reccondition variable
 * @param self is a pointer to the reccondition variable to be initialized
 * @return self as given
 */
LumieraReccondition
lumiera_reccondition_init (LumieraReccondition self, const char* purpose, struct nobug_flag* flag);


/**
 * Destroy a reccondition variable
 * @param self is a pointer to the reccondition variable to be destroyed
 * @return self as given
 */
LumieraReccondition
lumiera_reccondition_destroy (LumieraReccondition self, struct nobug_flag* flag);


/**
 * recconditionacquirer used to manage the state of a reccondition variable.
 */
struct lumiera_recconditionacquirer_struct
{
  LumieraReccondition reccondition;
  NOBUG_IF(NOBUG_MODE_ALPHA, struct nobug_flag* flag);
  RESOURCE_HANDLE (rh);
};
typedef struct lumiera_recconditionacquirer_struct lumiera_recconditionacquirer;
typedef struct lumiera_recconditionacquirer_struct* LumieraRecconditionacquirer;


/* helper function for nobug */
static inline void
lumiera_recconditionacquirer_ensureunlocked (LumieraRecconditionacquirer self)
{
  ENSURE (!self->reccondition, "forgot to unlock reccondition variable");
}


#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
