#include "utils.h"

int fd_readline(int sockfd, char *buffer, size_t socklen) {
	char c, *buf_start = buffer;
	int n, num_read = 0;
	
	while ((n = read(sockfd, &c, 1)) > 0) {
		socklen--;
		num_read++;
		*buffer++ = c;
		
		if (socklen == 1) { // out of buffer space
			break;
		}

		if (c == '\n') { // end of line
			break;
		}
	}

	*buffer = '\0';
	return buffer - buf_start;
}


int is_newline(char c) {
	return c == '\n';
}

int is_crlf(char *line) {
	if (!(line && (line+1)))
		return 0;

	return *line == '\r' && *(line+1) == '\n';
}

int is_null(char c) {
	return c == '\0';
}

int is_whitespace(char c) {
	return c == ' ' || c == '\t';
}

int is_number(char c) {
	return c >= '0' && c <= '9';
}

int is_printable(char c) {
	return c >= 32 && c <= 126;
}

int is_alpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int is_alnum(char c) {
	return is_alpha(c) || is_number(c);
}

char *skip_whitespace(char *c) {
	while (is_whitespace(*c))
		c++;

	return c;
}

char *word_end(char *c) {
	while (!is_whitespace(*c))
		c++;

	return c;
}

int newline_ending(char *c) {
	if (!c) return 0;
	
	c++;
	while (c) {
		if (is_null(*c) && is_newline(*(c-1)))
			return 1;
		c++;
	}
	
	return 0;
}


