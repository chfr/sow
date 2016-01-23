#include <stdlib.h>
#include <stdio.h>

#include "strlist.h"
#include "utils.h"

strlist *strlist_new() {
	strlist *ret = (strlist *)malloc(sizeof(strlist));
	ret->next = ret->prev = NULL;
	ret->data = NULL;
	ret->len = -1;
	DEBUG("Created new strlist at %p\n", (void *)ret);
	return ret;
}

// if node->data is NULL, this node is empty, and we add the
// data to this node. if data is not NULL, but node->next is,
// we create a new node and add the data there.
strlist *strlist_add(strlist *node, const char* str) {
	strlist *new;
	
	if (node->data && !node->next) {
		new = malloc(sizeof(strlist));
		node->next = new;
		new->prev = node;
		new->next = NULL;
	} else {
		new = node;
	}
	
	new->data = malloc(sizeof(char)*strlen(str));
	strcpy(new->data, str);
	new->len = strlen(str);

	return new;
}

void strlist_clear(const strlist *list) {
	strlist *next;

	while (list) {
		next = list->next;
		free(list->data);
		free((strlist *)list);
		list = next;
	}
}

void strlist_del(strlist *node) {
	if (!node)
		return;

	if (!node->prev && node->next) {
		ERROR("ERROR: Can't delete the head of a list while it has children!\n");
		exit(1);
	}

	if (node->next)
		node->next->prev = node->prev;

	if (node->prev)
		node->prev->next = node->next;

	if (node->data)
		free(node->data);
	free((strlist *)node);
}

void strlist_print(const strlist *list) {
	int n = 1;
	printf("Printing strlist starting at %p\n", (void *)list);

	while (list) {
		printf("[%3.d][%3.d] %s", n++, list->len, list->data);
		list = list->next;
	}
}


void strlist_print_debug(const strlist *list) {
	int n = 1;
	printf("Printing strlist starting at %p", (void *)list);

	while (list) {
		printf("[%3.d][%3.d][%p][%p][%p] %s\n", n++, list->len,
			(void *)list->prev, (void *)list, (void *)list->next,
			list->data);
		list = list->next;
	}
}

void strlist_printr(const strlist *list) {
	int n = 1;
	printf("Printing strlist in reverse starting at %p\n", (void *)list);

	while (list) {
		printf("[%3.d][%3.d][%p][%p][%p] %s", n++, list->len,
			(void *)list->prev, (void *)list, (void *)list->next,
			list->data);
		list = list->prev;
	}
}

int strlist_total_length(const strlist *list) {
	int ret = 0;

	while (list) {
		if (list->len >= 0)
			ret += list->len;
		list = list->next;
	}

	return ret;
}

char *strlist_to_string(const strlist *list) {
	if (!list)
		return NULL;

	int total_len = strlist_total_length(list);
	char *str = malloc(sizeof(char)*total_len + 1);
	char *ret = str;


	while (list) {
		strcpy(str, list->data);
		str += list->len;
		list = list->next;
	}

	return ret;
}
