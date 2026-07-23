//
// Created by Abdou on 11/07/2026.
//

#include "utilities.h"
#include "router.h"

#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "queue.h"

#define SIZE 10000

typedef struct router router_t;
typedef struct request request_t;
typedef struct response response_t;


void log_request(request_t *req, response_t *res, struct timespec begin_time, int client_fd);

void getFileURL(char *route, char *fileURL) {
    if (route[strlen(route) - 1] == '/') {
        strcat(route, "index.html");
    }
    strcpy(fileURL, "resource/assets");
    strcat(fileURL, route);

    const char *dot = strrchr(fileURL, '.');
    if (!dot || dot == fileURL) {
        strcat(fileURL, ".html");
    }
}

void getMimeType(char *file, char *mime) {
    const char *dot = strrchr(file, '.');

    if (dot == NULL)
        strcpy(mime, "text/html");

    else if (strcmp(dot, ".html") == 0)
        strcpy(mime, "text/html");

    else if (strcmp(dot, ".css") == 0)
        strcpy(mime, "text/css");

    else if (strcmp(dot, ".js") == 0)
        strcpy(mime, "application/js");

    else if (strcmp(dot, ".jpg") == 0)
        strcpy(mime, "image/jpeg");

    else if (strcmp(dot, ".png") == 0)
        strcpy(mime, "image/png");

    else if (strcmp(dot, ".gif") == 0)
        strcpy(mime, "image/gif");

    else
        strcpy(mime, "text/html");
}


int parse_request(char *raw, request_t *req) {
    memset(req, 0, sizeof(*req));

    char *line_end = strstr(raw, "\r\n");
    if (!line_end)
        return -1;

    if (sscanf(raw, "%7s %255s %15s", req->method, req->path, req->version) != 3)
        return -1;

    char *query_start = strchr(req->path, '?');
    if (query_start) {
        *query_start = '\0';
        query_start++;
        while (query_start) {

            char *next_param_start = strchr(query_start, '&');
            char *param_end = next_param_start ? next_param_start : query_start + strlen(query_start);

            char *equal_sign = memchr(query_start, '=', param_end - query_start);
            if (equal_sign && req->query_count < 32) {
                size_t key_len = equal_sign - query_start;
                if (key_len >= sizeof(req->query_params[0].key))
                    key_len = sizeof(req->query_params[0].key) - 1;
                memcpy(req->query_params[req->query_count].key, query_start, key_len);
                req->query_params[req->query_count].key[key_len] = '\0';

                char *value_start = equal_sign + 1;
                size_t value_len = param_end - value_start;
                if (value_len >= sizeof(req->query_params[0].value))
                    value_len = sizeof(req->query_params[0].value) - 1;
                memcpy(req->query_params[req->query_count].value, value_start, value_len);
                req->query_params[req->query_count].value[value_len] = '\0';
                req->query_count++;
            }

            query_start = next_param_start ? next_param_start + 1 : NULL;
        }
    }

    char *cursor = line_end + 2;

    // Header lines, terminated by a blank line ("\r\n\r\n")
    while (cursor[0] != '\r' || cursor[1] != '\n') {
        char *next_line = strstr(cursor, "\r\n");
        if (!next_line)
            return -1;

        char *colon = memchr(cursor, ':', next_line - cursor);
        if (colon && req->header_count < 32) {
            header_t *h = &req->headers[req->header_count];

            size_t name_len = colon - cursor;
            if (name_len >= sizeof(h->name))
                name_len = sizeof(h->name) - 1;
            memcpy(h->name, cursor, name_len);
            h->name[name_len] = '\0';

            char *value_start = colon + 1;
            while (value_start < next_line && *value_start == ' ')
                value_start++;

            size_t value_len = next_line - value_start;
            if (value_len >= sizeof(h->value))
                value_len = sizeof(h->value) - 1;
            memcpy(h->value, value_start, value_len);
            h->value[value_len] = '\0';

            req->header_count++;
        }

        cursor = next_line + 2;
    }

    req->body = cursor + 2;

    return 0;
}

char *build_headers(response_t *response) {
    char *buffer = malloc(1000 * sizeof(char));
    char status_line[100];
    sprintf(status_line, "HTTP/1.1 %d %s\r\n", response->statusCode, response->status_text);
    strcpy(buffer, status_line);
    char content_type_line[100];
    sprintf(content_type_line, "Content-Type: %s\r\n", response->content_type);
    strcat(buffer, content_type_line);
    char content_length[100];
    sprintf(content_length, "Content-Length: %lu\r\n", response->body_length);
    strcat(buffer, content_length);
    char date[100];
    getTimeString(date);
    strcat(buffer, date);
    // Add connection:close header
    strcat(buffer, "Connection:close");
    strcat(buffer, "\r\n\r\n");
    return buffer;
}

/*
HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n
Content-Length: 1234\r\n
Date: Mon, 14 Jul 2026 10:00:00 GMT\r\n
\r\n
<body bytes go here — no \r\n line-ending rules apply inside the body>
*/

void send_response(int clientSocket, response_t *response) {
    char *headers = build_headers(response);
    int header_size = strlen(headers);
    char *responseBuffer = malloc(strlen(headers) + response->body_length);
    memcpy(responseBuffer, headers, header_size);
    free(headers);
    // pointer arithmetic to point to the begining to the position of the resource
    char *bodyBuffer = responseBuffer + header_size;
    memcpy(bodyBuffer, response->body, response->body_length);
    send(clientSocket, responseBuffer, header_size + response->body_length, 0);
    free(responseBuffer);
}


void getTimeString(char *timeBuff) {
    const time_t t = time(NULL);
    //convert time to local structure
    struct tm tm_info_storage;
    const struct tm *tm_info = localtime_r(&t, &tm_info_storage);
    sprintf(timeBuff, "Date: %02d/%02d/%04d %02d:%02d:%02d",
            tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900,
            tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
}

void handle_static_file(request_t *req, response_t *res) {
    char fileURL[300];
    getFileURL(req->path, fileURL);

    FILE *file = fopen(fileURL, "r");

    if (!file) {
        char error_text[] = "Error while processing your request";
        res->statusCode = 404;
        strcpy(res->status_text, "Not Found");
        strcpy(res->content_type, "text/html");
        res->body = malloc(strlen(error_text) + 1);
        strcpy(res->body, error_text);
        res->body_length = strlen(error_text);
        return;
    } else {
        char mimeType[32];
        getMimeType(fileURL, mimeType);
        strcpy(res->content_type, mimeType);
        // Calculate the size of the file
        fseek(file, 0, SEEK_END);
        long fsize = ftell(file);
        // Equivalent to fseek(stream, 0L, SEEK_SET)
        rewind(file);
        res->body_length = fsize;
        // Read content of the file
        res->body = malloc(res->body_length * sizeof(char));
        fread(res->body, res->body_length, 1, file);
        res->statusCode = 200;
        strcpy(res->status_text, "OK");
        fclose(file);
    }
}

void handle_client(client_ctx_t ctx) {
    struct timespec begin_time;
    clock_gettime(CLOCK_MONOTONIC, &begin_time);
    char *raw = (char *) calloc(SIZE + 1, sizeof(char));
    read(ctx.client_fd, raw, SIZE);

    request_t req;
    if (parse_request(raw, &req) != 0) {
        //TODO: refactor to a send_404_error()
        char error_text[] = "Error while processing your request";
        size_t body_length = strlen(error_text);
        response_t response = {
            404, "Bad Request", "text/html", error_text, body_length
        };
        send_response(ctx.client_fd, &response);
        close(ctx.client_fd);
        free(raw);
        fprintf(stderr, "Malformed request from client %d\n", ctx.client_fd);
        return;
    }

    printf("%s %s\n", req.method, req.path);

    // TODO-1: refactor with route handling
    handler_fn h = router_match(ctx.router, &req);
    response_t response = {0};
    if (h) {
        h(&req, &response);
    } else {
        handle_static_file(&req, &response);
    }
    send_response(ctx.client_fd, &response);
    log_request(&req, &response, begin_time, ctx.client_fd);
    free(response.body);
    close(ctx.client_fd);
    //END TODO-1
    free(raw);
}

void *handle_client_thread(client_ctx_t ctx) {
    handle_client(ctx);
    return NULL;
}

void *worker_loop(void *arg) {

    conn_queue_t *conn_queue = (conn_queue_t *) arg;
    while (1) {
        //queue_pop is blocking and will suspend thread if no connection is available
        client_ctx_t ctx = queue_pop(conn_queue);
        handle_client_thread(ctx);
    }

}


void log_request(request_t *req, response_t *res, struct timespec begin_time, int client_fd) {
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    long total_ns = (end_time.tv_sec - begin_time.tv_sec) * 1000000000L
                    + (end_time.tv_nsec - begin_time.tv_nsec);

    char timeBuffer[100];
    getTimeString(timeBuffer);

    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);
    char serverAddress[INET_ADDRSTRLEN] = "unknown";
    if (getsockname(client_fd, (struct sockaddr *) &local_addr, &addr_len) == 0) {
        inet_ntop(AF_INET, &local_addr.sin_addr, serverAddress, sizeof(serverAddress));
    }

    pthread_t tid = pthread_self();

    printf("%s thread=%lu ip=%s %s %s %d %ldns\n", timeBuffer, (unsigned long) tid, serverAddress,
           req->method, req->path, res->statusCode, total_ns);
}
