typedef enum {GET, POST} http_method;

struct strlist;

typedef struct request {
	http_method method;
	char *path;
	char* user_agent;
	int major_ver;
	int minor_ver;
	int length;
} request;


void request_print(request *req);
request *request_parse(struct strlist *strlist);
