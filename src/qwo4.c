/*
 * Mokosuite
 * Quickwriting4 input method
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

#include <Evas.h>
#include <fakekey/fakekey.h>

#include "globals.h"
#include "qwo4.h"

// FIXME these have to be fixed
#define LANDSCAPE_INPUT_WIDTH     640
#define LANDSCAPE_INPUT_HEIGHT    254 //234
#define LANDSCAPE_INPUT_X         0
#define LANDSCAPE_INPUT_Y         (480 - LANDSCAPE_INPUT_HEIGHT)


#define PORTRAIT_INPUT_WIDTH     480
#define PORTRAIT_INPUT_HEIGHT    320
#define PORTRAIT_INPUT_X         0
#define PORTRAIT_INPUT_Y         (640 - PORTRAIT_INPUT_HEIGHT)

#define GLYPH_FONT          "Sans:style=bold"
#define GLYPH_SIZE_NORMAL   18
#define GLYPH_SIZE_ACTIVE   20
#define GLYPH_COLOR_NORMAL  200, 200, 200, 255
#define GLYPH_COLOR_ACTIVE  0, 0, 0, 255

#define XCOORD(v)       (v[0])
#define YCOORD(v)       (v[1])

#define SECTION_OFFSET  50

// il quadrato sara' calcolato all'avvio
#define CENTER_RADIUS   60
static int center_radius = CENTER_RADIUS * CENTER_RADIUS;

// sezioni
#define NUM_SECTIONS        4

// lettere disponibili
#define LETTERS_PER_SECTION     NUM_SECTIONS
//static char letters[] = { "abcdefghijklmnopqrstuvwzyx.,?!@'" };
static char letters[] = { "abcd?!.,ijklefghqrstmnopyz@'uvwx" };

typedef struct {
    FakeKey* fk;

    // TRUE quando siamo fuori dal centro per navigare
    bool moving;
    // TRUE quando stiamo muovendo il mouse nel centro
    bool in_center;
    // TRUE se non siamo ancora usciti dal centro dopo una pressione
    bool is_space;

    // sezione corrente
    int cur_section; // = -1;
    // sezione di partenza
    int start_section; // = -1;
    // indice di lettera corrente
    int cur_letter;

    Evas_Object** glyphs;
    Evas_Object* old_glyph;
    int sections[NUM_SECTIONS][3][2];

    int center_point[2];

} qwo4_private_data;

// thanks to http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
static int pnpoly(int nvert, int vert[][2], int testx, int testy)
{
  int i, j, c = 0;
  for (i = 0, j = nvert-1; i < nvert; j = i++) {
    if ( ( (YCOORD(vert[i]) > testy) != (YCOORD(vert[j]) > testy) ) &&
     (testx < (XCOORD(vert[j]) - XCOORD(vert[i])) * (testy - YCOORD(vert[i])) / (YCOORD(vert[j]) - YCOORD(vert[i])) + XCOORD(vert[i])) )
       c = !c;
  }
  return c;
}

static int distance2(int x1, int y1, int x2, int y2)
{
    int xx = x2 - x1;
    int yy = y2 - y1;
    return (xx * xx) + (yy * yy);
}

static bool inside_center(qwo4_private_data* priv, int x, int y)
{
    return distance2(x, y, XCOORD(priv->center_point), YCOORD(priv->center_point)) <= center_radius;
}

static void highlight_letter(qwo4_private_data* priv)
{
    Evas_Object* gly = NULL;
    if (priv->cur_letter)
        gly = priv->glyphs[
            (priv->start_section * LETTERS_PER_SECTION * 2) + // macroblocco (sezione)
            ((priv->cur_letter < 0 ? 1 : 0) * LETTERS_PER_SECTION) + // microblocco (destra-sinistra)
            (abs(priv->cur_letter) - 1) // lettera
        ];

    if (priv->old_glyph) {
        evas_object_text_font_set(priv->old_glyph, GLYPH_FONT, GLYPH_SIZE_NORMAL);
        evas_object_color_set(priv->old_glyph, GLYPH_COLOR_NORMAL);
        priv->old_glyph = NULL;
    }
    if (gly) {
        /*
        EINA_LOG_DBG("Activating glyph %p [%d]", gly,
            (priv->start_section * LETTERS_PER_SECTION * 2) + // macroblocco (sezione)
            ((priv->cur_letter < 0 ? 1 : 0) * LETTERS_PER_SECTION) + // microblocco (destra-sinistra)
            (abs(priv->cur_letter) - 1) // lettera
        );
        */
        evas_object_text_font_set(gly, GLYPH_FONT, GLYPH_SIZE_ACTIVE);
        evas_object_color_set(gly, GLYPH_COLOR_ACTIVE);
        priv->old_glyph = gly;
    }
}

static void reset(qwo4_private_data* priv)
{
    priv->start_section = -1;
    priv->cur_section = -1;
    priv->cur_letter = 0;
    highlight_letter(priv);
}

static int find_section(qwo4_private_data* priv, int x, int y)
{
    int i;
#if 1
    for (i = 0; i < NUM_SECTIONS; i++)
        if (pnpoly(3, priv->sections[i], x, y))
            return i;
#else
    for (i = 0; i < NUM_SECTIONS; i++) {
        int n = pnpoly(3, priv->sections[i], x, y);
        EINA_LOG_DBG("poly[%d]=%d", i, n);
        if (n) return i;
    }
#endif

    return -1;
}

static void _press(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    wm_input_client* ic = data;
    qwo4_private_data* priv = ic->private;

    Evas_Event_Mouse_Down* event = event_info;
    int x = event->canvas.x;
    int y = event->canvas.y;

    // iniziata pressione!
    if (inside_center(priv, x, y)) {
        EINA_LOG_DBG("Press inside center!");
        priv->moving = TRUE;
        priv->is_space = TRUE;
        priv->cur_section = -1;
        priv->in_center = TRUE;
    }
}

static void _move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    wm_input_client* ic = data;
    qwo4_private_data* priv = ic->private;

    if (priv->moving) {
        // subito ferma lo spazio
        priv->is_space = FALSE;

        Evas_Event_Mouse_Move* event = event_info;
        int x = event->cur.canvas.x;
        int y = event->cur.canvas.y;
        int section = find_section(priv, event->cur.canvas.x, event->cur.canvas.y);
        bool move_center = inside_center(priv, x, y);
        //EINA_LOG_DBG("x=%d, y=%d, section=%d", x, y, section);

        // aaaaahhh!!!!
        if (section < 0 && !move_center) {
            //EINA_LOG_DBG("AAAAAHHH!!!");
            //moving = FALSE;
            //reset();
            return;
        }

        // siamo al centro?
        if (!priv->in_center) {
            if (move_center) {
                // lettera! :)
                // bell'algoritmo :D
                EINA_LOG_DBG("Got letter (start_section=%d, cur_letter=%d)", priv->start_section, priv->cur_letter);
                if (priv->cur_letter) {
                    char letter = letters[
                        (priv->start_section * LETTERS_PER_SECTION * 2) + // macroblocco (sezione)
                        ((priv->cur_letter < 0 ? 1 : 0) * LETTERS_PER_SECTION) + // microblocco (destra-sinistra)
                        (abs(priv->cur_letter) - 1) // lettera
                    ];

                    EINA_LOG_DBG("back to center, letter: \"%c\"", letter);
                    fakekey_press(priv->fk, (unsigned char *) &letter, 1, 0);
                    fakekey_release(priv->fk);
                }
                else {
                    EINA_LOG_DBG("back to center, no letter");
                }

                // reset
                reset(priv);
            }
        }

        priv->in_center = move_center;
        // siamo al centro :D
        if (priv->in_center) return;

        // ci siamo spostati!
        if (priv->cur_section >= 0) {
            int diff = section - priv->cur_section;
            if (diff != 0) {
                // caso particolare: boundaries settori (evita di usare abs())
                if (diff != 1 && diff != -1) {
                    // evita di usare abs()
                    if (diff < 0) diff = 1;
                    else diff = -1;
                }

                EINA_LOG_DBG("move difference %d (new_section=%d, cur_section=%d)", diff, section, priv->cur_section);
                priv->cur_letter += diff;
                if (priv->cur_letter > LETTERS_PER_SECTION)
                    priv->cur_letter = 1;
                else if (priv->cur_letter < -LETTERS_PER_SECTION)
                    priv->cur_letter = -1;

                highlight_letter(priv);
            }
        }
        else {
            priv->start_section = section;
            priv->cur_letter = 0;
        }

        priv->cur_section = section;
    }
}

static void _release(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    wm_input_client* ic = data;
    qwo4_private_data* priv = ic->private;

    if (priv->is_space) {
        fakekey_press(priv->fk, (unsigned char *) " ", 1, 0);
        fakekey_release(priv->fk);
    }

    EINA_LOG_DBG("releasing");
    priv->moving = FALSE;
    priv->is_space = FALSE;
    reset(priv);
}

static Evas_Object* draw_letter(qwo4_private_data* priv, Evas* evas, const char* text, int center_x, int center_y)
{
    EINA_LOG_DBG("Glyph %s, x=%d, y=%d", text, center_x, center_y);
    Evas_Object* a = evas_object_text_add(evas);
    evas_object_text_font_set(a, GLYPH_FONT, GLYPH_SIZE_NORMAL);
    evas_object_color_set(a, GLYPH_COLOR_NORMAL);
    evas_object_pass_events_set(a, TRUE);
    evas_object_text_text_set(a, text);
    evas_object_move(a, priv->center_point[0] + center_x, priv->center_point[1] + center_y);
    evas_object_show(a);
    return a;
}

void qwo4_show(wm_input_client* ic)
{
    ecore_evas_show(ic->window);
}

void qwo4_hide(wm_input_client* ic)
{
    ecore_evas_hide(ic->window);
}

void qwo4_set_orientation(wm_input_client* ic, bool landscape)
{
    EINA_LOG_WARN("orientation change not supported yet.");
}

wm_input_client* qwo4_create(wm_client* c, bool landscape)
{
    wm_input_client* ic = calloc(1, sizeof(wm_input_client));
    qwo4_private_data* priv = calloc(1, sizeof(qwo4_private_data));

    ic->client = c;
    ic->landscape = landscape;
    ic->private = priv;

    ic->set_orientation = qwo4_set_orientation;
    ic->show = qwo4_show;
    ic->hide = qwo4_hide;

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

    priv->center_point[0] = ic->width / 2;
    priv->center_point[1] = ic->height / 2;

    // upper section
    priv->sections[0][0][0] = SECTION_OFFSET;
    priv->sections[0][1][0] = ic->width - SECTION_OFFSET;
    priv->sections[0][2][0] = XCOORD(priv->center_point);
    priv->sections[0][2][1] = YCOORD(priv->center_point);

    // right section
    priv->sections[1][0][0] = ic->width - SECTION_OFFSET;
    priv->sections[1][1][0] = ic->width - SECTION_OFFSET;
    priv->sections[1][1][1] = ic->height;
    priv->sections[1][2][0] = XCOORD(priv->center_point);
    priv->sections[1][2][1] = YCOORD(priv->center_point);

    // bottom section
    priv->sections[2][0][0] = SECTION_OFFSET;
    priv->sections[2][0][1] = ic->height;
    priv->sections[2][1][0] = ic->width - SECTION_OFFSET;
    priv->sections[2][1][1] = ic->height;
    priv->sections[2][2][0] = XCOORD(priv->center_point);
    priv->sections[2][2][1] = YCOORD(priv->center_point);

    // left section
    priv->sections[3][0][0] = SECTION_OFFSET;
    priv->sections[3][0][1] = ic->height;
    priv->sections[3][1][0] = SECTION_OFFSET;
    priv->sections[3][2][0] = XCOORD(priv->center_point);
    priv->sections[3][2][1] = YCOORD(priv->center_point);

#if 0
    { {0, 0}, {WIDTH, 0}, POINT_CENTER },               // sezione superiore
    { {WIDTH, 0}, {WIDTH, HEIGHT}, POINT_CENTER },      // sezione destra
    { {0, HEIGHT}, {WIDTH, HEIGHT}, POINT_CENTER },     // sezione inferiore
    { {0, HEIGHT}, {0, 0}, POINT_CENTER }               // sezione sinistra
#endif

    // finestra input principale
    ic->window = ecore_evas_new(NULL, ic->x, ic->y, ic->width, ic->height, NULL);
    if (!ic->window) {
        EINA_LOG_ERR("unable to create input window canvas");
        free(ic->private);
        free(ic);
        return NULL;
    }

    ecore_x_netwm_window_type_set(ecore_evas_software_x11_window_get(ic->window),
        ECORE_X_WINDOW_TYPE_UTILITY);

    ecore_evas_title_set(ic->window, "qwo4");
    Evas* evas = ecore_evas_get(ic->window);

    //Evas_Object *bg = evas_object_rectangle_add(evas);
    Evas_Object *bg = evas_object_image_filled_add(evas);
    evas_object_image_file_set(bg, DATADIR "/mokosuite/wm/qwo4_back.png", NULL);
    evas_object_resize(bg, ic->width, ic->height);
    evas_object_color_set(bg, 255, 255, 255, 255);
    evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(bg, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_show(bg);

#if 0
    Evas_Object* diag1 = evas_object_line_add(evas);
    evas_object_color_set(diag1, 0, 255, 0, 255);
    evas_object_line_xy_set(diag1, SECTION_OFFSET, 0, ic->width - SECTION_OFFSET, ic->height);
    evas_object_pass_events_set(diag1, TRUE);
    evas_object_show(diag1);

    Evas_Object* diag2 = evas_object_line_add(evas);
    evas_object_color_set(diag2, 0, 0, 255, 255);
    evas_object_line_xy_set(diag2, SECTION_OFFSET, ic->height, ic->width - SECTION_OFFSET, 0);
    evas_object_pass_events_set(diag2, TRUE);
    evas_object_show(diag2);
#endif

    Evas_Object** glyphs = priv->glyphs = calloc(strlen(letters), sizeof(Evas_Object*));

    // diagonale primaria \ y = -K * x
    // -40 = x*30
    // x*30 = -40
    // x = -40/30 = -4/3
#define LINE_PRI(x, off)     (((float)-4/5*x) - off)
#define SECT1_OFF       60
    glyphs[0] = draw_letter(priv, evas, "a", 40, LINE_PRI(40, SECT1_OFF));
    glyphs[1] = draw_letter(priv, evas, "b", 70, LINE_PRI(70, SECT1_OFF));
    glyphs[2] = draw_letter(priv, evas, "c", 100, LINE_PRI(100, SECT1_OFF));
    glyphs[3] = draw_letter(priv, evas, "d", 130, LINE_PRI(130, SECT1_OFF));

#define SECT2_OFF       -10
    glyphs[12] = draw_letter(priv, evas, "e", 80, LINE_PRI(80, SECT2_OFF));
    glyphs[13] = draw_letter(priv, evas, "f", 110, LINE_PRI(110, SECT2_OFF));
    glyphs[14] = draw_letter(priv, evas, "g", 140, LINE_PRI(140, SECT2_OFF));
    glyphs[15] = draw_letter(priv, evas, "h", 170, LINE_PRI(170, SECT2_OFF));

#define SECT4_OFF       -20
    glyphs[16] = draw_letter(priv, evas, "q", -40, LINE_PRI(-40, SECT4_OFF));
    glyphs[17] = draw_letter(priv, evas, "r", -70, LINE_PRI(-70, SECT4_OFF));
    glyphs[18] = draw_letter(priv, evas, "s", -100, LINE_PRI(-100, SECT4_OFF));
    glyphs[19] = draw_letter(priv, evas, "t", -130, LINE_PRI(-130, SECT4_OFF));

#define SECT5_OFF       50
    glyphs[28] = draw_letter(priv, evas, "u", -80, LINE_PRI(-80, SECT5_OFF));
    glyphs[29] = draw_letter(priv, evas, "v", -110, LINE_PRI(-110, SECT5_OFF));
    glyphs[30] = draw_letter(priv, evas, "w", -140, LINE_PRI(-140, SECT5_OFF));
    glyphs[31] = draw_letter(priv, evas, "x", -170, LINE_PRI(-170, SECT5_OFF));

    // diagonale secondaria /
#define LINE_SEC(x, off)     (((float)3.5/5*x) - off)
    glyphs[4] = draw_letter(priv, evas, "?", -40, LINE_SEC(-40, SECT1_OFF));
    glyphs[5] = draw_letter(priv, evas, "!", -70, LINE_SEC(-70, SECT1_OFF));
    glyphs[6] = draw_letter(priv, evas, ".", -100, LINE_SEC(-100, SECT1_OFF));
    glyphs[7] = draw_letter(priv, evas, ",", -130, LINE_SEC(-130, SECT1_OFF));

#define SECT2_1_OFF       -20
    glyphs[24] = draw_letter(priv, evas, "y", -80, LINE_SEC(-80, SECT2_1_OFF));
    glyphs[25] = draw_letter(priv, evas, "z", -110, LINE_SEC(-110, SECT2_1_OFF));
    glyphs[26] = draw_letter(priv, evas, "@", -140, LINE_SEC(-140, SECT2_1_OFF));
    glyphs[27] = draw_letter(priv, evas, "'", -170, LINE_SEC(-170, SECT2_1_OFF));

#define SECT5_1_OFF       40
    glyphs[8] = draw_letter(priv, evas, "i", 80, LINE_SEC(80, SECT5_1_OFF));
    glyphs[9] = draw_letter(priv, evas, "j", 110, LINE_SEC(110, SECT5_1_OFF));
    glyphs[10] = draw_letter(priv, evas, "k", 140, LINE_SEC(140, SECT5_1_OFF));
    glyphs[11] = draw_letter(priv, evas, "l", 170, LINE_SEC(170, SECT5_1_OFF));

#define SECT2_2_OFF      -35
    glyphs[20] = draw_letter(priv, evas, "m", 30, LINE_SEC(30, SECT2_2_OFF));
    glyphs[21] = draw_letter(priv, evas, "n", 70, LINE_SEC(70, SECT2_2_OFF));
    glyphs[22] = draw_letter(priv, evas, "o", 100, LINE_SEC(100, SECT2_2_OFF));
    glyphs[23] = draw_letter(priv, evas, "p", 130, LINE_SEC(130, SECT2_2_OFF));

    // i callback sono sullo sfondo
    evas_object_event_callback_add(bg, EVAS_CALLBACK_MOUSE_MOVE, _move, ic);
    evas_object_event_callback_add(bg, EVAS_CALLBACK_MOUSE_DOWN, _press, ic);
    evas_object_event_callback_add(bg, EVAS_CALLBACK_MOUSE_UP, _release, ic);

    // inizializza fakekey
    priv->fk = fakekey_init(ecore_x_display_get());
    EINA_LOG_DBG("FakeKey instance created (%p)", priv->fk);

    reset(priv);
    return ic;
}
