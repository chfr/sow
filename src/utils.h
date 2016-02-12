#pragma once

#include <stdio.h>
#include <unistd.h>

#define DEBUG_LEVEL_NONE 0
#define DEBUG_LEVEL_ERROR 1
#define DEBUG_LEVEL_WARN 2
#define DEBUG_LEVEL_INFO 3
#define DEBUG_LEVEL_DEBUG 4

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_LEVEL_WARN
#endif

 // http://stackoverflow.com/a/1644898
#define ERROR(...) \
        do { if (DEBUG_LEVEL >= DEBUG_LEVEL_ERROR) fprintf(stderr, __VA_ARGS__); } while (0)

#define WARN(...) \
        do { if (DEBUG_LEVEL >= DEBUG_LEVEL_WARN) fprintf(stderr, __VA_ARGS__); } while (0)

#define INFO(...) \
        do { if (DEBUG_LEVEL >= DEBUG_LEVEL_INFO) fprintf(stdout, __VA_ARGS__); } while (0)

#define DEBUG(...) \
        do { if (DEBUG_LEVEL >= DEBUG_LEVEL_DEBUG) fprintf(stdout, __VA_ARGS__); } while (0)

#define IFERROR(cmd) if (DEBUG_LEVEL >= DEBUG_LEVEL_ERROR) {cmd;}
#define IFWARN(cmd) if (DEBUG_LEVEL >= DEBUG_LEVEL_WARN) {cmd;}
#define IFINFO(cmd) if (DEBUG_LEVEL >= DEBUG_LEVEL_INFO) {cmd;}
#define IFDEBUG(cmd) if (DEBUG_LEVEL >= DEBUG_LEVEL_DEBUG) {cmd;}

int fd_readline(int sockfd, char *buffer, size_t socklen);

int is_newline(char c);
int is_crlf(char *line);
int is_null(char c);
int is_whitespace(char c);
int is_number(char c);
int is_printable(char c);
int is_alpha(char c);
int is_alnum(char c);
char *skip_whitespace(char *c);
char *word_end(char *c);
int newline_ending(char *c);
