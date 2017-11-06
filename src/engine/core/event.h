#ifndef __EVENT_H__
#define __EVENT_H__

#include <stdint.h>
#include "ats_type.h"
#include "event_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EVENT_LAST   1024
#define EVENT_CUSTOM 512

typedef uint16_t event_type;
typedef uint16_t event_flag;

#define EVENT_FLAG_TICK_MASK   (1 << 1)

#define EVENT_COMMON_FIELDS \
   atomic_t refcount; \
   event_type event_type; \
   event_flag event_flag; \
   datetime_t timestamp;


typedef struct event_s {
   EVENT_COMMON_FIELDS
}event_t;

typedef void(*event_destroy_func_t)(event_t *);
typedef event_t* (*event_clone_func_t)(event_t *);

int register_event(event_type type_id, event_flag default_flag,
                       const char *event_name, size_t event_size,
                      event_destroy_func_t event_destroy,
                       event_clone_func_t event_clone);

size_t event_size(event_type type_id);
event_t* event_zalloc(event_type type_id);
event_t* event_ref(event_t *);
void event_unref(event_t *);
event_t* event_clone(event_t *);

#define EVENT_NEW(x) (void *)event_zalloc((x))
#define EVENT_REF(x) event_ref((event_t*)(x))
#define EVENT_UNREF(x) event_unref((event_t*)(x))

#ifdef __cplusplus
}
#endif

#endif // __EVENT_H__
