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

#include "utilities.h"

#define PORT 8081
#define BACKLOG 10
#define SIZE 10000


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
        char *request = (char *) malloc(SIZE * sizeof(char));
        char method[10], route[100];
        int clientSocket = accept(serverSocket, NULL, NULL);
        read(clientSocket, request, SIZE);
        // scan the input to get the first 10 chars for the method, and the next 100 char for the route
        if (clientSocket == -1) {
            perror("Can't establish connection with the client");
            free(request);
            return errno;
        }
        sscanf(request, "%s %s", method, route);
        printf("%s %s\n", method, route);
        printf("%s", request);
        free(request);

        char fileURL[100];
        getFileURL(route, fileURL);

        FILE *file = fopen(fileURL, "r");

        if (!file) {
            const char response[] = "HTTP/1.1 404 Not Found\r\n\n";
            send(clientSocket, response, sizeof(response), 0);
            close(clientSocket);
        } else {
            char resHeader[SIZE];
            char mimeType[32];

            getMimeType(fileURL, mimeType);

            char timeBuff[100];
            getTimeString(timeBuff);

            sprintf(resHeader, "HTTP/1.1 200 OK\r\nDate: %s\r\nContent-Type: %s\r\n\n", timeBuff, mimeType);
            int headerSize = strlen(resHeader);

            printf(" %s", mimeType);

            // Calculate the size of the file
            fseek(file, 0, SEEK_END);
            long fsize = ftell(file);
            // Equivalent to fseek(stream, 0L, SEEK_SET)
            rewind(file);

            // Allocate a new response buffer whose size is the sum of the header and size of the resource
            char *resBuffer = (char *) malloc(fsize + headerSize);
            strcpy(resBuffer, resHeader);
            // pointer arithmetic to point to the begining to the position of the resource
            char *fileBuffer = resBuffer + headerSize;
            fread(fileBuffer, fsize, 1, file);
            send(clientSocket, resBuffer, fsize + headerSize, 0);
            free(resBuffer);
            fclose(file);
            close(clientSocket);
        }
    }
}
