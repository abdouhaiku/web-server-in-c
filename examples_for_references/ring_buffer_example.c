#define RING_SIZE 16
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/_types/_size_t.h>

typedef struct {
    int buf[RING_SIZE];
    size_t head;
    size_t tail;
    size_t count;
} simple_ring_t;

void simple_init(simple_ring_t *r) {
    r->head = r->tail = r->count = 0;
}

bool simple_put(simple_ring_t *r, uint8_t byte) {
    if (r->count == RING_SIZE) return false;
    r->buf[r->head] = byte;
    r->head = (r->head + 1) % RING_SIZE;
    r->count++;
    return true;
}

bool simple_get(simple_ring_t *r, int *byte) {
    if (r->count == 0) return false;
    *byte = r->buf[r->tail];
    r->tail = (r->tail + 1) % RING_SIZE;
    r->count--;
    return true;
}

int main(void) {
    simple_ring_t ring;
    simple_init(&ring);
    for (int i = 0; i < RING_SIZE; i++) {
        simple_put(&ring,  i);
    }

    for (size_t i = 0; i < RING_SIZE; i++) {
        int byte = 0;

        if (simple_get(&ring, &byte)) {
            printf("Value obtained from simple_get is: %d\n",  byte);
        } else {
            fprintf(stderr, "Failed to get value from the ring\n");
            return 1;
        }
    }
}
