#ifndef LLIST_TYPE
#error "The preprocessor macro LLIST_TYPE must be defined"
#endif

#ifndef LLIST_ASSERT_FUNC
#define LLIST_ASSERT_FUNC (void)
#endif

#ifndef LLIST_MALLOC_FUNC
#define LLIST_MALLOC_FUNC malloc
#endif

#ifndef LLIST_FREE_FUNC
#define LLIST_FREE_FUNC free
#endif

#define LLIST_PPCAT_NX(A, B) llist__ ## A ## _ ## B
#define LLIST_FUNC_NAME(A, B) LLIST_PPCAT_NX(A, B)

#ifndef LLIST_METHODS_DEFINED
#define LLIST_METHODS_DEFINED

#define llist_insert(T, head, index, value) llist__insert_##T ((head), (index), (value))
#define llist_append(T, head, value) llist__append_##T ((head), (value))
#define llist_prepend(T, head, value) llist__append_##T ((head), 0, (value))
#define llist_remove_at(T, head, index) llist__remove_at_##T ((head), (index))
#define llist_movenext(T, head, state, value) llist__movenext_##T ((head), (state), (value))

#endif /* LLIST_METHODS_DEFINED */

struct llist {
	LLIST_TYPE value;
	struct llist* next;
};

static int LLIST_FUNC_NAME(insert, LLIST_TYPE)(struct llist** head, size_t index, LLIST_TYPE value)
{
	LLIST_ASSERT_FUNC(head != NULL);

	struct llist* new = LLIST_MALLOC_FUNC(sizeof(struct llist));
	new->value = value;
	if (new == NULL)
		return 0;

	if (index == 0) {
		new->next = *head;
		*head = new;
		return 1;
	} else {
		size_t i = 0;
		struct llist* current = *head;
		while(i++ < index) {
			LLIST_ASSERT_FUNC(current != NULL);
			current = current->next;
		}
		LLIST_ASSERT_FUNC(current != NULL);
		new->next = current->next;
		current->next = new;
		return 1;
	}
}

static int LLIST_FUNC_NAME(append, LLIST_TYPE)(struct llist** head, LLIST_TYPE value)
{
	LLIST_ASSERT_FUNC(head != NULL);

	struct llist* new = LLIST_MALLOC_FUNC(sizeof(struct llist));
	new->next = NULL;
	new->value = value;
	if (new == NULL)
		return 0;

	if (*head) {
		struct llist* current = *head;
		while(current->next != NULL) {
			current = current->next;
		}
		current->next = new;
	} else {	
		*head = new;
		return 1;
	}
}

static LLIST_TYPE LLIST_FUNC_NAME(remove_at, LLIST_TYPE)(struct llist** head, size_t index)
{
	LLIST_ASSERT_FUNC(head != NULL);

	size_t i = 0;
	struct llist* current = *head;
	while(i++ < index) {
		LLIST_ASSERT_FUNC(current != NULL);
		current = current->next;
	}
	LLIST_ASSERT_FUNC(current != NULL);
	LLIST_ASSERT_FUNC(current->next != NULL);

	LLIST_TYPE value = current->next->value;

	struct llist* tmp = current->next->next;
	LLIST_FREE_FUNC(current->next);
	current->next = tmp;

	return value;
}

static int LLIST_FUNC_NAME(movenext, LLIST_TYPE)(const struct llist* head, void** state, LLIST_TYPE* value)
{
	LLIST_ASSERT_FUNC(head != NULL);
	LLIST_ASSERT_FUNC(state != NULL);
	LLIST_ASSERT_FUNC(value != NULL);

	if (*state) {
		struct llist* old = *state;
		if (!old->next)
			return 0;
		*value = old->next->value;
		*state = old->next;
		return 1;
	} else {
		if (head) {
			*value = head->value;
			*state = head;
			return 1;
		} else {
			return 0;
		}
	}
}

#undef LLIST_FUNC_NAME
#undef LLIST_PPCAT
#undef LLIST_PPCAT
