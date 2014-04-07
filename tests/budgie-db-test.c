/*
 * budgie-db-test.c
 * 
 * Copyright 2014 Ikey Doherty <ikey.doherty@gmail.com>
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
#include <stdlib.h>
#include <stdio.h>
#include <gio/gio.h>

#include "budgie-db.h"

int main(int argc, char **argv)
{
        BudgieDB *db = NULL;
        GFile *file = NULL;
        gchar *path = NULL;
        int ret = EXIT_FAILURE;
        int c;

        fprintf(stderr, "This program will overwrite your existing budgie-db\n"
                  "Only use if you know what you're doing!\n\n"
                  "Type \"y\" to continue:  ");
        c = getchar();
        switch (c) {
                case 'Y':
                case 'y':
                        break;
                default:
                        goto bail;
        }
        printf("Continuing\n");

        path = g_strdup_printf("%s/%s", g_get_user_config_dir(), CONFIG_NAME);
        file = g_file_new_for_path(path);
        /* Attempt to delete old database files */
        if (g_file_query_exists(file, NULL)) {
                if (!g_file_delete(file, NULL, NULL)) {
                        fprintf(stderr, "Unable to delete budgie db file!\n");
                        goto bail;
                }
        }

        db = budgie_db_new();

        /* TODO: Insert tests here :P */
        ret = EXIT_SUCCESS;

bail:
        g_free(path);
        if (file) {
                g_object_unref(file);
        }
        if (db) {
                g_object_unref(db);
        }

        return ret;
}
