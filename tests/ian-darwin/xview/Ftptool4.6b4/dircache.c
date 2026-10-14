#include "ftptool.h"

#pragma ident   "@(#)dircache.c 1.3     93/05/26"

#ifdef USE_PROTOTYPES
int dircache_add(struct dircache *dcache, char *name, struct dirlist *dlist)
#else
int dircache_add(dcache, name, dlist)
struct dircache *dcache;
char	*name;
struct dirlist *dlist;
#endif
{
	struct dirlist_header *new_header;
	struct dirlist_header *last;

	if (dircache_size != 0 && dircache_size == dcache->ndirs) {
		last = dcache->last;
		if (dcache->ndirs == 1) {
			dcache->first = 0;
			dcache->last = 0;
		} else {
			dcache->last = dcache->last->prev;
			dcache->last->next = 0;
		}
		free_dirlist_header(last);
		dcache->ndirs--;
	}
	new_header = new_dirlist_header(name, dlist);
	if (new_header == 0)
		return (ENOMEM);
	if (dcache->ndirs == 0) {
		dcache->first = new_header;
		dcache->last = new_header;
		new_header->next = 0;
		new_header->prev = 0;
	} else {
		new_header->prev = 0;
		new_header->next = dcache->first;
		dcache->first->prev = new_header;
		dcache->first = new_header;
	}

	dcache->ndirs++;

	return (0);
}

#ifdef USE_PROTOTYPES
void dircache_delete(struct dircache *dcache, char *name)
#else
void dircache_delete(dcache, name)
struct dircache *dcache;
char	*name;
#endif
{
	struct dirlist_header *first;

	if (cache_lookup(dcache, name) == 0)
		return;
	/* now at top of list */
	first = dcache->first;
	if (dcache->ndirs == 1) {
		dcache->first = 0;
		dcache->last = 0;
	} else {
		dcache->first = first->next;
		dcache->first->prev = 0;
	}
	free_dirlist_header(first);

	dcache->ndirs--;

	return;
}

#ifdef USE_PROTOTYPES
void dircache_shrink(struct dircache *dcache, int newsize)
#else
void dircache_shrink(dcache, newsize)
struct dircache *dcache;
int	newsize;
#endif
{
	struct dirlist_header *last;

	if (newsize == 0)
		return;
	while (dcache->ndirs > newsize) {
		last = dcache->last;
		dcache->last = dcache->last->prev;
		dcache->last->next = 0;
		free_dirlist_header(last);
		dcache->ndirs--;
	}
}

#ifdef USE_PROTOTYPES
void free_dircache(struct dircache *dcache)
#else
void free_dircache(dcache)
struct dircache *dcache;
#endif
{
	struct dirlist_header *next;
	struct dirlist_header *tmp;

	for (tmp = dcache->first; tmp != 0; /* null */) {
		next = tmp->next;
		free_dirlist_header(tmp);
		tmp = next;
	}
	dcache->ndirs = 0;
	dcache->first = 0;
	dcache->last = 0;
}

#ifdef USE_PROTOTYPES
struct dirlist *cache_lookup(struct dircache *dcache, char *dirname)
#else
struct dirlist *cache_lookup(dcache, dirname)
struct dircache *dcache;
char	*dirname;
#endif
{
	struct dirlist_header *tmp;

	for (tmp = dcache->first; tmp != 0; tmp = tmp->next)
		if (!strcmp(tmp->name, dirname)) {
			/* match! move to top of cache */
			if (tmp == dcache->first) {
				/* nothing to do */
				return (tmp->dlist);
			} else if (tmp == dcache->last) {
				/* at least 2 */
				/* off list */
				dcache->last = dcache->last->prev;
				dcache->last->next = 0;
			} else {
				/* in the middle */
				/* off list */
				tmp->prev->next = tmp->next;
				tmp->next->prev = tmp->prev;
			}
			/* now add to front */
			tmp->next = dcache->first;
			tmp->prev = 0;
			dcache->first->prev = tmp;
			dcache->first = tmp;

			return (tmp->dlist);
		}
	/* no match */
	return (0);
}

#ifdef USE_PROTOTYPES
struct dirlist_header *new_dirlist_header(char *name, struct dirlist *dlist)
#else
struct dirlist_header *new_dirlist_header(name, dlist)
char	*name;
struct dirlist *dlist;
#endif
{
	struct dirlist_header *tmp;

	tmp = (struct dirlist_header *)malloc(sizeof (struct dirlist_header));
	if (tmp == 0)
		return (0);
	bzero((char *)tmp, sizeof (struct dirlist_header));
	tmp->name = strdup(name);
	if (tmp->name == 0) {
		free((char *)tmp);
		return (0);
	}
	tmp->next = 0;
	tmp->prev = 0;
	tmp->dlist = dlist;
	return (tmp);
}

#ifdef USE_PROTOTYPES
void free_dirlist_header(struct dirlist_header *head)
#else
void free_dirlist_header(head)
struct dirlist_header *head;
#endif
{
	free_dirlist(head->dlist);
	free(head->name);
	free((char *)head);
}

#ifdef notdef

#ifdef USE_PROTOTYPES
void show_dircache(struct dircache *dcache)
#else
void show_dircache(dcache)
struct dircache *dcache;
#endif
{
	struct dirlist_header *tmp;

	printf("cache contains\n");
	printf("--------------\n");
	for (tmp = dcache->first; tmp != 0; tmp = tmp->next) {
		printf("%s\n", tmp->name);
	}
	printf("--------------\n");
}

#endif
