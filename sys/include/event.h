#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>

#include "irq.h"
#include "thread_flags.h"
#include "clist.h"

#define THREAD_FLAG_EVENT   0x1
#define EVENT_LOOP_EXIT     (0x0-1)

typedef struct event event_t;
typedef struct event_source event_source_t;
typedef struct event_tap event_tap_t;
typedef void(*event_cb_t)(event_t*);

struct event {
    clist_node_t list_node;
    event_cb_t callback;
};

typedef struct {
    clist_node_t event_list;
    thread_t *waiter;
} event_queue_t;

struct event_tap {
    clist_node_t list_node;
    event_tap_t *next;
    event_queue_t *queue;
};

struct event_source {
    clist_node_t tap_list;
    event_tap_t *list;
};

typedef struct {
    event_tap_t tap;
    event_t event;
} _event_tap_t;

void event_post(event_queue_t *queue, event_t *event);
event_t *event_wait(event_queue_t *queue);

void event_source_attach(event_source_t *source, event_queue_t *queue, event_tap_t *tap);
void event_source_trigger(event_source_t *source);

#endif /* EVENT_H */
