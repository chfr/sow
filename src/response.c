
#include "response.h"


response *response_new() {
	response *ret = malloc(sizeof(response));
	memset(ret, 0, sizeof(response));
	printf("Made response obj at %p\n", (void *)ret);
	return ret;
}

void response_clear(response *resp) {
	if (!resp)
		return;
	printf("Freeing response obj at %p\n", (void *)resp);
	if (resp->status_message)
		free(resp->status_message);
	if (resp->content_type)
		free(resp->content_type);
	if (resp->body) {
		printf("Freeing body at %p\n", (void *)resp->body);
		free(resp->body);
	}

	free(resp);
}

int response_write(response *resp, int fd) {
	if (!resp || !fd)
		return 1;

	char linebuf[LINEBUF_SIZE] = "";


	sprintf(linebuf, "HTTP/1.1 %d %s\r\n", resp->status_code, resp->status_message);
	write(fd, linebuf, strlen(linebuf));
	sprintf(linebuf, "Content-Type: %s\r\n", resp->content_type);
	write(fd, linebuf, strlen(linebuf));
	sprintf(linebuf, "Content-Length: %d\r\n", resp->content_length);
	write(fd, linebuf, strlen(linebuf));
	write(fd, "\r\n", 2);
	write(fd, resp->body, resp->content_length);
	write(fd, "\r\n\r\n", 4);
	return 0;
}

void response_set_status_code(response *resp, int code) {
	if (!resp)
		return;
	resp->status_code = code;

	if (resp->status_message)
		free(resp->status_message);
	
	// TODO set status_message properly here
	char *msg;
	if (code == 200) {
		msg = "OK";
	} else if (code == 404) {
		msg = "Not Found";
	} else {
		char err_msg[100] = "";
		sprintf(err_msg, "Status code %d not implemented!\n", code);
		perror(err_msg);
	}
	resp->status_message = malloc(sizeof(char)*strlen(msg));
	strcpy(resp->status_message, msg);
}

int response_get_status_code(response *resp) {
	if (!resp)
		return -1;

	return resp->status_code;
}
char *response_get_status_message(response *resp) {
	if (!resp)
		return NULL;

	return resp->status_message;
}

void response_set_content_length(response *resp, int length) {
	if (!resp)
		return;

	resp->content_length = length;
}
int response_get_content_length(response *resp) {
	if (!resp)
		return -1;

	return resp->content_length;
}

void response_set_content_type(response *resp, char *type) {
	if (!resp)
		return;

	if (resp->content_type)
		free(resp->content_type);

	resp->content_type = malloc(sizeof(char)*strlen(type));
	strcpy(resp->content_type, type);
}

char *response_get_content_type(response *resp) {
	if (!resp)
		return NULL;

	return resp->content_type;
}

void response_set_body(response *resp, char *body) {
	if (!resp || !body)
		return;

	int len = strlen(body);

	if (resp->body)
		free(resp->body);

	resp->body = malloc(sizeof(char)*len+1);
	printf("Malloc'd body at %p\n", (void *)resp->body);
	strcpy(resp->body, body);
	*(resp->body + len + 1) = '\0';
	printf("Setting response Content-Length to %d\n", len);
	resp->content_length = len;
}

char *response_get_body(response *resp) {
	if (!resp)
		return NULL;

	return resp->body;
}
