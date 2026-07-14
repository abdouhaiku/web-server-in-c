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
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == -1) {
            perror("Can't establish connection with the client");
            return errno;
        }

        char *raw = (char *) calloc(SIZE + 1, sizeof(char));
        read(clientSocket, raw, SIZE);

        request_t req;
        if (parse_request(raw, &req) != 0) {
            char error_text[] = "Error while processing your request";
            int body_length = strlen(error_text);
            response_t response = {
                404, "Bad Request", "text/html",error_text, body_length
            };
            send_response(clientSocket, &response);
            close(clientSocket);
            free(raw);
            continue;
        }

        printf("%s %s\n", req.method, req.path);

        char fileURL[300];
        getFileURL(req.path, fileURL);

        FILE *file = fopen(fileURL, "r");

        if (!file) {
            char error_text[] = "Error while processing your request";
            int body_length = strlen(error_text);
            response_t response = {
                404, "Not found", "text/html",error_text, body_length
            };
            send_response(clientSocket, &response);
            close(clientSocket);
        } else {
            response_t response;
            char mimeType[32];
            getMimeType(fileURL, mimeType);
            response.content_type = mimeType;
            // Calculate the size of the file
            fseek(file, 0, SEEK_END);
            long fsize = ftell(file);
            // Equivalent to fseek(stream, 0L, SEEK_SET)
            rewind(file);
            response.body_length = fsize;
            // Read content of the file
            response.body = malloc(response.body_length * sizeof(char));
            fread(response.body, response.body_length, 1, file);
            response.statusCode = 200;
            response.status_text = "OK";
            send_response(clientSocket, &response);
            free(response.body);
            fclose(file);
        }

        free(raw);
    }
}
