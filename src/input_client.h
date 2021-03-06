/*
 * Mokosuite
 * Window manager
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

#ifndef __MOKOWM_INPUT_CLIENT_H
#define __MOKOWM_INPUT_CLIENT_H

#include "client.h"
#include "globals.h"

typedef struct wm_input_client {
    // parent class (can be NULL)
    wm_client* client;

    // geometry
    int x;
    int y;
    int width;
    int height;

    // functions
    void (*set_orientation)(struct wm_input_client* ic, bool landscape);
    void (*show)(struct wm_input_client* ic);
    void (*hide)(struct wm_input_client* ic);

    Ecore_Evas* window;
    gboolean landscape;
    void* private;

} wm_input_client;

void input_client_reconfigure(wm_client *c);

void input_client_destroy(wm_client *c);
void input_client_hide(wm_client *c);
void input_client_show(wm_client *c);

void input_client_screen_changed(wm_input_client *ic, Ecore_X_Randr_Orientation orientation);

#endif  /* __MOKOWM_INPUT_CLIENT_H */
