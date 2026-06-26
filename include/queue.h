#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    volatile uint8_t *data;
    uint8_t capacity;
    volatile uint8_t head;
    volatile uint8_t tail;
} Queue;

void queue_init(Queue *q, volatile uint8_t *data, uint8_t capacity);

bool queue_push(Queue *q, uint8_t byte);

bool queue_peek(const Queue *q, uint8_t *out);

bool queue_pop(Queue *q);

bool queue_empty(const Queue *q);

#endif /* QUEUE_H_ */
