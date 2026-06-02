#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>

typedef struct {
    volatile uint8_t *data;
    uint8_t           capacity;
    volatile uint8_t  head;
    volatile uint8_t  tail;
} Queue;

void queue_init(Queue *q, volatile uint8_t *data, uint8_t capacity);

// Returns 1 on success, 0 if full (byte is dropped).
int queue_push(Queue *q, uint8_t byte);

// Reads the front byte into *out without removing it. Returns 1 on success, 0 if empty.
int queue_peek(const Queue *q, uint8_t *out);

// Removes the front byte. Returns 1 on success, 0 if empty.
int queue_pop(Queue *q);

int queue_empty(const Queue *q);

#endif /* QUEUE_H_ */
