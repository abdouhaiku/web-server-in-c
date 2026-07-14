//
// Created by Abdou on 11/07/2026.
//

#ifndef WEB_SERVER_IN_C_UTILITIES_H
#define WEB_SERVER_IN_C_UTILITIES_H
#include <sys/_types/_size_t.h>

void getFileURL(char*, char*);
void getMimeType(char*, char*);
void getTimeString(char*);

typedef struct {
    char name[64];
    char value[256];
} header_t;

typedef struct {
    char method[8];
    char path[256];
    char version[16];
    header_t headers[32];
    int header_count;
    char *body; // points inside the raw request buffer passed to parse_request
} request_t;

typedef struct {
    int statusCode;
    char *status_text;
    char *content_type;
    char *body;
    size_t body_length;
} response_t;

// Parses a raw, null-terminated HTTP request into req. Returns 0 on success,
// -1 if the request line or headers are malformed. req->body points into
// raw, so raw must stay allocated for as long as req is used.
int parse_request(char *raw, request_t *req);

void send_response(int client_socket, response_t *response);
#endif //WEB_SERVER_IN_C_UTILITIES_H
