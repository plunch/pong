#ifndef UTIL_PTRLLIST_H
#define UTIL_PTRLLIST_H

#include <stdlib.h>

struct llist__struct_voidptrlist;
typedef struct llist__struct_voidptrlist pllist;

int pllist_insert(pllist** head, size_t index, void* value);

int pllist_append(pllist** head, void* value);

int pllist_prepend(pllist** head, void* value);

void* pllist_remove_at(pllist** head, size_t index);

int pllist_movenext(pllist* head, void** state, void** value);

size_t pllist_count(pllist* head);

#endif /* UTIL_PTRLLIST_H */
