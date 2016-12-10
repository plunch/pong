#include "ptrllist.h"

#include <assert.h>

#define LLIST_TYPE void*
#define LLIST_MNAME voidptrlist
#define LLIST_ASSERT_FUNC assert
#define LLIST_MALLOC_FUNC malloc
#define LLIST_FREE_FUNC free
#include "llist.h"
#undef LLIST_FREE_FUNC
#undef LLIST_MALLOC_FUNC
#undef LLIST_ASSERT_FUNC
#undef LLIST_MNAME
#undef LLIST_TYPE

int pllist_insert(pllist** head, size_t index, void* value)
{
	return llist_insert(voidptrlist, head, index, value);
}

int pllist_append(pllist** head, void* value)
{
	return llist_append(voidptrlist, head, value);
}

int pllist_prepend(pllist** head, void* value)
{
	return llist_prepend(voidptrlist, head, value);
}

void* pllist_remove_at(pllist** head, size_t index)
{
	return llist_remove_at(voidptrlist, head, index);
}

int pllist_remove(pllist** head, void* value)
{
	return llist_remove(voidptrlist, head, value);
}

int pllist_movenext(pllist* head, void** state, void** value)
{
	return llist_movenext(voidptrlist, head, state, value);
}

size_t pllist_count(pllist* head)
{
	return llist_count(voidptrlist, head);
}

size_t pllist_clear(pllist** head)
{
	return llist_count(voidptrlist, head);
}

