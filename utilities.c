//
// Created by Abdou on 11/07/2026.
//

#include "utilities.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void getFileURL(char *route, char *fileURL)
{
    char *question = strrchr(route, '?');
    if (question)
        *question = '\0';

    if (route[strlen(route) - 1] == '/')
    {
        strcat(route, "index.html");
    }

    strcpy(fileURL, "resource/assets");
    strcat(fileURL, route);

    const char *dot = strrchr(fileURL, '.');
    if (!dot || dot == fileURL)
    {
        strcat(fileURL, ".html");
    }
}

void getMimeType(char *file, char *mime)
{
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


int parse_request(char *raw, request_t *req)
{
    memset(req, 0, sizeof(*req));

    char *line_end = strstr(raw, "\r\n");
    if (!line_end)
        return -1;

    if (sscanf(raw, "%7s %255s %15s", req->method, req->path, req->version) != 3)
        return -1;

    char *cursor = line_end + 2;

    // Header lines, terminated by a blank line ("\r\n\r\n")
    while (cursor[0] != '\r' || cursor[1] != '\n')
    {
        char *next_line = strstr(cursor, "\r\n");
        if (!next_line)
            return -1;

        char *colon = memchr(cursor, ':', next_line - cursor);
        if (colon && req->header_count < 32)
        {
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

void getTimeString(char* timeBuff) {
    time_t t = time(NULL);
    //convert time to local structure
    struct tm *tm_info = localtime(&t);

    //print the formatted string in timebuff
    printf("Current Date and Time: %02d/%02d/%04d %02d:%02d:%02d\n",
           tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900,
           tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);

    sprintf(timeBuff,"Current Date and Time: %02d/%02d/%04d %02d:%02d:%02d",
           tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900,
           tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);

}