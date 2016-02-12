#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>

#include "strlist.h"
#include "request.h"
#include "utils.h"
#include "response.h"

#define SRVPORT 8080

#define MAXLEN 1024

void respond(request **req, int client_socket);
strlist *process_lines(strlist *head, char *buf);
response *make_response(request *req);
response *make_404_response();
strlist *readfile(char *filename);

int main(/*int argc, char *argv[]*/) {
	INFO("Started\n");
	int server_socket, client_socket, port, res, client_len, n;
	struct sockaddr_in srv_addr, client_addr;
	int iSetOption = 1;

	port = SRVPORT;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,
        sizeof(iSetOption));
        
	if (server_socket < 0) {
		ERROR("Error opening socket\n");
		IFERROR(perror(""));
	}
	INFO("Made server socket\n");

	memset((char *) &srv_addr, 0, sizeof(srv_addr));
	
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(port);
	srv_addr.sin_addr.s_addr = INADDR_ANY;

	while ((res = bind(server_socket,
			   (struct sockaddr *) &srv_addr,
			   sizeof(srv_addr))) < 0) {
		ERROR("Error binding socket\n");
		IFERROR(perror(""));
	}

	listen(server_socket, 5);
	INFO("Listening on port %d\n", port);

	client_len = sizeof(client_addr);

	if (chdir("content")) {
		ERROR("Could not chdir to the content directory\n");
		IFERROR(perror(""));
		exit(1);
	}

	do {
		client_socket = accept(server_socket,
							   (struct sockaddr *) &client_addr,
							   (socklen_t *) &client_len);

		INFO("Accepted client socket\n");

		if (client_socket < 0) {
			ERROR("Error on client accept\n");
			IFERROR(perror(""));
		}

		request *req = request_new();
		char line[MAXLEN];

		while ((n = fd_readline(client_socket, line, MAXLEN)) > 0) {
			if (n == MAXLEN-1 && line[MAXLEN-1] != '\n') {
				WARN("Line length exceeded MAXLEN %d\n", MAXLEN);
				char c;
				int dropped = 0;
				while ((n = read(client_socket, &c, 1)) > 0) {
					dropped += 1;
					if (c == '\n')
						break;
				}
				WARN("Skipped to the next line, dropped %d characters\n", dropped);	
			}

			if (is_crlf(line)) { // We've reached the end of the headers
				int left_to_read = request_get_content_length(req);
				int total_read = 0, len;
				
				if (left_to_read > 0) {
					DEBUG("There's %d chars of body to read\n", left_to_read);
					// There's a Content-Length so there must be a body
					req->body = malloc(sizeof(char) * (left_to_read+1));
					
					len = MAXLEN < left_to_read ? MAXLEN : left_to_read;
					len += 1;
					
					while (left_to_read && (n = fd_readline(client_socket, line, len)) > 0) {
						DEBUG("Read %d characters of body: %s\n", n, line);
						strcpy(&req->body[total_read], line);
						total_read += n;
						left_to_read -= n;
						DEBUG("total_read: %d, left_to_read: %d\n", total_read, left_to_read);
						len = MAXLEN < left_to_read ? MAXLEN : left_to_read;
					}
					req->body[total_read+1] = '\0';
					DEBUG("Done reading body in %d chars:\n", total_read);
					DEBUG("%s\n", req->body);
					respond(&req, client_socket);
					continue;
				} else {
					DEBUG("No body to read, generating response\n");
					respond(&req, client_socket);
					continue;
				}
			}

			if (request_parse(req, line)) {
				WARN("An error occurred while parsing the previous line\n");
			}
		}
		DEBUG("Done with this request! n = %d\n", n);
	} while (1);

	close(server_socket);
	close(client_socket);

	return 0;
}

void respond(request **req, int client_socket) {
	response *resp = make_response(*req);
	response_write(resp, client_socket);
	response_clear(resp);

	request_clear(*req);
	*req = request_new();
}

response *make_response(request *req) {
	strlist *filedata;
	char *html, *path = req->path;
	
	response *resp = response_new();
	response_set_status_code(resp, 200);
	response_set_content_type(resp, "text/html");
	
	if (req->method == GET || req->method == POST) {
		INFO("Requested path %s\n", path);

		if (path[0] == '/')
			path = &path[1];

		filedata = readfile(path);
		if (!filedata) {
			WARN("Could not open file, generating 404...\n");
			return make_404_response();
		}
		html = strlist_to_string(filedata);
		
		response_set_body(resp, html);

		free(html);
	}
	return resp;
}

response *make_404_response() {
	char *path = "404.html", *html;
	response *resp = response_new();
	strlist *filedata = strlist_new();
	
	response_set_status_code(resp, 404);
	response_set_content_type(resp, "text/html");
	
	html = "<html><head><title>Not Found</title></head><body> \
<h1>404!</h1><br>Sorry, the object you requested was not found. \
</body><html>";
	
	
	filedata = readfile(path);
	if (filedata) {
		html = strlist_to_string(filedata);
	}
	
	//INFO("Read HTML:\n%s", html);
	
	response_set_body(resp, html);

	//INFO("Generated response:\n");
	//IFINFO(response_write(resp, STDOUT_FILENO));

	return resp;
}


strlist *readfile(char *filename) {
	int fd, n, i;
	char line[MAXLEN];
	strlist *data = strlist_new();
	strlist *ret = data;

	INFO("Opening file %s\n", filename);

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		ERROR("ERROR: could not open %s\n", filename);
		IFERROR(perror(""));
		return NULL;
	}
	
	i = 0; // TODO make sure this doesn't overflow line
	while ((n = read(fd, &line[i], 1)) > 0) {
		if (line[i] == '\n') {
			line[i+1] = '\0';
			data = strlist_add(data, line);
			i = 0;
			continue;
		}
		i++;
	}

	close(fd);

	return ret;
}
