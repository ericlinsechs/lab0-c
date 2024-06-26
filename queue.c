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
    if (!head || list_empty(head))
        return false;

    struct list_head *f = head->next, *b = head->prev;

    /*
     * Find the middle node by taking step in each iteration from both side
     */
    while (f != b && f->next != b) {
        f = f->next;
        b = b->prev;
    }

    // Delete the node from list
    list_del_init(b);

    // Free element
    element_t *ele = list_entry(b, element_t, list);
    free(ele->value);
    free(ele);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    struct list_head *pos, *n;
    bool flag = false;

    list_for_each_safe (pos, n, head) {
        element_t *pos_ele = list_entry(pos, element_t, list);
        char *n_val = list_entry(n, element_t, list)->value;
        if (pos->next != head && strcmp(pos_ele->value, n_val) == 0) {
            /*
             * Found duplicates node, delete current node.
             */
            flag = true;
            list_del(&pos_ele->list);
            q_release_element(pos_ele);
        } else if (flag) {
            /*
             * Delete the last duplicates node.
             */
            flag = false;
            list_del(&pos_ele->list);
            q_release_element(pos_ele);
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *pos, *n;
    bool odd = false;
    list_for_each_safe (pos, n, head) {
        if (odd)
            list_move(pos, pos->prev->prev);
        odd = !odd;
    }
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
    if (!head || list_empty(head) || k <= 1)
        return;

    struct list_head *pos, *n, *cut = head, tmp;
    int cnt = 0;

    list_for_each_safe (pos, n, head) {
        cnt++;
        if (cnt == k) {
            list_cut_position(&tmp, cut->next, pos);
            q_reverse(&tmp);
            list_splice(&tmp, cut);
            cut = n->prev;
            cnt = 0;
        }
    }
}

static void merge_two_lists(struct list_head *l1,
                            struct list_head *l2,
                            bool descend)
{
    if (!l1 || !l2)
        return;

    struct list_head head;
    INIT_LIST_HEAD(&head);

    while (!list_empty(l1) && !list_empty(l2)) {
        element_t *el = list_first_entry(l1, element_t, list);
        element_t *er = list_first_entry(l2, element_t, list);
        struct list_head *node = (descend ^ (strcmp(el->value, er->value) < 0))
                                     ? l1->next
                                     : l2->next;
        list_move_tail(node, &head);
    }
    list_splice_tail_init(list_empty(l1) ? l2 : l1, &head);
    list_splice(&head, l1);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *slow = head, *fast = head->next;
    for (; fast != head && fast->next != head; fast = fast->next->next)
        slow = slow->next;

    struct list_head left;

    // Divide list from the middle
    list_cut_position(&left, head, slow);

    q_sort(&left, descend);
    q_sort(head, descend);

    // Merge divided lists
    merge_two_lists(head, &left, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;

iterate:
    element_t *first, *sec;
    bool again = false;

    list_for_each_entry_safe (first, sec, head, list) {
        if (&sec->list == head)
            break;

        if (strcmp(first->value, sec->value) >= 0) {
            list_del(&first->list);
            q_release_element(first);
            again = true;
        }
    }

    if (again)
        goto iterate;

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;

iterate:
    element_t *first, *sec;
    bool again = false;

    list_for_each_entry_safe (first, sec, head, list) {
        if (&sec->list == head)
            break;

        if (strcmp(first->value, sec->value) <= 0) {
            list_del(&first->list);
            q_release_element(first);
            again = true;
        }
    }

    if (again)
        goto iterate;

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    else if (list_is_singular(head))
        return q_size(list_first_entry(head, queue_contex_t, chain)->q);

    struct list_head first;
    INIT_LIST_HEAD(&first);

    struct list_head *node;

    list_for_each (node, head)
        merge_two_lists(&first, list_entry(node, queue_contex_t, chain)->q,
                        descend);

    int size = q_size(&first);

    list_splice_tail(&first, list_first_entry(head, queue_contex_t, chain)->q);

    return size;
}
