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

#include <Ecore_X.h>
#include <Ecore_Input.h>
#include <Ecore.h>
#include <Eet.h>
#include <Eina.h>

#include <glib.h>
#include <glib-object.h>

#include "input_client.h"
#include "client.h"
#include "wm.h"
#include "input.h"

void input_client_reconfigure(wm_client *c)
{
    ecore_x_window_geometry_get(root_win, NULL, NULL, &c->rect.w, NULL);

    // TODO
    //c->rect.x = c->rect.y = 0;

    // spazio riservato :)
    if (c->visible)
        reserved_bottom = c->rect.h;
}

void input_client_show(wm_client *c)
{
    reserved_bottom = c->rect.h;
    resize_all_clients();
}

void input_client_destroy(wm_client *c)
{
    reserved_bottom = 0;
    resize_all_clients();
}

void input_client_hide(wm_client *c)
{
    // e' piu' o meno la stessa cosa...
    input_client_destroy(c);
}

void input_client_screen_changed(wm_client *c, Ecore_X_Randr_Orientation orientation)
{
    input_win_switch(orientation == ECORE_X_RANDR_ORIENTATION_ROT_90 ||
        orientation == ECORE_X_RANDR_ORIENTATION_ROT_270);

    // HACK purtroppo non si sa perche' la configure_request non arriva :(
    c->rect.x = INPUT_X;
    c->rect.y = INPUT_Y;
    c->rect.w = INPUT_WIDTH;
    c->rect.h = INPUT_HEIGHT;
}
