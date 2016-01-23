#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "request.h"
#include "strlist.h"
#include "utils.h"

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 3
#endif

request *request_new() {
	request *req = malloc(sizeof(request));
	memset(req, 0, sizeof(request));

	req->content_length = -1;

	return req;
}

int request_parse(request *req, char *line) {
	char *c;
	
	if (!req->path) { // has to be the first line, otherwise error
		c = word_end(line);
		*c = '\0';
		
		if (!strcmp(line, "GET")) {
			req->method = GET;
		} else if (!strcmp(line, "POST")) {
			req->method = POST;
		} else {
			return 1;
		}

		line = skip_whitespace(c+1);
		c = word_end(line);
		*c = '\0';
		
		req->path = malloc(sizeof(char)*strlen(line));
		strcpy(req->path, line);

		line = skip_whitespace(c+1);
		c = word_end(line);
		*c = '\0';
		
		if (!(is_number(line[5]) && is_number(line[7]))) {
			ERROR("Invalid version in line: %s\n", line);
			return 1;
		}
		line[6] = '\0';
		req->major_ver = atoi(&line[5]);
		req->minor_ver = atoi(&line[7]);
	}
	
	return 0;
}

int request_get_content_length(request *req) {
	return req->content_length;
}

void request_print(request *req) {
	printf("Request object at %p\n", (void *)req);
	printf("%d %s HTTP/%d.%d\n",
		req->method, req->path, req->major_ver, req->minor_ver);
	printf("User-Agent: %s\n", req->user_agent);
	printf("Content-Length: %d\n", req->content_length);
}

void request_clear(request *req) {
	if (req->path)
		free(req->path);

	if (req->user_agent)
		free(req->user_agent);

	free(req);
}


