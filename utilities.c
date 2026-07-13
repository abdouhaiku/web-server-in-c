//
// Created by Abdou on 11/07/2026.
//

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