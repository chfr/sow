#pragma once

#define HDR_HOST "Host"
#define HDR_USER_AGENT "User-Agent"
#define HDR_ACCEPT "Accept"
#define HDR_ACCEPT_LANGUAGE "Accept-Language"
#define HDR_ACCEPT_ENCODING "Accept-Encoding"
#define HDR_CONNECTION "Connection"
#define HDR_REFERER "Referer"
#define HDR_CONTENT_TYPE "Content-Type"
#define HDR_CONTENT_LENGTH "Content-Length"

typedef enum {GET, POST} http_method;

struct strlist;

typedef struct request {
	http_method method;
	char *path;
	int major_ver;
	int minor_ver;
	char *host;
	char *accept;
	char *accept_language;
	char *accept_encoding;
	char *connection;
	char *user_agent;
	char *referer;
	char *content_type;
	char *get_params;
	int content_length;

	char *body;
} request;


request *request_new();
int request_parse(request *req, char *line);
void request_parse_path(request *req, char *line);
void request_print(request *req);
void request_clear(request *req);

int request_get_content_length(request *req);
char *request_get_method_string(request *req);
int request_has_get_parameter(request *req, char *name);
char *request_get_get_parameter_value(request *req, char *name);
