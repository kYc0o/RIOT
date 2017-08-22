#ifndef CALLBACK_H
#define CALLBACK_H

#include "event.h"

typedef struct {
    event_t super;
    void (*callback)(void*);
    void *arg;
} event_callback_t;

void event_callback_init(event_callback_t *event_callback, void (callback)(void*),  void *arg);
void _event_callback(event_t *event);

#define EVENT_CALLBACK_INIT(_cb, _arg)  { .super.callback=_event_callback, .callback=_cb, .arg=(void*)_arg }

#endif /* CALLBACK_H */
