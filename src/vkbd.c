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

#include <Edje.h>
#include <Evas.h>
#include <Ecore_X.h>
#include <fakekey/fakekey.h>
#include <ctype.h>

#include "vkbd.h"

#define LANDSCAPE_INPUT_WIDTH     640
#define LANDSCAPE_INPUT_HEIGHT    254 //234
#define LANDSCAPE_INPUT_X         0
#define LANDSCAPE_INPUT_Y         (480 - LANDSCAPE_INPUT_HEIGHT)

#define PORTRAIT_INPUT_WIDTH     480
#define PORTRAIT_INPUT_HEIGHT    254 //234
#define PORTRAIT_INPUT_X         0
#define PORTRAIT_INPUT_Y         (640 - PORTRAIT_INPUT_HEIGHT)

typedef struct {
    gboolean is_shift_down;
    gboolean is_mouse_down;

    Evas_Object* kbd;
    FakeKey* fk;
    GHashTable* pressed_keys;
} vkbd_private_data;


static void each_release_key(gpointer key, gpointer value, gpointer data)
{
    edje_object_signal_emit((Evas_Object *) data, "release_key", (char*) key);
}

static void kbd_key_down(void *data, Evas_Object *obj, const char *emission, const char *source)
{
    wm_input_client* ic = data;
    vkbd_private_data* priv = ic->private;

    char* key = NULL;
    char* work = g_strdup(source);

    key = strstr(work, ":");
    if (!key)
        key = (char *)work;
    else
        key++;

    if (!strcmp(key, "enter")) {
        // TODO press_shift();
        // key press!!
        fakekey_press_keysym(priv->fk, XK_Return, 0);
        fakekey_release(priv->fk);
    }

    else if (!strcmp(key, "backspace")) {
        // key press!!
        fakekey_press_keysym(priv->fk, XK_BackSpace, 0);
        fakekey_release(priv->fk);
    }

    else if (!strcmp(key, "shift")) {
        // TODO toggle_shift();
    }

    else if (!strcmp(key, ".?123") || !strcmp(key, "ABC") || !strcmp(key, "#+=") || !strcmp(key, ".?12")) {
        // lol
        //pass
    }

    else if (!strcmp(key, "&")) {
        //g_string_append(text, "&amp;");
        //edje_object_part_text_set(obj, "field", text->str);
        // TODO
    }

    else if (!strcmp(key, "<")) {
        //g_string_append(text, "&lt;");
        //edje_object_part_text_set(obj, "field", text->str);
        // TODO
    }

    else if (!strcmp(key, ">")) {
        //g_string_append(text, "&gt;");
        //edje_object_part_text_set(obj, "field", text->str);
        // TODO
    }

    else {
        if (priv->is_shift_down) {
            // TODO release_shift();
            key[0] = toupper(key[0]);
        } else {
            key[0] = tolower(key[0]);
        }

        // key press!!!
        g_debug("Sending key press: %s (%c)", key, key[0]);
        fakekey_press(priv->fk, (unsigned char *) key, 1, 0);
        fakekey_release(priv->fk);
    }

    g_free(work);
}

static void kbd_mouse_over_key(void *data, Evas_Object *obj, const char *emission, const char *source)
{
    wm_input_client* ic = data;
    vkbd_private_data* priv = ic->private;

    // mmm...
    if (!priv->is_mouse_down || !strstr(source, ":")) return;

    char *work = g_strdup(source);
    char *part = NULL, *subpart = NULL;

    // sottoparte
    subpart = (strstr(work, ":") + 1);

    // parte
    *(subpart-1) = '\0';
    part = (char *) work;

    if (g_hash_table_lookup(priv->pressed_keys, subpart))
        return;

    #if 0
    for k in self.pressed_keys.values():
        o.signal_emit("release_key", k)
    self.pressed_keys.clear()
    self.pressed_keys[subpart] = subpart
    o.signal_emit("press_key", subpart)
    #else
    Evas_Object* part_obj = edje_object_part_swallow_get(obj, part);

    g_hash_table_foreach(priv->pressed_keys, each_release_key, part_obj);
    g_hash_table_remove_all(priv->pressed_keys);
    g_hash_table_insert(priv->pressed_keys, g_strdup(subpart), GINT_TO_POINTER(1));

    edje_object_signal_emit(part_obj, "press_key", subpart);
    #endif
}

static void kbd_mouse_out_key(void *data, Evas_Object *obj, const char *emission, const char *source)
{
    wm_input_client* ic = data;
    vkbd_private_data* priv = ic->private;

    // mmm...
    if (!priv->is_mouse_down || !strstr(source, ":")) return;

    char *work = g_strdup(source);
    char *part = NULL, *subpart = NULL;

    // sottoparte
    subpart = (strstr(work, ":") + 1);

    // parte
    *(subpart-1) = '\0';
    part = (char *) work;

    Evas_Object* part_obj = edje_object_part_swallow_get(obj, part);

    #if 0
    if subpart in self.pressed_keys:
        del self.pressed_keys[subpart]
        o.signal_emit("release_key", subpart)
    #else
    if (g_hash_table_lookup(priv->pressed_keys, subpart)) {
        g_hash_table_remove(priv->pressed_keys, subpart);
        edje_object_signal_emit(part_obj, "release_key", subpart);
    }
    #endif
}

static void kbd_mouse_down_key(void *data, Evas_Object *obj, const char *emission, const char *source)
{
    wm_input_client* ic = data;
    vkbd_private_data* priv = ic->private;

    // mmm...
    if (!strstr(source, ":")) return;

    char *work = g_strdup(source);
    char *part = NULL, *subpart = NULL;

    // sottoparte
    subpart = (strstr(work, ":") + 1);

    // parte
    *(subpart-1) = '\0';
    part = (char *) work;

    priv->is_mouse_down = TRUE;

    if (g_hash_table_lookup(priv->pressed_keys, subpart))
        return;

    // TODO
    #if 0
    for k in self.pressed_keys.values():
        o.signal_emit("release_key", k)
    self.pressed_keys.clear()
    self.pressed_keys[subpart] = subpart
    #else
    Evas_Object* part_obj = edje_object_part_swallow_get(obj, part);

    g_hash_table_foreach(priv->pressed_keys, each_release_key, part_obj);
    g_hash_table_remove_all(priv->pressed_keys);
    g_hash_table_insert(priv->pressed_keys, g_strdup(subpart), GINT_TO_POINTER(1));
    #endif

    edje_object_signal_emit(part_obj, "press_key", subpart);

    g_free(work);
}

static void kbd_mouse_up_key(void *data, Evas_Object *obj, const char *emission, const char *source)
{
    wm_input_client* ic = data;
    vkbd_private_data* priv = ic->private;

    // mmm...
    if (!strstr(source, ":")) return;

    char *work = g_strdup(source);
    char *part = NULL, *subpart = NULL;

    // sottoparte
    subpart = (strstr(work, ":") + 1);

    // parte
    *(subpart-1) = '\0';
    part = (char *) work;

    Evas_Object* part_obj = edje_object_part_swallow_get(obj, part);

    priv->is_mouse_down = FALSE;

    // TODO
    #if 0
    if subpart in self.pressed_keys:
        del self.pressed_keys[subpart]
        o.signal_emit("release_key", subpart)
        o.signal_emit("activated_key", subpart)
    #else
    if (g_hash_table_lookup(priv->pressed_keys, subpart)) {
        g_hash_table_remove(priv->pressed_keys, subpart);

        edje_object_signal_emit(part_obj, "release_key", subpart);
        edje_object_signal_emit(part_obj, "activated_key", subpart);
    }
    #endif
}

void vkbd_show(wm_input_client* ic)
{
    ecore_evas_show(ic->window);
}

void vkbd_hide(wm_input_client* ic)
{
    ecore_evas_hide(ic->window);
}

/* orientation change */
void vkbd_set_orientation(wm_input_client* ic, bool landscape)
{
    g_debug("[%s] old_landscape=%s, new_landscape=%s",
        __func__, ic->landscape ? "true" : "false", landscape ? "true" : "false");

    if (ic->landscape != landscape) {
        ic->landscape = landscape;

        if (landscape) {
            ic->width = LANDSCAPE_INPUT_WIDTH;
            ic->height = LANDSCAPE_INPUT_HEIGHT;
            ic->x = LANDSCAPE_INPUT_X;
            ic->y = LANDSCAPE_INPUT_Y;
        }
        else {
            ic->width = PORTRAIT_INPUT_WIDTH;
            ic->height = PORTRAIT_INPUT_HEIGHT;
            ic->x = PORTRAIT_INPUT_X;
            ic->y = PORTRAIT_INPUT_Y;
        }

        g_debug("[%s] resizing input window to %dx%d and moving to %dx%d",
                __func__, ic->width, ic->height, ic->x, ic->y);
        ecore_evas_move_resize(ic->window, ic->x, ic->y, ic->width, ic->height);

        vkbd_private_data* priv = ic->private;

        char* edjfile = g_strdup_printf(DATADIR "/mokosuite/vkbd.%s.edj", landscape ? "landscape" : "portrait");
        edje_object_file_set(priv->kbd, edjfile, "main");
        g_free(edjfile);

        evas_object_hide(priv->kbd);
        evas_object_resize(priv->kbd, ic->width, ic->height);
        evas_object_show(priv->kbd);
    }
}

wm_input_client* vkbd_create(wm_client* client, bool landscape)
{
    wm_input_client* ic = g_new0(wm_input_client, 1);
    vkbd_private_data* priv = g_new0(vkbd_private_data, 1);

    ic->client = client;
    ic->landscape = landscape;
    ic->private = priv;

    ic->set_orientation = vkbd_set_orientation;
    ic->show = vkbd_show;
    ic->hide = vkbd_hide;

    if (landscape) {
        ic->width = LANDSCAPE_INPUT_WIDTH;
        ic->height = LANDSCAPE_INPUT_HEIGHT;
        ic->x = LANDSCAPE_INPUT_X;
        ic->y = LANDSCAPE_INPUT_Y;
    }
    else {
        ic->width = PORTRAIT_INPUT_WIDTH;
        ic->height = PORTRAIT_INPUT_HEIGHT;
        ic->x = PORTRAIT_INPUT_X;
        ic->y = PORTRAIT_INPUT_Y;
    }

    g_debug("[%s] Creating input window", __func__);
    ic->window = ecore_evas_new(NULL, ic->x, ic->y, ic->width, ic->height, NULL);
    if (!ic->window) {
        g_warning("[%s] Unable to create input window canvas", __func__);
        g_free(ic->private);
        g_free(ic);
        return NULL;
    }

    ecore_x_netwm_window_type_set(ecore_evas_software_x11_window_get(ic->window),
        ECORE_X_WINDOW_TYPE_UTILITY);

    Evas* evas = ecore_evas_get(ic->window);

    priv->kbd = edje_object_add(evas);

    char* edjfile = g_strdup_printf(DATADIR "/mokosuite/vkbd.%s.edj", landscape ? "landscape" : "portrait");
    edje_object_file_set(priv->kbd, edjfile, "main");
    g_free(edjfile);

    evas_object_resize(priv->kbd, ic->width, ic->height);
    evas_object_show(priv->kbd);

    // eventi sulla tastiera
    edje_object_signal_callback_add(priv->kbd, "key_down", "*", kbd_key_down, ic);
    edje_object_signal_callback_add(priv->kbd, "mouse_over_key", "*", kbd_mouse_over_key, ic);
    edje_object_signal_callback_add(priv->kbd, "mouse_out_key", "*", kbd_mouse_out_key, ic);
    edje_object_signal_callback_add(priv->kbd, "mouse,down,1", "*", kbd_mouse_down_key, ic);
    edje_object_signal_callback_add(priv->kbd, "mouse,down,1,*", "*", kbd_mouse_down_key, ic);
    edje_object_signal_callback_add(priv->kbd, "mouse,up,1", "*", kbd_mouse_up_key, ic);

    ecore_evas_title_set(ic->window, "Virtual keyboard");
    // NON MOSTRARE SUBITO

    /*
    TODO
    self.on_mouse_down_add(self.on_mouse_down)
    self.on_mouse_up_add(self.on_mouse_up)
    self.on_key_down_add(self.on_key_down)
    */

    // inizializza fakekey
    priv->fk = fakekey_init(ecore_x_display_get());
    g_debug("[%s] FakeKey instance created (%p)", __func__, priv->fk);

    // array tasti premuti
    priv->pressed_keys = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

    return ic;
}
