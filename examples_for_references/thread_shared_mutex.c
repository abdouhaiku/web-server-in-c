#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int work_available;
} shared_t;

shared_t shared = {
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .cond = PTHREAD_COND_INITIALIZER,
    .work_available = 0
};

void *worker(void *arg)
{
    (void)arg;

    pthread_mutex_lock(&shared.mutex);

    while (!shared.work_available) {
        printf("Worker: waiting for work...\n");
        pthread_cond_wait(&shared.cond, &shared.mutex);
    }

    printf("Worker: got work!\n");

    shared.work_available = 0;

    pthread_mutex_unlock(&shared.mutex);

    return NULL;
}

int main(void)
{
    pthread_t tid;

    pthread_create(&tid, NULL, worker, NULL);

    sleep(2);

    pthread_mutex_lock(&shared.mutex);
    shared.work_available = 1;
    pthread_cond_signal(&shared.cond);
    pthread_mutex_unlock(&shared.mutex);

    pthread_join(tid, NULL);

    return 0;
}