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

#ifndef __ATM_WINDOWS_H__
#define __ATM_WINDOWS_H__

/** Win32 variant of atm_platform.h */
#include "platform.h"

typedef intptr_t atomic_t;
#define ATM_MAX INTPTR_MAX

#define atomic_full_barrier MemoryBarrier

static __inline atomic_t atomic_acq_load(const atomic_t *p) {
  atomic_t result = *p;
  atomic_full_barrier();
  return result;
}

static __inline atomic_t atomic_no_barrier_load(const atomic_t *p) {
  /* TODO(dklempner): Can we implement something better here? */
  return atomic_acq_load(p);
}

static __inline void atomic_rel_store(atomic_t *p, atomic_t value) {
  atomic_full_barrier();
  *p = value;
}

static __inline void atomic_no_barrier_store(atomic_t *p, atomic_t value) {
  /* TODO(ctiller): Can we implement something better here? */
  atomic_rel_store(p, value);
}

static __inline int atomic_no_barrier_cas(atomic_t *p, atomic_t o, atomic_t n) {
/** InterlockedCompareExchangePointerNoFence() not available on vista or
   windows7 */
#ifdef ARCH_64
  return o == (atomic_t)InterlockedCompareExchangeAcquire64(
                  (volatile LONGLONG *)p, (LONGLONG)n, (LONGLONG)o);
#else
  return o == (atomic_t)InterlockedCompareExchangeAcquire((volatile LONG *)p,
                                                         (LONG)n, (LONG)o);
#endif
}

static __inline int atomic_acq_cas(atomic_t *p, atomic_t o, atomic_t n) {
#ifdef ARCH_64
  return o == (atomic_t)InterlockedCompareExchangeAcquire64(
                  (volatile LONGLONG *)p, (LONGLONG)n, (LONGLONG)o);
#else
  return o == (atomic_t)InterlockedCompareExchangeAcquire((volatile LONG *)p,
                                                         (LONG)n, (LONG)o);
#endif
}

static __inline int atomic_rel_cas(atomic_t *p, atomic_t o, atomic_t n) {
#ifdef ARCH_64
  return o == (atomic_t)InterlockedCompareExchangeRelease64(
                  (volatile LONGLONG *)p, (LONGLONG)n, (LONGLONG)o);
#else
  return o == (atomic_t)InterlockedCompareExchangeRelease((volatile LONG *)p,
                                                         (LONG)n, (LONG)o);
#endif
}

static __inline int atomic_full_cas(atomic_t *p, atomic_t o, atomic_t n) {
#ifdef ARCH_64
  return o == (atomic_t)InterlockedCompareExchange64((volatile LONGLONG *)p,
                                                    (LONGLONG)n, (LONGLONG)o);
#else
  return o == (atomic_t)InterlockedCompareExchange((volatile LONG *)p, (LONG)n,
                                                  (LONG)o);
#endif
}

static __inline atomic_t atomic_no_barrier_fetch_add(atomic_t *p,
                                                     atomic_t delta) {
  /** Use the CAS operation to get pointer-sized fetch and add */
  atomic_t old;
  do {
    old = *p;
  } while (!atomic_no_barrier_cas(p, old, old + delta));
  return old;
}

static __inline atomic_t atomic_full_fetch_add(atomic_t *p, atomic_t delta) {
  /** Use a CAS operation to get pointer-sized fetch and add */
  atomic_t old;
#ifdef ARCH_64
  do {
    old = *p;
  } while (old != (atomic_t)InterlockedCompareExchange64((volatile LONGLONG *)p,
                                                        (LONGLONG)old + delta,
                                                        (LONGLONG)old));
#else
  do {
    old = *p;
  } while (old != (atomic_t)InterlockedCompareExchange(
                      (volatile LONG *)p, (LONG)old + delta, (LONG)old));
#endif
  return old;
}

static __inline atomic_t atomic_full_xchg(atomic_t *p, atomic_t n) {
  return (atomic_t)InterlockedExchangePointer((PVOID *)p, (PVOID)n);
}

#endif /* __ATM_WINDOWS_H__ */
