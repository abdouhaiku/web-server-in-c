#include "queue.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void queue_init(conn_queue_t *q, int capacity) {
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_full, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    q->items = malloc(capacity * sizeof(client_ctx_t));
    if (q->items == NULL) {
        fprintf(stderr, "queue init allocation failed\n");
        exit(1);
    }
    q->capacity = capacity;
    q->head = q->tail = q->count = 0;

}

// blocks if full
void queue_push(conn_queue_t *q, client_ctx_t item) {
    // First try to get the lock
    //This would suspend until the lock is available

    pthread_t tid = pthread_self();
    pthread_mutex_lock(&q->mutex);

    while (q->count == q->capacity) {
        printf("Worker %lu: queue is full, waiting for work... \n", (unsigned long) tid);
        pthread_cond_wait(&q->not_full, &q->mutex);
    }

    printf("Producer %lu: will push the client to the queue!\n", (unsigned long) tid);
    q->items[q->head] = item;
    q->head = (q->head + 1) % q->capacity;
    q->count++;

    pthread_cond_signal(&q->not_empty);   
    pthread_mutex_unlock(&q->mutex);
}

client_ctx_t queue_pop(conn_queue_t *q) {
    // Get the lock
    pthread_t tid = pthread_self();
    pthread_mutex_lock(&q->mutex);

    while (q->count == 0) {
        printf("Worker %lu: queue is empty, waiting for work... \n", (unsigned long) tid);
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }

    printf("Worker %lu: Will pop the connection.. \n", (unsigned long) tid);
    client_ctx_t ctx = q->items[q->tail];
    q->tail = (q->tail +1) % q->capacity;
    q->count--;

    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
    return ctx;

}

void queue_destroy(conn_queue_t* q) {
    free(q->items);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->not_full);
    pthread_cond_destroy(&q->not_empty);
}