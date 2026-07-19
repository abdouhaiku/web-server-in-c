//
// Created by Abdou on 18/07/2026.
//

#include "application_routes.h"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void get_dummy_json(request_t *req, response_t *res) {
    // Read from a json file
    FILE *file = fopen("resource/assets/dummy.json", "r");
    if (file == NULL) {
        perror("File is not found");
        char error_text[] = "Error while processing your request";
        res->statusCode = 404;
        strcpy(res->status_text ,"Not found");
        strcpy(res->content_type, "text/html");
        res->body = malloc(strlen(error_text) + 1);
        strcpy(res->body, error_text);
        res->body_length = strlen(error_text);

        return;
    }
    else {
        // Calculate the size of the file
        char mimeType[32];
        getMimeType("resource/asssets/dummy.json", mimeType);
        strcpy(res->content_type, mimeType);
        fseek(file, 0, SEEK_END);
        long fsize = ftell(file);
        rewind(file);
        res->body_length = fsize;
        // Read the content of the file inside a buffer
        res->body = malloc(fsize + 1);
        fread(res->body, res->body_length, 1, file);
        res->statusCode = 200;
        strcpy(res->status_text, "OK");
        fclose(file);

    }
}

void get_full_name(request_t *req, response_t *res) {
    // Get the arguments from the request
    char *first_name = 0;
    char *last_name = 0;
    for (int i = 0; i<req->query_count; i++) {
        if (strcmp(req->query_params[i].key,"first_name") == 0) {
            first_name = req->query_params[i].value;
        }
        if (strcmp(req->query_params[i].key,"last_name") == 0) {
            last_name = req->query_params[i].value;
        }
    }

    if (!first_name || !last_name) {
        res->statusCode = 400;
        strcpy(res->status_text, "Bad Request");
        strcpy(res->content_type, "text/html");
        char msg[] = "Missing first_name or last_name";
        res->body = malloc(strlen(msg) + 1);
        strcpy(res->body, msg);
        res->body_length = strlen(msg);
        return;
    }

    char body[1000];
    sprintf(body, "Hello %s %s how is it going ?", first_name, last_name);

    res->body_length = strlen(body);
    // Read the content of the file inside a buffer
    res->body = malloc(strlen(body) + 1);
    strcpy(res->body, body);
    res->statusCode = 200;
    strcpy(res->status_text, "OK");
    strcpy(res->content_type, "text/html");

}

