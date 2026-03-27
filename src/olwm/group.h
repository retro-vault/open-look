/*
 * group.h: declarations and shared types for the group module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef _OLWM_GROUP_H
#define _OLWM_GROUP_H

typedef Window GroupID;
typedef void *(*GroupFunc)(/* Client *, void * */);

typedef struct _group {
	GroupID groupid;
	Client *leader;
	List *dependents; 	/* List of Client * */
	List *independents; 	/* List of Client * */
	} Group;

#define GROUP_LEADER 0x01
#define GROUP_DEPENDENT 0x02
#define GROUP_INDEPENDENT 0x04

/* GroupInit: Initialize group. */
extern void GroupInit();
	/* initialises the Group package; should be called only once 
	 * at startup.
	 */

/* GroupApply: Execute group. */
extern void *GroupApply();	/* GroupID, GroupFunc, void *, mask */
	/* applies a function to each part of the group identified in the
	 * mask; as with any apply function, when the function returns
	 * a non-NULL value application stops and that value is returned.
	 */

/* GroupLookup: Return group. */
extern Group *GroupLookup();	/* GroupID */
	/* given a GroupID, returns the associated group structure, or NULL
	 * if there is none.
	 */

/* GroupAdd: Add group. */
extern Bool GroupAdd();		/* GroupID, Client *, mask */
	/* adds a client to the named group, creating the group if need be.
	 * The kind of client is specified in the mask field.  Returns True
	 * if the client was successfully added.
	 */

/* GroupRemove: Remove group. */
extern Bool GroupRemove();	/* GroupID, Client * */
	/* removes a client from the group.  If the group becomes empty
	 * it is deleted.  Returns True if the client was successfully deleted.
	 */

/* GroupIsLeader: Process group is leader. */
extern Bool GroupIsLeader();	/* GroupID, Client * */
	/* returns whether a particular client is the leader of
	 * a group 
	 */

/* GroupLeader: Process group leader. */
extern Client *GroupLeader();	/* GroupID */
	/* returns the leader of a group */

#endif /* _OLWM_GROUP_H */
