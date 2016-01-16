#pragma once

#include <string.h>


typedef struct strlist {
	char* data;
	int len;
	struct strlist *prev, *next;
} strlist;

strlist *strlist_new();
strlist *strlist_add(strlist *node, const char* str);
void strlist_clear(const strlist *list);
void strlist_del(strlist *node);
void strlist_print(const strlist *list);
void strlist_print_debug(const strlist *list);
void strlist_printr(const strlist *list);


