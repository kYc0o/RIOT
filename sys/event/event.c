#include <assert.h>

#include "event.h"
#include "clist.h"

void event_post(event_queue_t *queue, event_t *event)
{
    assert(!event->next);
    unsigned state = irq_disable();
    clist_rpush(&queue->event_list, &event->list_node);
    irq_restore(state);

    if (queue->waiter) {
        thread_flags_set(queue->waiter, THREAD_FLAG_EVENT);
    }
}

event_t *event_wait(event_queue_t *queue)
{
    thread_flags_wait_any(THREAD_FLAG_EVENT);
    unsigned state = irq_disable();
    event_t *result = (event_t *) clist_lpop(&queue->event_list);
    result->list_node.next = NULL;
    irq_restore(state);
    return result;
}

void event_source_attach(event_source_t *source, event_queue_t *queue, event_tap_t *tap)
{
    _event_tap_t *_tap = (_event_tap_t *) tap;
    unsigned state = irq_disable();
    clist_rpush(&source->tap_list, &tap->list_node);
    _tap->tap.queue = queue;
    irq_restore(state);
}

void event_source_trigger(event_source_t *source)
{
    _event_tap_t *_tap;

    unsigned state = irq_disable();
    while ((_tap = (_event_tap_t *)clist_lpop(&source->tap_list))) {
        event_post(_tap->tap.queue, &_tap->event);
    }
    irq_restore(state);
}
