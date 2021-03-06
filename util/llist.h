#ifndef LLIST_TYPE
#error "The preprocessor macro LLIST_TYPE must be defined"
#endif

#ifndef LLIST_MNAME
#define LLIST_MNAME_UNDEFINED
#define LLIST_MNAME LLIST_TYPE
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
#define llist_prepend(T, head, value) llist__append_##T ((head), (value))
#define llist_remove_at(T, head, index) llist__remove_at_##T ((head), (index))
#define llist_remove(T, head, value) llist__remove_##T ((head), (value))
#define llist_movenext(T, head, state, value) llist__movenext_##T ((head), (state), (value))
#define llist_count(T, head) llist__count_##T ((head))
#define llist(T) LLIST_PPCAT_NX(struct, T)

#endif /* LLIST_METHODS_DEFINED */

struct llist(LLIST_MNAME) {
	LLIST_TYPE value;
	struct llist(LLIST_MNAME)* next;
};

static int LLIST_FUNC_NAME(insert, LLIST_MNAME)(struct llist(LLIST_MNAME)** head, size_t index, LLIST_TYPE value)
{
	LLIST_ASSERT_FUNC(head != NULL);

	struct llist(LLIST_MNAME)* new = LLIST_MALLOC_FUNC(sizeof(struct llist(LLIST_MNAME)));
	new->value = value;
	if (new == NULL)
		return 0;

	if (index == 0) {
		new->next = *head;
		*head = new;
		return 1;
	} else {
		size_t i = 0;
		struct llist(LLIST_MNAME)* current = *head;
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

static int LLIST_FUNC_NAME(append, LLIST_MNAME)(struct llist(LLIST_MNAME)** head, LLIST_TYPE value)
{
	LLIST_ASSERT_FUNC(head != NULL);

	struct llist(LLIST_MNAME)* new = LLIST_MALLOC_FUNC(sizeof(struct llist(LLIST_MNAME)));
	new->next = NULL;
	new->value = value;
	if (new == NULL)
		return 0;

	if (*head) {
		struct llist(LLIST_MNAME)* current = *head;
		while(current->next != NULL) {
			current = current->next;
		}
		current->next = new;
	} else {	
		*head = new;
	}
	return 1;
}

static LLIST_TYPE LLIST_FUNC_NAME(remove_at, LLIST_MNAME)(struct llist(LLIST_MNAME)** head, size_t index)
{
	LLIST_ASSERT_FUNC(head != NULL);

	size_t i = 0;
	struct llist(LLIST_MNAME)* current = *head;
	while(i++ < index) {
		LLIST_ASSERT_FUNC(current != NULL);
		current = current->next;
	}
	LLIST_ASSERT_FUNC(current != NULL);
	LLIST_ASSERT_FUNC(current->next != NULL);

	LLIST_TYPE value = current->next->value;

	struct llist(LLIST_MNAME)* tmp = current->next->next;
	LLIST_FREE_FUNC(current->next);
	current->next = tmp;

	return value;
}

static int LLIST_FUNC_NAME(remove, LLIST_MNAME)(struct llist(LLIST_MNAME)** head, LLIST_TYPE value)
{
	LLIST_ASSERT_FUNC(head != NULL);

	struct llist(LLIST_MNAME)* current = *head;
	struct llist(LLIST_MNAME)* prev = NULL;
	while(current != NULL) {
		if (current->value == value) {
			if (prev != NULL)
				prev->next = current->next;
			else
				*head = current->next;
			LLIST_FREE_FUNC(current);
			return 1;
		}
		prev = current;
		current = current->next;
	}
	return 0;
}

static int LLIST_FUNC_NAME(movenext, LLIST_MNAME)(struct llist(LLIST_MNAME)* head, void** state, LLIST_TYPE* value)
{
	LLIST_ASSERT_FUNC(head != NULL);
	LLIST_ASSERT_FUNC(state != NULL);
	LLIST_ASSERT_FUNC(value != NULL);

	if (*state) {
		const struct llist(LLIST_MNAME)* old = *state;
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

static size_t LLIST_FUNC_NAME(count, LLIST_MNAME)(struct llist(LLIST_MNAME)* head)
{
	struct llist(LLIST_MNAME)* current = head;
	size_t c = 0;
	while(current != NULL) {
		c++;
		current = current->next;
	}
	return c;
}

static size_t LLIST_FUNC_NAME(clear, LLIST_MNAME)(struct llist(LLIST_MNAME)** head)
{
	struct llist(LLIST_MNAME)* current = head;
	struct llist(LLIST_MNAME)* prev = NULL;
	size_t c = 0;
	while(current != NULL) {
		c++;
		prev = current;
		current = current->next;
		LLIST_FREE_FUNC(prev);
	}
	head = NULL;
	return c;
}

#undef LLIST_FUNC_NAME
#undef LLIST_PPCAT
#undef LLIST_PPCAT

#ifdef LLIST_MNAME_UNDEFINED
#undef LLIST_MNAME_UNDEFINED
#undef LLIST_MNAME
#endif /* LLIST_MNAME_UNDEFINED */
