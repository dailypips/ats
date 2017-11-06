/*
 *
 * Copyright 2016 gRPC authors.
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

#ifndef __MPSCQ_H__
#define __MPSCQ_H__

#include <stdbool.h>
#include "common.h"
#include "atomic.h"

// Multiple-producer single-consumer lock free queue, based upon the
// implementation from Dmitry Vyukov here:
// http://www.1024cores.net/home/lock-free-algorithms/queues/intrusive-mpsc-node-based-queue

// List node (include this in a data structure at the top, and add application
// fields after it - to simulate inheritance)
typedef struct mpscq_node { atomic_t next; } mpscq_node;

// Actual queue type
typedef struct mpscq_s {
  char pad1[CACHELINE_SIZE];
  atomic_t head;
  // make sure head & tail don't share a cacheline
  ALIGN(CACHELINE_SIZE) mpscq_node *tail;
  ALIGN(CACHELINE_SIZE)mpscq_node sentinel;
  char pad2[CACHELINE_SIZE - sizeof(mpscq_node)];
} mpscq_t;

FUNC void mpscq_init(mpscq_t *q);
FUNC void mpscq_destroy(mpscq_t *q);
// Push a node
FUNC void mpscq_push(mpscq_t *q, mpscq_node *n);
// Pop a node (returns NULL if no node is ready - which doesn't indicate that
// the queue is empty!!)
FUNC mpscq_node *mpscq_pop(mpscq_t *q);
// Pop a node; sets *empty to true if the queue is empty, or false if it is not
FUNC mpscq_node *mpscq_pop_and_check_end(mpscq_t *q, bool *empty);

#endif /* __MPSCQ_H__ */
