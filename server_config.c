//
// Created by Abdou on 16/07/2026.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_config.h"
#include <netdb.h>  // getnameinfo
#include <sys/socket.h> // socket APIs
#include <netinet/in.h> // sockaddr_in
#include <unistd.h>     // open, close
#include <signal.h> // signal handling
#include <arpa/inet.h>
#include <sys/errno.h>


int parse_config_ini(char* path, server_config_t *config) {
    memset(config, 0, sizeof(*config));
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("File config.ini not found");
        return 1;
    }
    // Calculate the size of the file
    fseek(file, 0, SEEK_END);
    int fsize = ftell(file);
    rewind(file);
    // Read the content of the file inside a buffer
    char buffer[fsize + 1];
    size_t bytes_read = fread(buffer, 1, fsize, file);
    buffer[bytes_read] = '\0';
    fclose(file);
    for (int i = 0; buffer[i] != '\0'; i++) {
        while (buffer[i] != '[' && buffer[i] != '\0') {
            i++;
        }
        if (buffer[i] == '\0') {
            break;
        }

        char *line_end = strchr(&buffer[i], '\n');
        if (line_end == NULL) {
            return 1;
        }
        char header[100];
        if (sscanf(&buffer[i], "[%99[^]]]", header) == 1) {
            if (strcmp(header, "server_config") != 0) {
                //skip to the next iteration
                continue;
            }
        } else {
            printf("Error in header format\n");
            return 1;
        }
        char *cursor = line_end + 1;
        while (*cursor != '\0') {
            char *next_line = strchr(cursor, '\n');
            int has_newline = (next_line != NULL);
            if (!has_newline) {
                next_line = cursor + strlen(cursor);   // points at the buffer's '\0'
            }
            if (next_line == NULL) {
                return 1;
            }
            //find a byte within a fixed size block
            char *equal_sign = memchr(cursor, '=', next_line - cursor);
            if (equal_sign) {
                char *first_whitespace = cursor;
                while (*first_whitespace != ' ' && first_whitespace < equal_sign) {
                    first_whitespace += 1;
                }
                size_t key_len = first_whitespace - cursor;
                char key[key_len];
                memcpy(key, cursor, key_len);
                key[key_len] = '\0';
                printf("Key value is %s\n", key);

                char *value_start = equal_sign + 1;
                while (value_start < next_line && *value_start == ' ') {
                    value_start++;
                }

                size_t value_len = next_line - value_start;
                char value[value_len];
                memcpy(value, value_start, value_len);
                value[value_len] = '\0';

                printf("Associated value of the last key is %s\n", value);

                if (strcmp(key, "port") == 0)
                    config->port = atoi(value);
                else if (strcmp(key, "backlog") == 0)
                    config->backlog = atoi(value);
                else if (strcmp(key, "bind_address") == 0)
                    strcpy(config->bind_address, value);
                else if (strcmp(key,"number_worker_threads") == 0)
                    config->number_worker_threads = atoi(value);
                else if (strcmp(key, "maximum_of_connections") == 0)
                    config->maximum_of_connections = atoi(value); 
            }
            cursor = next_line + 1;
        }
    }

    printf("Server config is the following : \n");
    printf("Port of the server config is %d\n", config->port);
    printf("Bind address of the server config is %s\n", config->bind_address);
    printf("Backlog of the server config is %d", config->backlog);

    return 0;
}

int create_server_socket(server_t* server) {
    struct sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(server->config.port);

    if (inet_pton(AF_INET, server->config.bind_address, &serverAddress.sin_addr) !=1) {
        perror("Invalid IP address");
        return 1;
    }

    // Create socket and return file descriptor
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    server->socket_fd = serverSocket;

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        return 1;
    }

    //Bind and check
    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error: The server is not bound to the address");
        return errno;
    }


    //List to the server socket
    if (listen(serverSocket, server->config.backlog) < 0) {
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
    return 0;
}

int server_init(char* path, server_t *server) {
    if (parse_config_ini(path, &server->config) != 0) {
        printf("Error in parsing the config file\n");
        return 1;
    }
    return create_server_socket(server);


}
