//
// Created by Abdou on 17/07/2026.
//

#ifndef WEB_SERVER_IN_C_ROUTER_H
#define WEB_SERVER_IN_C_ROUTER_H

#define MAX_ROUTES 32

typedef struct request request_t;
typedef struct response response_t;
typedef struct router router_t;

typedef void (*handler_fn)(request_t *req, response_t *res);

typedef struct {
    char method[8];
    char path[256];
    handler_fn handler;
} route_t;

struct router {
    route_t routes[MAX_ROUTES];
    int count;
};


int router_add(router_t *router, const char* method, const char* path, handler_fn handler);
handler_fn router_match(router_t *router, request_t *req);
#endif //WEB_SERVER_IN_C_ROUTER_H
