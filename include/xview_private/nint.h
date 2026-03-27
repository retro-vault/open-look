/*
 * Declares private XView interfaces, types, and macros for nint.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef	NINT_DEFINED
#define	NINT_DEFINED

/*
The interposition stack is independent of the detector and dispatcher.
Interposition stuff is prefaced with nint_.  Interposition is only viable
with static conditions.  The number of interpositions is limited
to the max size of the fast allocaction node (about 6 levels deep).
The truth of the function list (interposition list) is kept by the
detector.

Keeping track of which function to call when running down the
interposition list is done by maintaining an "interposition stack".
This is a stack of conditions that currently have callouts outstanding.

Before an initial callout is made, a copy of the condition is pushed
onto the top of the stack.  The first callout is made and the func_next
index is set to 1.

When a notify_next_*_func call is made, the top of the interposition stack
is supposed to be the relevent condition.  Some consistency checks are done
to check this.  The next function for that condition is called.

When the initial callout returns, the condition is popped from the
interposition stack.  If a condition is removed from the notifier that
is currently on the interposition stack, nothing is done to the
interposition stack.  It is assumed that higher levels of software
wouldn't call notify_next_*_func for that condition.

********************** Public Interface Supporting *********************
The public programming interface that the interposer supports follows:

notify_interpose_input_func
notify_interpose_output_func
notify_interpose_exception_func
notify_interpose_itimer_func
notify_interpose_signal_func
notify_interpose_wait3_func
notify_interpose_destroy_func
notify_interpose_event_func

notify_next_input_func
notify_next_output_func
notify_next_exception_func
notify_next_itimer_func
notify_next_signal_func
notify_next_wait3_func
notify_next_destroy_func
notify_next_event_func

notify_remove_input_func
notify_remove_output_func
notify_remove_exception_func
notify_remove_itimer_func
notify_remove_signal_func
notify_remove_wait3_func
notify_remove_destroy_func
notify_remove_event_func

*/

/* Private to interposer */
extern	NTFY_CONDITION *nint_stack;	/* Condition stack */
extern	int nint_stack_size;		/* Length of condition stack */
extern	int nint_stack_next;		/* Next empty slot in condition stack */

#endif	/* NINT_DEFINED */
