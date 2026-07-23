//
// Created by Abdou on 21/07/2026.
//

#ifndef WEB_SERVER_IN_C_QUEUE_H
#define WEB_SERVER_IN_C_QUEUE_H
#include <pthread.h>
#include "utilities.h"

typedef struct {
    client_ctx_t *items;   // fixed array, size = capacity
    int capacity;
    int head, tail, count;
    //[[],[],[]...]
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} conn_queue_t;


void queue_init(conn_queue_t *q, int capacity);
void queue_push(conn_queue_t *q, client_ctx_t item);  // blocks if full
client_ctx_t queue_pop(conn_queue_t *q);               // blocks if empty
#endif //WEB_SERVER_IN_C_QUEUE_H
