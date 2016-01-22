#pragma once

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
