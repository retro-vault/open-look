/*
 *	(c) Copyright 1991 Sun Microsystems, Inc.  All rights reserved.
 *	See LEGAL_NOTICE file for terms and restrictions.
 */

#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)list.c 1.2 92/01/28";
#endif
#endif

#include <sspkg/list.h>
#include <memory.h>

Listnode	*
list_first(Listnode	* node)
{
	register Listnode	*tmp = node;
	if(!node)
		return (Listnode*)0;
	while (node = node->prev)
		tmp = node;
	return (tmp);
}


Listnode	*
list_last(Listnode	* node)
{
	register Listnode       *tmp = node;
	if(!node)
		return (Listnode*)0;
	while (node = node->next)
		tmp = node;
	return (tmp);
}


Listnode       *
list_unlink_node(node)
	Listnode       *node;
/*
 * Return either the previous or the next
 * node or 0 if this is the last node on the list.
 */
{
	Listnode       *tmp = 0;

	if (node == 0)
		return 0;

	if (node->prev) {
		node->prev->next = node->next;
		tmp = node->prev;
	}
	if (node->next) {
		node->next->prev = node->prev;
		tmp = node->next;
	}
	node->next = node->prev = 0;
	return (tmp);
}

Listnode       *
list_delete_node(Listnode       * node)
{
	Listnode       *tmp = 0;
	tmp = list_unlink_node(node);
	if(node)
		free(node);
	return (tmp);
}


void
list_destroy(Listnode       * node)
{
	/*
	 * destroy the entire list from any element in the list.
	 */
	while (node = list_delete_node(node));
}


Listnode       *
list_insert_after(node, new_element)
	Listnode       *node, *new_element;
/*
 * Insert a new list element after an existing list element.
 */
{
	if (node) {
		new_element->prev = node;
		new_element->next = node->next;
		node->next = new_element;
		if (new_element->next)
			new_element->next->prev = new_element;
	} else {
		new_element->next =
			new_element->prev = 0;
	}
	return (new_element);
}


Listnode       *
list_insert_before(node, new_element)
	Listnode       *node, *new_element;
/*
 * Insert a new list element before an existing list element.
 */
{
	if (node) {
		new_element->next = node;
		new_element->prev = node->prev;
		node->prev = new_element;
		if (new_element->prev)
			new_element->prev->next = new_element;
	} else {
		new_element->next =
			new_element->prev = 0;
	}
	return (new_element);
}


Listnode       *
list_dup(Listnode       * node, int size)
{
	Listnode       *newlist = 0;
	node = list_first(node);
	if (node)
		do {
			Listnode       *tmp = (Listnode *) malloc(size);
			/* duplicate element then insert after new list's end */

			/*bcopy(node, tmp, size);*/
			memcpy((char*)tmp, (char*)node, size);
			list_insert_after(newlist, tmp);
			newlist = tmp;

		} while (node = list_next(node));
	return (list_first(newlist));
}


Listnode       *
list_concat(list1, list2)
	Listnode       *list1, *list2;
/*
 * Concatenate two lists by linking the end of list1 to the beginning of
 * list2.
 */
{
	register Listnode       *list1end, *list2start;
	if (list1 && list2) {
		list1end = list_last(list1);
		list2start = list_first(list2);
		list1end->next = list2start;
		list2start->prev = list1end;
	}
	return (list1 ? list_first(list1) : list_first(list2));
}

Listnode       *
list_find(Listnode       * node, void           * handle)
{
	list_for(node)
		if (node->handle == handle)
		return node;
	return (0);
}


/*
 * should use varargs here...
 */
void
list_traverse(node, function, call_data)
	Listnode       *node;
	int             (*function) ();
	void           *call_data;
{
	list_for(node) {
		(*function) (node, call_data);
	}
}
