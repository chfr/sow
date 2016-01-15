#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "strlist.h"

node *list_add(node *head, char *str) {	
	head->next = NULL;
	head->data = str;
	head->len = strlen(str);

	head->next = (node *)malloc(sizeof(node));

	return head->next;
}

void list_clear(const node *head) {
	node *next;

	while (head) {
		next = head->next;
		free(head->data);
		free((node *)head);
		head = next;
	}
}

void print_list(node *head) {
	if (!head || (head && !head->len)) {
		printf("end of printing list\n");
		return;
	}
		
	printf("[%4d]\t%s", head->len, head->data);
	print_list(head->next);
}

