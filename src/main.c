/*
 * main.c
 * 
 * Copyright 2013 Ikey Doherty <ikey.doherty@gmail.com>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include "budgie-window.h"

#define BUDGIE_APP_ID "com.evolve-os.budgie.MediaPlayer"

static BudgieWindow *window;

static void perform_migration(void)
{
        int i;
        gchar *path;
        const gchar *config;
        GFile *file;
        guint path_len;

        const gchar* paths[] = {
                "idmp-1.db",
        };
        path_len = 1;

        /* $HOME/.config/ */
        config = g_get_user_config_dir();
        for (i=0; i < path_len; i++) {
                path = g_strdup_printf("%s/%s", config, paths[i]);
                file = g_file_new_for_path(path);
                /* Attempt to delete old database files */
                if (g_file_query_exists(file, NULL)) {
                        if (!g_file_delete(file, NULL, NULL))
                                g_warning("Unable to delete old DB: %s", path);
                }
                g_object_unref(file);
                g_free(path);
        }
}

static void do_activate(GApplication *app, gpointer userdata)
{
        /* Already initialised and running */
        if (window) {
                gtk_widget_grab_focus(GTK_WIDGET(window));
                return;
        }
        /* Allow error handling in future */
        perform_migration();

        /* Window shows itself right now */
        window = budgie_window_new(GTK_APPLICATION(app));
}

int main(int argc, char **argv)
{
        gint ret;
        GtkApplication *app = NULL;
        gst_init(&argc, &argv);

        /* In the future we'll support command line too */
        app = gtk_application_new(BUDGIE_APP_ID, G_APPLICATION_FLAGS_NONE);
        g_signal_connect(app, "activate", G_CALLBACK(do_activate), NULL);

        ret = g_application_run(G_APPLICATION(app), argc, argv);

        g_object_unref(app);

        return ret;
}

