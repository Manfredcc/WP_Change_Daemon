#ifndef _LIST_SELF_H_
#define _LIST_SELF_H_
#include <stddef.h>

/*=====================================================
            CIRCULAR DOUBLY LINKED LIST
Introduce:
    This .c file is coding with reference to list.h in 
linux kernel.

    When           Who              Description
    -----          ----             ------------
    2024/01/06     Manfred          First release


======================================================*/

#define LIST_HEAD_INIT(name)    { &(name), &(name) }

#define container_of(ptr, type, name)   \
    ((type *)((char *)(ptr) - offsetof(type, name)))

/* 获取包含该链表的结构体 */
#define list_entry(ptr, type, name) \
    container_of(ptr, type, name)    

/* 静态初始化 */
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

struct list_head {
    struct list_head *next, *prev;
};

/*
 * 动态初始化
 */
static inline void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}

/*
 * 在已知的两个元素中间插入元素
 */
static inline void _list_add(struct list_head *new, 
                struct list_head *prev, 
                struct list_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

/*
 * 在head后添加元素
 */
static inline void list_add(struct list_head *new, struct list_head *head)
{
    _list_add(new, head, head->next);
}

/*
 * 在head前添加元素
 */
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
    _list_add(new, head->prev, head);
}

/*
 * 通过让前后元素彼此建立新的指向来达到删除元素
 */
static inline void _list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

/*
 * 删除元素并清理指针
 */
static inline void list_del(struct list_head *entry)
{
    _list_del(entry->prev, entry->next);
    entry->prev = NULL;
    entry->next = NULL;
}

/*
 * 测试该表是否为空
 */
static inline int list_empty(const struct list_head *head)
{
    return head->next == head;
}

/*
 * 测试@list是否是该表的头结点
 */
static inline int list_is_head(const struct list_head *list, const struct list_head *head)
{
    return list == head;
}

/*
 * 遍历列表，可以同时删除元素
 * @pos:    用来遍历的指针
 * @n:      使用临时存储的另一个list_head结构
 * @head:   表头
 */
#define list_for_each_safe(pos, n, head)    \
    for (pos = (head)->next, n = pos->next; \
        !list_is_head(pos, (head)); \
        pos = n, n = pos->next)

/* 反向遍历 */
#define list_for_each_prev_safe(pos, n, head)   \
    for (pos = (head)->prev, n = pos->prev; \
        !list_is_head(pos, (head)); \
        pos = n, n = pos->prev)

/*
 * 获取结点个数
 */
static inline int list_length(struct list_head *head)
{
    int length = 0;

    struct list_head *pos, *n;
    list_for_each_safe(pos, n, head) {
        length++;
    }

    return length;
}

#endif /*  _LIST_SELF_H_*/