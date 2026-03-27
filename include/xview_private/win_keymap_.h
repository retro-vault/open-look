/*
 * Declares private XView interfaces, types, and macros for win keymap.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WIN_KEYMAP__H)
#define WIN_KEYMAP__H

#include <xview/pkg.h>
#include <xview/win_input.h>
#include <xview_private/win_keymap.h>

int win_metanormalize(register int c, register int mask);
void win_keymap_set_imask_from_std_bind(Inputmask *mask, unsigned short action);
void win_keymap_unset_imask_from_std_bind(Inputmask *mask, unsigned short action);
void win_keymap_fault_resolve(Xv_object win);
void win_keymap_copy_on_write(Xv_object win);
void win_keymap_set_smask(Xv_object win, unsigned short code);
void win_keymap_unset_smask(Xv_object win, unsigned short code);
int win_keymap_get_smask(Xv_object win, unsigned short code);
void win_keymap_set_smask_class(Xv_object win, Event_class class);
void win_keymap_set_imask_class(Inputmask *mask, Event_class class);
void win_keymap_enable(void);
void win_keymap_disable(void);
int win_keymap_map(Xv_object win, Event *event);
int win_keymap_show_inputmask(Inputmask *m);
int win_keymap_show_keymap(Xv_object win);
void win_keymap_map_code_and_masks(int newevent, int masklist[], int mapping, Xv_object win);
void win_keymap_unmap_code_and_masks(int newevent, int masklist[], Xv_object win);

#endif
