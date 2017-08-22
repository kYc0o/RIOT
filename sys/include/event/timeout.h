#ifndef TIMEOUT_H
#define TIMEOUT_H

#include "event.h"
#include "xtimer.h"

typedef struct {
    xtimer_t timer;
    event_queue_t *queue;
    event_t *event;
} event_timeout_t;

void event_timeout_init(event_timeout_t *event_timeout, event_queue_t *queue, event_t *event);
void event_timeout_set(event_timeout_t *event_timeout, uint32_t timeout);

#endif /* TIMEOUT_H */
