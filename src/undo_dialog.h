/*
 * Sweep, a sound wave editor.
 *
 * Copyright (C) 2000 Conrad Parker
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __UNDO_DIALOG_H__
#define __UNDO_DIALOG_H__

#include <sweep/sweep_types.h>

void
undo_dialog_create (sw_sample * sample);

void
undo_dialog_set_sample (sw_sample * sample);

void
undo_dialog_refresh_history (sw_sample * sample);

void
undo_dialog_refresh_sample_list (void);

void
undo_dialog_refresh_edit_mode (sw_sample * sample);

#endif /* __UNDO_DIALOG_H__ */
