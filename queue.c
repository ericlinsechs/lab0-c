#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an element and initialize it */
__attribute__((nonnull)) static element_t *create_element(char *s)
{
    element_t *new_ele = (element_t *) malloc(sizeof(*new_ele));
    if (!new_ele)
        return NULL;

    new_ele->value = malloc(sizeof(char) * (strlen(s) + 1));
    if (!new_ele->value) {
        free(new_ele);
        return NULL;
    }

    strncpy(new_ele->value, s, strlen(s) + 1);

    return new_ele;
}

__attribute__((nonnull(1))) static inline void remove_element(element_t *ele,
                                                              char *sp,
                                                              size_t bufsize)
{
    if (sp) {
        size_t len = strlen(ele->value) < (bufsize - 1) ? strlen(ele->value)
                                                        : (bufsize - 1);
        memcpy(sp, ele->value, len);
        sp[len] = '\0';
    }

    list_del_init(&ele->list);
}

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = (struct list_head *) malloc(sizeof(*head));

    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);

    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *entry, *safe;

    list_for_each_entry_safe (entry, safe, head, list) {
        list_del(&entry->list);
        free(entry->value);
        free(entry);
    }

    free(head);
}


/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_ele = create_element(s);
    if (!new_ele)
        return false;

    list_add(&new_ele->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_ele = create_element(s);
    if (!new_ele)
        return false;

    list_add_tail(&new_ele->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *node = head->next;
    element_t *ele = list_entry(node, element_t, list);

    remove_element(ele, sp, bufsize);

    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *node = head->prev;
    element_t *ele = list_entry(node, element_t, list);

    remove_element(ele, sp, bufsize);

    return ele;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    element_t *entry;
    int s = 0;

    list_for_each_entry (entry, head, list)
        s++;

    return s;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node, *tmp;

    list_for_each_safe (node, tmp, head) {
        node->next = node->prev;
        node->prev = tmp;
    }

    node->next = node->prev;
    node->prev = tmp;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
