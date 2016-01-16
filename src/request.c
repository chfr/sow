#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "request.h"
#include "strlist.h"

request *request_parse(struct strlist *list) {
	char *line;
	int i;
	request *req = (request *)malloc(sizeof(request));

	for (i = 0; list; list = list->next, i++) {
		line = list->data;

		if (!line)
			continue;

		if (i == 0) {
			// buffer overflow? no way man, who would do such a thing
			char b1[10], b2[100], b3[10];
			
			sscanf(line, "%s %s %s\n", b1, b2, b3);
			req->path = (char *)malloc(sizeof(char)*strlen(b2));
			strcpy(req->path, b2);
			if (!strcmp(b1, "GET")) {
				req->method = GET;
			} else if (!strcmp(b1, "POST")) {
				req->method = POST;
			} else {
				printf("WARNING: unknown HTTP method in line:\n%s\n", line);
			}

			req->length = 1337;
			
		} else {
			if (strstr(line, "Host: ")) {

			} else if (strstr(line, "User-Agent: ")) {
				req->user_agent = line + 12;
			} else if (strstr(line, "Accept: ")) {

			} else if (strstr(line, "Accept-Language: ")) {

			} else if (strstr(line, "Accept-Encoding: ")) {

			} else if (strstr(line, "Connection: ")) {

			} else {
				printf("WARNING: unknown HTTP header in line:\n%s\n", line);
			}
		}
	}

	return req;
}

void request_print(request *req) {
	printf("request obj at %p", (void *)req);
	printf("%d %s HTTP/X.X\n", req->method, req->path);
	printf("ua: %s\n", req->user_agent);
	printf("len: %d\n", req->length);
}


