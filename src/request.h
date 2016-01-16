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


void print_req(request *req);
request *parse_request(struct strlist *strlist);
