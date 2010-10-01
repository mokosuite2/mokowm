/*
 * Mokosuite
 * Virtual keyboard input window
 * Copyright (C) 2009-2010 Daniele Ricci <daniele.athome@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __MOKO_INPUT_H
#define __MOKO_INPUT_H

#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <glib.h>

extern int INPUT_WIDTH;
extern int INPUT_HEIGHT;
extern int INPUT_X;
extern int INPUT_Y;

Ecore_X_Window input_xwin(void);

void input_win_show(void);
void input_win_hide(void);

void input_win_switch(gboolean is_landscape);

Ecore_Evas* input_win_new(gboolean is_landscape);

#endif  /* __MOKO_INPUT_H */
