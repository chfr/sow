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

#define BUFLEN 1024
#define MAXLEN 1024

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
		response *resp;
		char line[MAXLEN];
		int i = 0; // TODO check so it doesn't overflow line

		while ((n = read(client_socket, &line[i], 1)) > 0) {
			if (is_newline(line[i])) {
				if (i == 1 && is_crlf(line)) {
					// we've read an empty line, should be either the
					// end of the request or the start of the body
					
					if (request_get_content_length(req) <= 0) {
						DEBUG("Empty line and no Content-Length header read\n");

						IFINFO(request_print(req));
						resp = make_response(req);
						response_write(resp, client_socket);
						response_clear(resp);

						request_clear(req);
						req = request_new();

						i = 0;
						continue;
					} else {
						DEBUG("We need to read %d more characters\n", request_get_content_length(req));
					}
				}
				line[++i] = '\0';
				//~ printf("%s\n", line);
				if (request_parse(req, line)) {
					// some error occurred while parsing
				}
				
				i = 0;
				continue;
			}
			i++;
		}
		
		if (n < 0) {
			ERROR("Reading from socket failed");
			IFERROR(perror(""));
			exit(1);
		}
	} while (1);
	
	close(server_socket);
	close(client_socket);

	return 0;
}

response *make_response(request *req) {
	strlist *filedata;
	char *html, *path = req->path;
	
	response *resp = response_new();
	response_set_status_code(resp, 200);
	response_set_content_type(resp, "text/html");
	
	if (req->method == GET) {
		INFO("Requested path %s\n", path);

		if (path[0] == '/')
			path = &path[1];

		filedata = readfile(path);
		if (!filedata) {
			WARN("Could not open file, generating 404...\n");
			return make_404_response();
		}
		html = strlist_to_string(filedata);
		//INFO("Read HTML:\n%s", html);
		
		response_set_body(resp, html);

		//IFINFO(INFO("Generated response:\n"));
		//IFINFO(response_write(resp, STDOUT_FILENO));

		//~ free(html);
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

	//IFINFO(INFO("Generated response:\n"));
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

	INFO("Finished reading file %s:\n", filename);
	//IFDEBUG(strlist_print(ret));

	close(fd);

	return ret;
}
