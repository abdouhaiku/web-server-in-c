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

