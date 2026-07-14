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


#include "utilities.h"

#define PORT 8081
#define BACKLOG 10


int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    struct sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // Create socket and return file descriptor
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    //Bind and check
    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error: The server is not bound to the address");
        return errno;
    }

    //List to the server socket
    if (listen(serverSocket, BACKLOG) < 0) {
        printf("Error: the server is not listening %d\n", errno);
        return 1;
    }

    char hostBuffer[NI_MAXHOST], serviceBuffer[NI_MAXSERV];
    int error = getnameinfo((struct sockaddr *) &serverAddress, sizeof(serverAddress), hostBuffer,
                            sizeof(hostBuffer), serviceBuffer, sizeof(serviceBuffer), 0);

    if (error != 0) {
        printf("Error: %s\n", gai_strerror(error));
        return 1;
    }

    printf("\nServer is listening on http://%s:%s/\n\n", hostBuffer, serviceBuffer);

    while (1) {

        // TODO 1 : Wrap all of this in a function that can be called by a thread
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == -1) {
            perror("Can't establish connection with the client");
            continue;
        }

        pthread_t thread;
        int *client_ptr = malloc(sizeof(int));
        *client_ptr = clientSocket;
        int pthread_ws = pthread_create(&thread, NULL, handle_client_thread, client_ptr);
        if (pthread_ws  != 0) {
            fprintf(stderr, "Cannot create thread: %s\n", strerror(pthread_ws));
            continue;

        }
        pthread_detach(thread);

    }
}
