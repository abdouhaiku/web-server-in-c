//
// Created by Abdou on 11/07/2026.
//

#include <stdio.h>  // console input/output, perror
#include <stdlib.h> // exit
#include <string.h> // string manipulation
#include <netdb.h>  // getnameinfo

#include <sys/socket.h> // socket APIs
#include <netinet/in.h> // sockaddr_in
#include <unistd.h>     // open, close

#include <signal.h> // signal handling
#include <sys/errno.h>
#include <pthread.h>
#include "application_routes.h"


#include "router.h"
#include "server_config.h"
#include "utilities.h"

#define PORT 8081
#define BACKLOG 10


int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);

    server_t server;
    //TODO : make it configurable
    char path[] = {"config.ini"};
    if (server_init(path, &server) != 0) {
        printf("error in starting up the server");
        return 1;
    }

    router_t router = {0};
    router_add(&router, "GET", "/dummy", get_dummy_json);

    while (1) {
        // TODO 1 : Wrap all of this in a function that can be called by a thread
        int clientSocket = accept(server.socket_fd, NULL, NULL);
        if (clientSocket == -1) {
            perror("Can't establish connection with the client");
            continue;
        }
        pthread_t thread;
        //TODO : fix the dangling pointer by doing a malloc
        client_ctx_t *ctx = (client_ctx_t *) malloc(sizeof(client_ctx_t));
        ctx->client_fd = clientSocket;
        ctx->router = &router;
        int pthread_ws = pthread_create(&thread, NULL, handle_client_thread, ctx);
        if (pthread_ws != 0) {
            fprintf(stderr, "Cannot create thread: %s\n", strerror(pthread_ws));
            continue;
        }
        pthread_detach(thread);
    }
}
