#ifndef MM_DLLIST_H
#define MM_DLLIST_H 

#include <stddef.h> 

typedef struct __MMDLList MMDLList;

struct __MMDLList {
    MMDLList *prev;
    MMDLList *next;
};

#define MMDLList_getNext(list) ((MMDLList*)list)->next 
#define MMDLList_setNext(list,val) MMDLList_getNext(list) = (MMDLList*)(val)
#define MMDLList_getPrev(list) ((MMDLList*)list)->prev
#define MMDLList_setPrev(list,val) MMDLList_getPrev(list) = (MMDLList*)(val)

/* Insert who after whom and update pointers of who to point to previous
 * pointers of whom. */
static inline MMDLList *MMDLList_insertAfter(MMDLList *whom, MMDLList *who)
{
    if (!whom) {
        return NULL;
    }
    if (who) {
        who->next = whom->next;
        who->prev = whom;
        if (whom->next) {
            whom->next->prev = who;
        }
    }
    whom->next = who;
    return whom;
}

/* Insert who before whom and update pointers of who to point to previous
 * pointers of whom. Returns whom. */
static inline MMDLList *MMDLList_insertBefore(MMDLList *whom, MMDLList *who)
{
    if (!whom) {
        return NULL;
    }
    if (who) {
        who->next = whom;
        who->prev = whom->prev;
        if (whom->prev) {
            whom->prev->next = who;
        }
    }
    whom->prev = who;
    return whom;
}

/* Inserts who before whom and returns who. If whom is the head of a list, who
 * becomes the new head and this returns it. */
static inline MMDLList *MMDLList_pushBefore(MMDlList *whom, MMDLList *who)
{
    (void) MMDLList_insertBefore(whom,who);
    return who;
}

/* remove the item at address of who and return the address to it. The pointers
 * of who are still as they were before the removal (they still point to its
 * previous "prev" and "next" nodes. */
static inline MMDLList *MMDLList_remove(MMDLList *who)
{
    if (!who) {
        return NULL;
    }
    if (who->prev) {
        who->prev->next = who->next;
    }
    if (who->next) {
        who->next->prev = who->prev;
    }
    return who;
}

/* Insert who after whose's last node. */
static inline void MMDLList_addAfterTail(MMDLList *whose, MMDLList *who)
{
    while (whose->next) {
        whose = whose->next;
    }
    MMDLList_insertAfter(whose,who);
}
/* Insert who before whose's first node. */
static inline void MMDLList_addBeforeHead(MMDLList *whose, MMDLList *who)
{
    while (whose->prev) {
        whose = whose->prev;
    }
    MMDLList_insertBefore(whose,who);
}

/* Get the last item in the list */
static inline MMDLList *MMDLList_getTail(MMDLList *who)
{
    while (who->next) {
        who = who->next;
    }
    return who;
}

/* Get the length of the list */
static size_t MMDLList_getLength(MMDLList *who)
{
    size_t result = 0;
    while (who) {
        result++;
        who = who->next;
    }
    return result;
}

#define MMDLList_init(who) \
    ((MMDLList*)who)->next = NULL; \
    ((MMDLList*)who)->prev = NULL;

#endif /* MM_DLLIST_H */
