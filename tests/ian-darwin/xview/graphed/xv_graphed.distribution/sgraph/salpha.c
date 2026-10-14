/* (C) Universitaet Passau 1986-1991 */
/************************************************************************
 * File    : salpha.c                                       Version 1.0 *
 * Aufgabe : Hilfsfunktionen zum Zugriff auf die Alphabete		*
 *           						                *
 * Autor   : Torsten Bachmann						*
 * Datum   : 19.11.90							*
 ************************************************************************/


#include "std.h"
#include "slist.h"
#include "sgraph.h"
#include "sgragra.h"


Slist	S_alphabet_append(alphabet, element)
Slist	*alphabet;
char	*element;
	/*
	Creates a strdup'ed entry for element in the Slist alphabet. If the
	element is existing, you get a pointer to it (no new label is malloced)
	This function is similar to add_to_slist, but it works on 
	strings, while add_to_slist works on attibutes and pointers.
	*/
{
	Slist l = S_alphabet_test (*alphabet, element);
	if (l==0)
	{
		*alphabet = add_immediately_to_slist( *alphabet,
			make_attr(ATTR_DATA, iif (element != 0, strdup(element), strdup(""))));
		l = S_alphabet_test (*alphabet, element);
	}
	return l;
}


Slist	S_alphabet_test  (alphabet, element)
Slist	alphabet;
char	*element;
	/*
	Tests, whether element exists in alphabet. If so, a pointer
	to its Slist-pendant is the result. Otherwise 0.
	*/
{	
	Slist l;
	for_slist(alphabet, l)
	{
		if (element == 0) {
			if (attr_data(l) == 0) {
				return l;
			} else {
				/* continue */
			}
		} else if (strcmp(attr_data(l), element) == 0) {
			return l;
		}
	} end_for_slist(alphabet, l);
	return 0;
}


void	S_alphabet_remove (alphabet)
Slist	alphabet;
	/*
	Free's memory which is used by the alphabet. This function
	will also free mallocated memory for the labels.
	*/
	{
		Slist l;
		for_slist(alphabet, l)
		{
			myfree(attr_data(l));
		} end_for_slist(alphabet, l);
		free_slist(alphabet);
	}

