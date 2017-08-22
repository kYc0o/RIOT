#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>

#include "irq.h"
#include "thread_flags.h"
#include "clist.h"

#define THREAD_FLAG_EVENT   0x1
#define EVENT_LOOP_EXIT     (0x0-1)

#define EVENT_QUEUE_INIT    { .waiter=(thread_t *)sched_active_thread }

typedef struct event event_t;
typedef void(*event_cb_t)(event_t*);

struct event {
    clist_node_t list_node;
    event_cb_t callback;
};

typedef struct {
    clist_node_t event_list;
    thread_t *waiter;
} event_queue_t;

void event_queue_init(event_queue_t *queue);
void event_post(event_queue_t *queue, event_t *event);
void event_cancel(event_queue_t *queue, event_t *event);
event_t *event_wait(event_queue_t *queue);
void event_loop(event_queue_t *queue);

#endif /* EVENT_H */
