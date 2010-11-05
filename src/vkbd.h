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

#ifndef __VKBD_H
#define __VKBD_H

#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <glib.h>

#include "input_client.h"

wm_input_client* vkbd_create(wm_client* client, gboolean is_landscape);

void vkbd_show(wm_input_client* ic);
void vkbd_hide(wm_input_client* ic);

void vkbd_set_orientation(wm_input_client* ic, gboolean is_landscape);

#endif  /* __VKBD_H */
