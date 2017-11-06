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

#ifndef __ATM_GCC_SYNC_H__
#define __ATM_GCC_SYNC_H__

/* variant of atm_platform.h for gcc and gcc-like compiers with __sync_*
   interface */
#include "platform.h"

typedef intptr_t atomic_t;
#define ATM_MAX INTPTR_MAX

#define ATM_COMPILE_BARRIER_() __asm__ __volatile__("" : : : "memory")

#if defined(__i386) || defined(__x86_64__)
/* All loads are acquire loads and all stores are release stores.  */
#define ATM_LS_BARRIER_() ATM_COMPILE_BARRIER_()
#else
#define ATM_LS_BARRIER_() atomic_full_barrier()
#endif

#define atomic_full_barrier() (__sync_synchronize())

static __inline atomic_t atomic_acq_load(const atomic_t *p) {
  atomic_t value = *p;
  ATM_LS_BARRIER_();
  return value;
}

static __inline atomic_t atomic_no_barrier_load(const atomic_t *p) {
  atomic_t value = *p;
  ATM_COMPILE_BARRIER_();
  return value;
}

static __inline void atomic_rel_store(atomic_t *p, atomic_t value) {
  ATM_LS_BARRIER_();
  *p = value;
}

static __inline void atomic_no_barrier_store(atomic_t *p, atomic_t value) {
  ATM_COMPILE_BARRIER_();
  *p = value;
}

#undef ATM_LS_BARRIER_
#undef ATM_COMPILE_BARRIER_

#define atomic_no_barrier_fetch_add(p, delta) \
  atomic_full_fetch_add((p), (delta))
#define atomic_full_fetch_add(p, delta) (__sync_fetch_and_add((p), (delta)))

#define atomic_no_barrier_cas(p, o, n) atomic_acq_cas((p), (o), (n))
#define atomic_acq_cas(p, o, n) (__sync_bool_compare_and_swap((p), (o), (n)))
#define atomic_rel_cas(p, o, n) atomic_acq_cas((p), (o), (n))
#define atomic_full_cas(p, o, n) atomic_acq_cas((p), (o), (n))

static __inline atomic_t atomic_full_xchg(atomic_t *p, atomic_t n) {
  atomic_t cur;
  do {
    cur = atomic_acq_load(p);
  } while (!atomic_rel_cas(p, cur, n));
  return cur;
}

#endif /* __ATM_GCC_SYNC_H__ */
