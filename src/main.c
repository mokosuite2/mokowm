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
#include <Edje.h>

#include <unistd.h>
#include <ctype.h>
#include <glib.h>
#include <glib-object.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "globals.h"
#include "wm.h"
#include "vkbd.h"
#include "qwo4.h"

// default log domain
int _log_dom = -1;

// input window in use
wm_input_client* input_win = NULL;


static void input_show(int sig_num)
{
    if (input_win)
        (input_win->show)(input_win);
}

static void input_hide(int sig_num)
{
    if (input_win)
        (input_win->hide)(input_win);
}

int main(int argc, char* argv[])
{
    eina_init();
    ecore_init();
    ecore_app_args_set(argc, (const char **)argv);

    _log_dom = eina_log_domain_register(PACKAGE, EINA_COLOR_CYAN);
    eina_log_domain_level_set(PACKAGE, LOG_LEVEL);

    bool disable_input = FALSE;
    bool enable_qwo4 = FALSE;

    int c;
    opterr = 0;
    while ((c = getopt (argc, argv, "iq")) != -1) {
        switch (c) {
            case 'i':
                disable_input = TRUE;
                break;

            case 'q':
                enable_qwo4 = TRUE;
                break;

            default:
                if (isprint(optopt))
                    EINA_LOG_ERR("Unknown option: `-%c'.", optopt);
                else
                    EINA_LOG_ERR("Unknown option: `-\\x%x'.", optopt);
                return EXIT_FAILURE;
        }
        opterr = 0;
    }


    if (!ecore_x_init(NULL)) {
        EINA_LOG_ERR("Cannot connect to X server. Exiting.");
        return EXIT_FAILURE;
    }

    ecore_x_netwm_init();

    // inizializza il wm
    wm_init();

    if (!disable_input) {
        ecore_evas_init();
        edje_init();

        Ecore_X_Randr_Orientation orientation =
            ecore_x_randr_screen_primary_output_orientation_get(ecore_x_window_root_first_get());

        gboolean landscape = (orientation == ECORE_X_RANDR_ORIENTATION_ROT_90 ||
            orientation == ECORE_X_RANDR_ORIENTATION_ROT_270);

        // scelta metodo di input
        input_win = ((enable_qwo4) ? qwo4_create : vkbd_create)(NULL, landscape);
    }

    // FIXME meglio un messagio X che dici? :)
    // segnaletica ;)
    struct sigaction usr1;
    usr1.sa_handler = input_show;
    usr1.sa_flags = 0;
    sigaction(SIGUSR1, &usr1, NULL);

    struct sigaction usr2;
    usr2.sa_handler = input_hide;
    usr2.sa_flags = 0;
    sigaction(SIGUSR2, &usr2, NULL);


    // gestisci tutte le finestre
    manage_all_windows();

    ecore_main_loop_begin();

    ecore_shutdown();
    eet_shutdown();
    eina_shutdown();

    return EXIT_SUCCESS;
}
