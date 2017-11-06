/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __ATOMIC_H__
#define __ATOMIC_H__

/** This interface provides atomic operations and barriers.
   It is internal to atl support code and should not be used outside it.

   If an operation with acquire semantics precedes another memory access by the
   same thread, the operation will precede that other access as seen by other
   threads.

   If an operation with release semantics follows another memory access by the
   same thread, the operation will follow that other access as seen by other
   threads.

   Routines with "acq" or "full" in the name have acquire semantics.  Routines
   with "rel" or "full" in the name have release semantics.  Routines with
   "no_barrier" in the name have neither acquire not release semantics.

   The routines may be implemented as macros.

   // Atomic operations act on an intergral_type atomic_t that is guaranteed to
   // be the same size as a pointer.
   typedef intptr_t atomic_t;

   // A memory barrier, providing both acquire and release semantics, but not
   // otherwise acting on memory.
   void atomic_t_full_barrier(void);

   // Atomically return *p, with acquire semantics.
   atomic_t atomic_t_acq_load(atomic_t *p);
   atomic_t atomic_t_no_barrier_load(atomic_t *p);

   // Atomically set *p = value, with release semantics.
   void atomic_t_rel_store(atomic_t *p, atomic_t value);

   // Atomically add delta to *p, and return the old value of *p, with
   // the barriers specified.
   atomic_t atomic_t_no_barrier_fetch_add(atomic_t *p, atomic_t delta);
   atomic_t atomic_t_full_fetch_add(atomic_t *p, atomic_t delta);

   // Atomically, if *p==o, set *p=n and return non-zero otherwise return 0,
   // with the barriers specified if the operation succeeds.
   int atomic_t_no_barrier_cas(atomic_t *p, atomic_t o, atomic_t n);
   int atomic_t_acq_cas(atomic_t *p, atomic_t o, atomic_t n);
   int atomic_t_rel_cas(atomic_t *p, atomic_t o, atomic_t n);
   int atomic_t_full_cas(atomic_t *p, atomic_t o, atomic_t n);

   // Atomically, set *p=n and return the old value of *p
   atomic_t atomic_t_full_xchg(atomic_t *p, atomic_t n);
*/

#include "platform.h"

#if defined(GCC_ATOMIC)
#include "atm_gcc_atomic.h"
#elif defined(GCC_SYNC)
#include "atm_gcc_sync.h"
#elif defined(WINDOWS_ATOMIC)
#include "atm_windows.h"
#else
#error could not determine platform for atm
#endif

/** Adds \a delta to \a *value, clamping the result to the range specified
    by \a min and \a max.  Returns the new value. */
atomic_t atomic_t_no_barrier_clamped_add(atomic_t *value, atomic_t delta,
                                       atomic_t min, atomic_t max);

#endif /* __ATOMIC_H__ */
