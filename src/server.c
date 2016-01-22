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

#define SRVPORT 8080

#define BUFLEN 1024
#define MAXLEN 1024

void error(char* msg);
strlist *process_lines(strlist *head, char *buf);
char *respond(request *req);
strlist *readfile(char *filename);

int main(/*int argc, char *argv[]*/) {
	printf("started\n");
	int server_socket, client_socket, port, res, client_len, n;
	struct sockaddr_in srv_addr, client_addr;
	int iSetOption = 1;

	port = SRVPORT;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,
        sizeof(iSetOption));
        
	if (server_socket < 0) {
		error("error opening socket\n");
	}
	printf("made server socket\n");

	memset((char *) &srv_addr, 0, sizeof(srv_addr));
	
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(port);
	srv_addr.sin_addr.s_addr = INADDR_ANY;

	while ((res = bind(server_socket,
			   (struct sockaddr *) &srv_addr,
			   sizeof(srv_addr))) < 0) {
		error("error binding socket");
	}

	listen(server_socket, 5);
	printf("listening\n");

	client_len = sizeof(client_addr);

	if (chdir("content")) {
		error("Could not chdir to the content directory\n");
		exit(1);
	}

	do {
		client_socket = accept(server_socket,
							   (struct sockaddr *) &client_addr,
							   (socklen_t *) &client_len);

		printf("accepted client socket\n");

		if (client_socket < 0) {
			error("error on client accept\n");
		}

		request *req = request_new();
		char *response;
		char line[MAXLEN];
		int i = 0; // TODO check so it doesn't overflow line

		while ((n = read(client_socket, &line[i], 1)) > 0) {
			if (is_newline(line[i])) {
				if (i == 1 && is_crlf(line)) {
					// we've read an empty line, should be either the
					// end of the request or the start of the body
					
					if (request_get_content_length(req) <= 0) {
						printf("empty line and no Content-Length header read\n");

						request_print(req);
						response = respond(req);
						write(client_socket, response, strlen(response));

						i = 0;
						continue;
					}							
				}
				line[++i] = '\0';
				if (request_parse(req, line)) {
					// some error occurred while parsing
				}
				
				i = 0;
				continue;
			}
			i++;
		}
		
		if (n < 0) {
			perror("reading from socket failed");
			exit(1);
		}
	} while (1);
	
	close(server_socket);
	close(client_socket);

	return 0;
}

char *respond(request *req) {
	strlist *data, *iter;
	int len = 0;
	char *html, *ret, *path = req->path, *str;
	char *headers = "HTTP/1.1 200 OK\n\
Content-Type: text/html\n\
Content-Length: %d\n\n\n";
	size_t headerlen = strlen(headers);
	size_t htmllen;
	if (req->method == GET) {
		printf("requested path %s\n", path);

		if (path[0] == '/')
			path = &path[1];

		data = readfile(path);
		iter = data;
		while (iter) {
			len += iter->len;
			iter = iter->next;
		}
		
		html = malloc(sizeof(char)*(len+1));
		htmllen = (size_t)(len+1);

		iter = data;
		str = html;
		while (iter) {
			if (!iter->data) {
				iter = iter->next;
				continue;
			}
			strcpy(str, iter->data);
			str += iter->len;
			iter = iter->next;
		}
		
		ret = malloc(sizeof(char)*(headerlen+htmllen));
		sprintf(ret, headers, htmllen);
		strcpy(&ret[headerlen], html);
		ret[headerlen+htmllen] = '\0';
		
		free(html);
		printf("sending response:\n|%s|\n\n", ret);
	}
	return ret;
}

strlist *readfile(char *filename) {
	int fd, n;
	char c, *line, *line_start;
	strlist *data = strlist_new();
	strlist *ret = data;

	printf("opening file %s\n", filename);

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		printf("ERROR: could not open %s\n", filename);
		error("ERROR:");
		exit(1);
	}
	line = malloc(MAXLEN*sizeof(char));
	line_start = line;
	while ((n = read(fd, &c, 1)) > 0) {
		*line++ = c;
		if (c == '\n') {
			*line = '\0';
			data = strlist_add(data, line_start);
			line = line_start;
		}
	}

	printf("finished reading file %s:\n", filename);
	strlist_print(ret);

	free(line);

	close(fd);

	return ret;
}


void error(char *msg) {
	perror(msg);
}
