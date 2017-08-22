#include "event/callback.h"

void _event_callback(event_t *event)
{
    event_callback_t *event_callback = (event_callback_t *) event;
    event_callback->callback(event_callback->arg);
}

void event_callback_init(event_callback_t *event_callback, void (callback)(void*),  void *arg)
{
    event_callback->super.callback = _event_callback;
    event_callback->callback = callback;
    event_callback->arg = arg;
}
