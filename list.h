#ifndef __LIST_H
#define __LIST_H
#include<string.h>

struct list_head{
    struct list_head *next,*prev;
};
#define LIST_HEAD_INIT(name) {&(name), &(name)}

#define LIST_HEAD(name) \
        struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head* list)
{
    list->next = list;
    list->prev = list;
}

static inline void __list_add(struct list_head* new,
                              struct list_head* prev,
                              struct list_head* next)
{
    next->prev = new;
    new->next = next;
    prev->next = new;
    new->prev = prev;
}

static inline void list_add(struct list_head* new, struct list_head* head)
{
    __list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head* new, struct list_head* head)
{
    __list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head* prev, struct list_head* next)
{
    next->prev = prev;
    prev->next = next;
}
static inline void __list_del_entry(struct list_head* entry)
{
    __list_del(entry->prev, entry->next);
}

static inline void list_del(struct list_head* entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = NULL;
    entry->prev = NULL;
}

static inline void list_replace(struct list_head* old,
                         struct list_head* new)
{
    new->next = old->next;
    new->prev = old->prev;
    old->prev->next = new;
    old->next->prev = new;
}
static inline void list_replace_init(struct list_head* old,
                              struct list_head* new)
{
    list_replace(old, new);
    INIT_LIST_HEAD(old);
}

static inline void list_del_init(struct list_head* entry)
{
    __list_del_entry(entry);
    INIT_LIST_HEAD(entry);
}

static inline void list_move(struct list_head* list, struct list_head* head)
{
    __list_del_entry(list);
    list_add(list, head);
}

static inline void list_move_tail(struct list_head* list, struct list_head* head)
{
    __list_del_entry(list);
    list_add_tail(list, head);
}
static inline int list_is_last(const struct list_head* list,
                        const struct list_head* head)
{
    return list->next == head;
}
static inline int list_empty(const struct list_head* head)
{
    return head->next == head;
}

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({			\
	const typeof(((type *)0)->member) * __mptr = (ptr);	\
	(type *)((char *)__mptr - offsetof(type, member)); })

#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)


#define list_for_each(pos, head) \
        for ( pos = (head)->next;pos != (head); pos = pos->next)

#define __list_for_each(pos, head) \
        for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_prev(pos, head) \
        for (pos = (head)->prev; pos != (head); pos = pos->prev)


#define list_for_each_entry(pos, head, member) \
        for (pos = list_entry((head)->next, typeof(*pos), member); \
            &pos->member != (head); \
            pos = list_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_reverse(pos, head, member) \
        for (pos = list_entry((head)->prev, typeof(*pos), member); \
             &pos->member != (head); \
             pos = list_entry(pos->member.prev, typeof(*pos), member))

#define list_for_each_entry_safe(pos, n, head, member) \
        for (pos = list_entry((head)->next, typeof(*pos), member), \
                n = list_entry(pos->member.next,typeof(*pos), member); \
            &pos->member != (head); \
            pos = n,n = list_entry(n->member.next, typeof(*n), member) )


#endif