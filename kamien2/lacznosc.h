
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>

typedef struct _Memory
{
    char *response;
    size_t size;
} Memory;

static size_t write_callback(void *data, size_t size, size_t nmemb, void *userp);
char *make_request(char *url, int n);
