#include<stdio.h>
#include<stdlib.h>
#include"list.h"
struct numlist{
    int num;
    struct list_head list;
};
void createList(struct list_head* head)
{
    int i = 0;
    struct numlist* newNode = NULL;
    struct list_head* tmp = head;
    for(i = 0; i < 10; i++) {
        newNode = (struct numlist*)malloc(sizeof(struct numlist));
        newNode->num = i;
        list_add(&newNode->list, tmp);
        tmp = tmp->next;
    }
}
void printList(struct list_head* head)
{
    struct numlist* pos = NULL;
    list_for_each_entry(pos, head, list) {
        printf("%d ",pos->num);
    }
    printf("\n");
}
int main()
{
    struct numlist head;
    INIT_LIST_HEAD(&head.list);
    createList(&head.list);
    printList(&head.list);
    return 0;
}