#pragma once

typedef enum {GET, POST} http_method;

struct strlist;

typedef struct request {
	http_method method;
	char *path;
	char* user_agent;
	int major_ver;
	int minor_ver;
	int content_length;
} request;


request *request_new();
int request_parse(request *req, char *line);
void request_print(request *req);
void request_clear(request *req);

int request_get_content_length(request *req);
