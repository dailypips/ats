/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/


#include "event.h"
#include "atomic.h"
#include <stdint.h>
#include <stddef.h>


typedef struct event_info_s {
    size_t event_size;
	uint16_t default_flag;
	char *name;
    event_destroy_func_t destroy_func;
    event_clone_func_t clone_func;
}event_info_t;

static event_info_t event_vtable[EVENT_LAST];

/* check event is defined */
static int check_event_type(event_type type)
{
    if (type >= EVENT_LAST)
        return 1;
    else
        return 0;
}

/* public api */
int register_event(event_type type_id, event_flag
                       default_flag, const char *event_name, size_t event_size,
                       event_destroy_func_t event_destroy, event_clone_func_t event_clone)
{
    if (event_vtable[type_id].event_size != 0)
        return -1;

    event_vtable[type_id].event_size = event_size;
    event_vtable[type_id].default_flag = default_flag;
    event_vtable[type_id].name = strdup(event_name);
    event_vtable[type_id].destroy_func = event_destroy;
    event_vtable[type_id].clone_func = event_clone;

    return 0;
}

size_t event_size(event_type type)
{
    ASSERT(check_event_type(type));
    return event_vtable[type].event_size;
}

static void event_free(event_t *e)
{
    event_destroy_func_t destroy = event_vtable[e->event_type].destroy_func;
    if (destroy)
        destroy(e);
    ats_free(e);
}

static event_t *default_clone(event_t *e)
{
    size_t size = event_vtable[e->event_type].event_size;
    event_t *result = ats_zalloc(size);
    memcpy(result, e, size);
    atomic_rel_store(&e->refcount, 1);
    return result;
}

event_t *event_clone(event_t *e)
{
    ASSERT(check_event_type(e->event_type));
    event_t *result;
    event_clone_func_t clone = event_vtable[e->event_type].clone_func;
    if(clone)
        result = clone(e);
    else
        result = default_clone(e);

    return result;
}

event_t *event_ref(event_t *event)
{
  ASSERT(event != NULL);
  atomic_full_fetch_add(&event->refcount, 1);
  return event;
}

void event_unref(event_t *event)
{
   ASSERT(event != NULL);
   if(atomic_full_fetch_add(&event->refcount, -1) == 1)
       event_free(event);
}

event_t* event_zalloc(event_type type)
{
    ASSERT(check_event_type(type) == 0);
    event_t* e = ats_zalloc(event_vtable[type].event_size);
    if (e) {
        e->event_type = type;
        e->event_flag = event_vtable[type].default_flag;
        atomic_rel_store(&e->refcount, 1);
    }
    return e;
}



