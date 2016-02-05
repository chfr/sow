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
		return 0;
	}

	c = word_end(line);
	*c = '\0';

	if (!strcmp(line, HDR_HOST":")) {
		line = skip_whitespace(c+1);
		req->host = malloc(sizeof(char)*strlen(line));
		strcpy(req->host, line);
	} else if (!strcmp(line, HDR_USER_AGENT":")) {
		line = skip_whitespace(c+1);
		req->user_agent = malloc(sizeof(char)*strlen(line));
		strcpy(req->user_agent, line);
	} else if (!strcmp(line, HDR_ACCEPT":")) {
		line = skip_whitespace(c+1);
		req->accept = malloc(sizeof(char)*strlen(line));
		strcpy(req->accept, line);
	} else if (!strcmp(line, HDR_ACCEPT_LANGUAGE":")) {
		line = skip_whitespace(c+1);
		req->accept_language = malloc(sizeof(char)*strlen(line));
		strcpy(req->accept_language, line);
	} else if (!strcmp(line, HDR_ACCEPT_ENCODING":")) {
		line = skip_whitespace(c+1);
		req->accept_encoding = malloc(sizeof(char)*strlen(line));
		strcpy(req->accept_encoding, line);
	} else if (!strcmp(line, HDR_CONNECTION":")) {
		line = skip_whitespace(c+1);
		req->connection = malloc(sizeof(char)*strlen(line));
		strcpy(req->connection, line);
	} else if (!strcmp(line, HDR_REFERER":")) {
		line = skip_whitespace(c+1);
		req->referer = malloc(sizeof(char)*strlen(line));
		strcpy(req->referer, line);
	} else if (!strcmp(line, HDR_CONTENT_TYPE":")) {
		line = skip_whitespace(c+1);
		req->content_type = malloc(sizeof(char)*strlen(line));
		strcpy(req->content_type, line);
	} else if (!strcmp(line, HDR_CONTENT_LENGTH":")) {
		line = skip_whitespace(c+1);
		req->content_length = atoi(line);
	} else {
		*c = ' ';
		WARN("Unknown header: %s\n", line);
	}
	

	return 0;
}

void request_print(request *req) {
	printf("Request object at %p\n", (void *)req);
	printf("%s %s HTTP/%d.%d\n",
		request_get_method_string(req), req->path, req->major_ver, req->minor_ver);

	if (req->host)
		printf(HDR_HOST": %s\n", req->host);
	if (req->user_agent)
		printf(HDR_USER_AGENT": %s\n", req->user_agent);
	if (req->accept)
		printf(HDR_ACCEPT": %s\n", req->accept);
	if (req->accept_language)
		printf(HDR_ACCEPT_LANGUAGE": %s\n", req->accept_language);
	if (req->accept_encoding)
		printf(HDR_ACCEPT_ENCODING": %s\n", req->accept_encoding);
	if (req->connection)
		printf(HDR_CONNECTION": %s\n", req->connection);
	if (req->referer)
		printf(HDR_REFERER": %s\n", req->referer);
	if (req->content_type)
		printf(HDR_CONTENT_TYPE": %s\n", req->content_type);

	if (req->content_length >= 0)
		printf("Content-Length: %d\n", req->content_length);

	if (req->body)
		printf("\n%s\n", req->body);
}

void request_clear(request *req) {
	if (req->path)
		free(req->path);
	if (req->user_agent)
		free(req->user_agent);
	if (req->host)
		free(req->host);
	if (req->accept)
		free(req->accept);
	if (req->accept_language)
		free(req->accept_language);
	if (req->accept_encoding)
		free(req->accept_encoding);
	if (req->connection)
		free(req->connection);
	if (req->referer)
		free(req->referer);
	if (req->content_type)
		free(req->content_type);

	if (req->body)
		free(req->body);

	free(req);
}


int request_get_content_length(request *req) {
	return req->content_length;
}

char *request_get_method_string(request *req) {
	if (req->method == GET)
		return "GET";
	else if (req->method == POST)
		return "POST";
	else
		return "UNKNOWN";
}
