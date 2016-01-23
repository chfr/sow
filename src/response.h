#pragma once

#include <string.h>
#include <stdlib.h>

typedef struct response {
	int status_code;
	char *status_message;
	int content_length;
	char *content_type;
	char *body;
} response;

response *response_new();
void response_clear(response *resp);
void response_set_status_code(response *resp, int code);
int response_get_status_code(response *resp);
char *response_get_status_message(response *resp);
void response_set_content_length(response *resp, int length);
int response_get_content_length(response *resp);
void response_set_content_type(response *resp, char *type);
char *response_get_content_type(response *resp);
void response_set_body(response *resp, char *body);
char *response_get_body(response *resp);
