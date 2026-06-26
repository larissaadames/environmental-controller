#include "queue.h"

void queue_init(Queue *q, volatile uint8_t *data, uint8_t capacity)
{
    q->data     = data;
    q->capacity = capacity;
    q->head     = 0;
    q->tail     = 0;
}

bool queue_push(Queue *q, uint8_t byte)
{
    uint8_t next = (uint8_t)((q->head + 1) % q->capacity);
    if (next == q->tail)
    {
        return false;
    }

    q->data[q->head] = byte;
    q->head = next;

    return true;
}

bool queue_peek(const Queue *q, uint8_t *out)
{
    if (q->head == q->tail)
    {
        return false;
    }

    *out = q->data[q->tail];

    return true;
}

bool queue_pop(Queue *q)
{
    if (q->head == q->tail)
    {
        return false;
    }

    q->tail = (uint8_t)((q->tail + 1) % q->capacity);

    return true;
}

bool queue_empty(const Queue *q)
{
    return q->head == q->tail;
}
