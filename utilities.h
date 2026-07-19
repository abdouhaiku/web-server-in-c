//
// Created by Abdou on 11/07/2026.
//

#ifndef WEB_SERVER_IN_C_UTILITIES_H
#define WEB_SERVER_IN_C_UTILITIES_H
#include <sys/_types/_size_t.h>
typedef struct router router_t;
typedef struct request request_t;
typedef struct response response_t;

void getFileURL(char*, char*);
void getMimeType(char*, char*);
void getTimeString(char*);

typedef struct {
    char name[64];
    char value[256];
} header_t;

struct request {
    char method[8];
    char path[256];
    char version[16];
    header_t headers[32];
    int header_count;
    char *body; // points inside the raw request buffer passed to parse_request
};

struct response{
    int statusCode;
    char status_text[32];
    char content_type[32];
    char *body;
    size_t body_length;
};

typedef struct {
    int client_fd;
    router_t *router;
} client_ctx_t;

// Parses a raw, null-terminated HTTP request into req. Returns 0 on success,
// -1 if the request line or headers are malformed. req->body points into
// raw, so raw must stay allocated for as long as req is used.
int parse_request(char *raw, request_t *req);

void send_response(int client_socket, response_t *response);

void *handle_client_thread(void* arg);
#endif //WEB_SERVER_IN_C_UTILITIES_H
