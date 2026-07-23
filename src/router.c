//
// Created by Abdou on 17/07/2026.
//

#include "router.h"
#include "utilities.h"

#include <stdio.h>
#include <string.h>

int router_add(router_t *router, const char* method, const char* path, handler_fn handler) {
    if (router->count < MAX_ROUTES) {
        route_t *new_route = &router->routes[router->count];
        if (strlen(method) >= sizeof(new_route->method) || strlen(path) >= sizeof(new_route->path)) {
            fprintf(stderr, "route method/path too long: %s %s\n", method, path);
            return 1;
        }
        else {
            strcpy(new_route->path, path);
            strcpy(new_route->method, method);
            new_route->handler = handler;
            router->count++;
            return 0;
        }
    }
    else {
        printf("MAX ROUTES is reached. Cannot add anymore routes");
        return 1;
    }
}


handler_fn router_match(router_t *router, request_t *req) {
    for (int i=0; i<router->count; i++) {
        if (strcmp(router->routes[i].path, req->path) == 0 && strcmp(req->method, router->routes[i].method) == 0) {
            return router->routes[i].handler;
        }
    }
    return NULL;
}