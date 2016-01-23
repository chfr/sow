
#include "response.h"


response *response_new() {
	response *ret = malloc(sizeof(response));
	memset(ret, 0, sizeof(response));
	return ret;
}

void response_clear(response *resp) {
	if (resp->status_message)
		free(resp->status_message);
	if (resp->content_type)
		free(resp->content_type);
	if (resp->body)
		free(resp->body);

	free(resp);
}
void response_set_status_code(response *resp, int code) {
	if (!resp)
		return;
	resp->status_code = code;

	if (resp->status_message)
		free(resp->status_message);
	
	// TODO set status_message properly here
	resp->status_message = malloc(sizeof(char)*3);
	strcpy(resp->status_message, "OK\0");
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
	if (!resp)
		return;

	if (resp->body)
		free(resp->body);

	resp->body = malloc(sizeof(char)*strlen(body));
	strcpy(resp->body, body);
}

char *response_get_body(response *resp) {
	if (!resp)
		return NULL;

	return resp->body;
}
