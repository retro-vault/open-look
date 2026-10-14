/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 */

#ifdef IDENT
#ident	"@(#)group.c	26.7	91/09/14 SMI"
#endif

#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "i18n.h"
#include "mem.h"
#include "st.h"
#include "olwm.h"
#include "win.h"
#include "list.h"
#include "group.h"

/***************************************************************************
* Local data
***************************************************************************/

static st_table *groupHashTable;

/***************************************************************************
* Local functions
***************************************************************************/

/* groupFindList -- find a client in a list of clients; return a given
 * value if found or 0 otherwise.  If passed a pointer to a List **, set that.
 */
static unsigned int
groupFindList(List ** plist, Client * cli, List *** retlist, unsigned int retval)
{
	if ((cli == 0) || (*plist == 0))
	{
		if (retlist != 0)
			*retlist = 0;
		return 0;
	}

	if ((*plist)->value == cli)
	{
		if (retlist != 0)
			*retlist = plist;
		return retval;
	}
	return groupFindList(&((*plist)->next), cli, retlist, retval);
}


/* groupFindCli -- find a client in a group.  Return a mask indicating
 * which category of client the client was found in, if any; if a list
 * pointer is passed in, return a pointer to a List * in that parameters
 * (presumably for list deletion).
 */
static unsigned int
groupFindCli(Group * group, Client * cli, List *** ppList)
{
    unsigned int retmask;

    if (cli == group->leader)
    {
        retmask = GROUP_LEADER;
    }
    else
    {
	if ((retmask = groupFindList(&(group->dependents), cli, ppList, 
	  GROUP_DEPENDENT)) == 0)
	    retmask = groupFindList(&(group->independents), cli, ppList, 
		GROUP_INDEPENDENT);
    }
    return retmask;
}

/* groupCompare -- utility function to compare two group id's
 */
static int
groupCompare(char * g1, char *g2)
{
	return ((GroupID)g1) - ((GroupID)g2);
}

/* groupHash -- hash function for group id lookup table 
 */
static int
groupHash(char * g1, int modulus)
{
	return ((GroupID)g1)%modulus;
}

/* groupInsert -- put a group structure into the lookup table
 */
static void
groupInsert(Group * group)
{
	st_insert(groupHashTable, (int)group->groupid, (char *)group);
}

/* groupDelete -- remove a group structure from the lookup table
 */
static Bool
groupDelete(GroupID grpid)
{
	Group *oldGrp;
	GroupID tmpGrp = grpid;
	GroupID *tmpGrpPtr = &tmpGrp;

	return st_delete(groupHashTable, (char *)tmpGrpPtr, (char *)&oldGrp);
}

/***************************************************************************
* Global functions
***************************************************************************/

/*
 * GroupInit -- initialises the hash table used to map group id's to
 * 	group structures.
 */
void 
GroupInit(void)
{
	groupHashTable = st_init_table(groupCompare, groupHash);
}


/* GroupApply -- applies a function to each part of the group identified 
 * in the mask; as with any apply function, when the function returns
 * a non-0 value application stops and that value is returned.
 */
void *
GroupApply(GroupID grpid, GroupFunc func, void * closure, unsigned int mask)
{
	Group *group;
	void *res = 0;

	group = GroupLookup(grpid);
	if (group == 0)
		return 0;

	if ((mask & GROUP_LEADER) && (group->leader != 0))
	{
		res = (func)(group->leader, closure);
	}

	if ((mask & GROUP_DEPENDENT) && (group->dependents != 0) && 
	    (res == 0))
	{
		res = ListApply(group->dependents,func,closure);
	}

	if ((mask & GROUP_INDEPENDENT) && (group->independents != 0) && 
	    (res == 0))
	{
		res = ListApply(group->independents,func,closure);
	}

	return res;
}

/* GroupLookup -- given a GroupID, returns the associated group structure, 
 * or 0 if there is none.
 */
Group *
GroupLookup(GroupID group)
{
	Group *tmp = 0;

	st_lookup(groupHashTable, group, &tmp);
	return tmp;
}

/* GroupAdd -- adds a client to the named group, creating the group if need be.
 * The kind of client is specified in the mask field.  Returns True
 * if the client was successfully added.
 */
Bool 
GroupAdd(GroupID grpid, Client * cli, unsigned int mask)
{
	Group *group;
	unsigned int currmask;

	group = GroupLookup(grpid);
	if (group == 0)
	{
		group = MemNew(Group);
		group->groupid = grpid;
		groupInsert(group);
		currmask = 0;
	}
	else
	{
		currmask = groupFindCli(group, cli, 0);
	}
	
	if (mask & GROUP_LEADER)
	{
		if ((currmask == 0) || (group->leader == cli))
		{
			group->leader = cli;
			return True;
		}
		else
			return False;
	}
	else if (mask & GROUP_DEPENDENT)
	{
		if (currmask != 0)
			return False;
		group->dependents = ListCons(cli,group->dependents);
	}
	else if (mask & GROUP_INDEPENDENT)
	{
		if (currmask != 0)
			return False;
		group->independents = ListCons(cli,group->independents);
	}
	return True;
}

/* GroupRemove -- removes a client from the group.  If the group becomes empty
 * it is deleted.  Returns True if the client was successfully deleted.
 */
Bool 
GroupRemove(GroupID grpid, Client * cli)
{
	Group *group;
	unsigned int mask;
	List **plist;

	group = GroupLookup(grpid);
	if (group == 0)
		return False;

	mask = groupFindCli(group, cli, &plist);
	if (mask == GROUP_LEADER)
	{
		group->leader = 0;
	}
	else if ((mask == GROUP_DEPENDENT) || (mask == GROUP_INDEPENDENT))
	{
		ListDestroyCell(plist);
	}
	else
	{
		return False;
	}
	if ((group->leader == 0) && (group->dependents == 0) && 
	    (group->independents == 0))
	{
		groupDelete(group->groupid);
		MemFree(group);
	}
	return True;
}


/* GroupIsLeader -- returns whether a particular client is leader of a group
 *	If the group hasn't been registered yet, assume that this client is 
 *	its leader.
 */
Bool
GroupIsLeader(GroupID grpid, Client * cli)
{
    Group *group;

    group = GroupLookup(grpid);
    if (group == 0)
        return True;
    return (group->leader == cli);
}


/* GroupLeader -- returns the leader of a group
 */
Client *
GroupLeader(GroupID grpid)
{
	Group *group;

	group = GroupLookup(grpid);
	if (group == 0)
		return 0;
	else
		return group->leader;
}
