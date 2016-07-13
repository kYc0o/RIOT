#include <assert.h>

#include "event.h"
#include "clist.h"

void event_post(event_sink_t *sink, event_t *event)
{
    assert(!event->next);
    unsigned state = irq_disable();
    clist_rpush((clist_node_t *)sink, (clist_node_t *)event);
    irq_restore(state);

    if (sink->waiter) {
        thread_flags_set(sink->waiter, THREAD_FLAG_EVENT);
    }
}

event_t *event_wait(event_sink_t *sink)
{
    thread_flags_wait_any(THREAD_FLAG_EVENT);
    unsigned state = irq_disable();
    event_t *result = (event_t *) clist_lpop((clist_node_t *)sink);
    result->next = NULL;
    irq_restore(state);
    return result;
}

void event_source_attach(event_source_t *source, event_sink_t *sink, event_tap_t *tap)
{
    _event_tap_t *_tap = (_event_tap_t *) tap;
    unsigned state = irq_disable();
    clist_rpush((clist_node_t *)source, (clist_node_t *)tap);
    _tap->tap.sink = sink;
    irq_restore(state);
}

void event_source_trigger(event_source_t *source)
{
    _event_tap_t *_tap;

    unsigned state = irq_disable();
    while ((_tap = (_event_tap_t *)clist_lpop((clist_node_t *)source))) {
        event_post(_tap->tap.sink, &_tap->event);
    }
    irq_restore(state);
}
