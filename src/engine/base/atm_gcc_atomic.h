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

#ifndef __ATM_GCC_ATOMIC_H__
#define __ATM_GCC_ATOMIC_H__

/* atm_platform.h for gcc and gcc-like compilers with the
   __atomic_* interface.  */
#include "platform.h"

typedef intptr_t atomic_t;
#define ATM_MAX INTPTR_MAX

#ifdef LOW_LEVEL_COUNTERS
extern atomic_t counter_atm_cas;
extern atomic_t counter_atm_add;
#define ATM_INC_COUNTER(counter) \
  __atomic_fetch_add(&counter, 1, __ATOMIC_RELAXED)
#define ATM_INC_CAS_THEN(blah) \
  (ATM_INC_COUNTER(counter_atm_cas), blah)
#define ATM_INC_ADD_THEN(blah) \
  (ATM_INC_COUNTER(counter_atm_add), blah)
#else
#define ATM_INC_CAS_THEN(blah) blah
#define ATM_INC_ADD_THEN(blah) blah
#endif

#define atomic_full_barrier() (__atomic_thread_fence(__ATOMIC_SEQ_CST))

#define atomic_acq_load(p) (__atomic_load_n((p), __ATOMIC_ACQUIRE))
#define atomic_no_barrier_load(p) (__atomic_load_n((p), __ATOMIC_RELAXED))
#define atomic_rel_store(p, value) \
  (__atomic_store_n((p), (intptr_t)(value), __ATOMIC_RELEASE))
#define atomic_no_barrier_store(p, value) \
  (__atomic_store_n((p), (intptr_t)(value), __ATOMIC_RELAXED))

#define atomic_no_barrier_fetch_add(p, delta) \
  ATM_INC_ADD_THEN(                        \
      __atomic_fetch_add((p), (intptr_t)(delta), __ATOMIC_RELAXED))
#define atomic_full_fetch_add(p, delta) \
  ATM_INC_ADD_THEN(                  \
      __atomic_fetch_add((p), (intptr_t)(delta), __ATOMIC_ACQ_REL))

static __inline int atomic_no_barrier_cas(atomic_t *p, atomic_t o, atomic_t n) {
  return ATM_INC_CAS_THEN(__atomic_compare_exchange_n(
      p, &o, n, 0, __ATOMIC_RELAXED, __ATOMIC_RELAXED));
}

static __inline int atomic_acq_cas(atomic_t *p, atomic_t o, atomic_t n) {
  return ATM_INC_CAS_THEN(__atomic_compare_exchange_n(
      p, &o, n, 0, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED));
}

static __inline int atomic_rel_cas(atomic_t *p, atomic_t o, atomic_t n) {
  return ATM_INC_CAS_THEN(__atomic_compare_exchange_n(
      p, &o, n, 0, __ATOMIC_RELEASE, __ATOMIC_RELAXED));
}

static __inline int atomic_full_cas(atomic_t *p, atomic_t o, atomic_t n) {
  return ATM_INC_CAS_THEN(__atomic_compare_exchange_n(
      p, &o, n, 0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED));
}

#define atomic_full_xchg(p, n) \
  ATM_INC_CAS_THEN(__atomic_exchange_n((p), (n), __ATOMIC_ACQ_REL))

#endif /* __ATM_GCC_ATOMIC_H__ */
