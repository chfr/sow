typedef struct node {
	struct node *next;
	char *data;
	int len;
} node;

node *list_add(node *head, char *str);
void list_del(node *head);
void print_list(node *head);
