//
// Created by Abdou on 16/07/2026.
//

#ifndef WEB_SERVER_IN_C_SERVER_CONFIG_H
#define WEB_SERVER_IN_C_SERVER_CONFIG_H
typedef struct {
    int port;
    int backlog;
    char bind_address[32];
    int number_worker_threads;
    int maximum_of_connections;
} server_config_t;

typedef struct {
    int socket_fd;
    server_config_t config;
} server_t;

int parse_config_ini(char*, server_config_t*);

int create_server_socket(server_t*);

int server_init(char*, server_t*);


#endif //WEB_SERVER_IN_C_SERVER_CONFIG_H
