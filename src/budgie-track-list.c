/*
 * budgie-track-list.c
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
#include "config.h"
#include "budgie-track-list.h"
#include "util.h"

G_DEFINE_TYPE(BudgieTrackList, budgie_track_list, GTK_TYPE_BIN)

/* Boilerplate GObject code */
static void budgie_track_list_class_init(BudgieTrackListClass *klass);
static void budgie_track_list_init(BudgieTrackList *self);
static void budgie_track_list_dispose(GObject *object);

/* Initialisation */
static void budgie_track_list_class_init(BudgieTrackListClass *klass)
{
        GObjectClass *g_object_class;

        g_object_class = G_OBJECT_CLASS(klass);
        g_object_class->dispose = &budgie_track_list_dispose;
}

static void budgie_track_list_init(BudgieTrackList *self)
{
        GtkWidget *page;
        GtkWidget *info_box;
        GtkWidget *image;
        GtkWidget *label;
        
        GtkWidget *list;
        GtkWidget *scroll;

        GtkCellRenderer *renderer;
        GtkTreeViewColumn *column;
        
        GtkStyleContext *style;

        page = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_container_set_border_width(GTK_CONTAINER(page), 0);
        /* Don't forget to add us to the stack. */

        /* Info box stores image, count, etc. */
        info_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_box_pack_start(GTK_BOX(page), info_box, FALSE, FALSE, 0);
        gtk_widget_set_valign(info_box, GTK_ALIGN_FILL);

        /* Image for album art */
        image = gtk_image_new();
        gtk_widget_set_halign(image, GTK_ALIGN_START);
        gtk_widget_set_valign(image, GTK_ALIGN_CENTER);
        g_object_set(image, "margin-top", 20, NULL);
        g_object_set(image, "margin-left", 20, NULL);
        g_object_set(image, "margin-right", 20, NULL);
        gtk_box_pack_start(GTK_BOX(info_box), image, FALSE, FALSE, 0);
        gtk_image_set_pixel_size(GTK_IMAGE(image), 256);
        self->image = image;

        /* Album, etc, */
        label = gtk_label_new("");
        gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
        gtk_label_set_max_width_chars(GTK_LABEL(label), 30);
        gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
        g_object_set(label, "margin-bottom", 10,
                "margin-left", 5, NULL);
        self->current_label = label;
        gtk_box_pack_start(GTK_BOX(info_box), label, FALSE, FALSE, 0);
        style = gtk_widget_get_style_context(label);
        
        /* Count label */
        label = gtk_label_new("");
        self->count_label = label;
        gtk_box_pack_start(GTK_BOX(info_box), label, FALSE, FALSE, 0);
        gtk_widget_set_valign(label, GTK_ALIGN_END);
        style = gtk_widget_get_style_context(label);
        gtk_style_context_add_class(style, "info-label");

        /* Construct the track list box. */
        self->store = gtk_list_store_new(BUDGIE_TRACK_LIST_DB_NUM_FIELDS,
                G_TYPE_STRING, /* Title */
                G_TYPE_INT, /* Track no */
                G_TYPE_STRING, /* Artist/author */
                G_TYPE_STRING, /* Album */
                G_TYPE_STRING, /* Band */
                G_TYPE_STRING, /* Genre */
                G_TYPE_STRING, /* File path */
                G_TYPE_STRING, /* MIME type */
                G_TYPE_POINTER, /* MediaInfo */
                G_TYPE_STRING /* Now-playing */
                );

        /* Append tracks to a pretty listbox */
        list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(self->store));
        gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(list), TRUE);
        g_object_set(G_OBJECT(list), "headers-visible", FALSE, NULL);

        /* Add columns for the list. */
        renderer = gtk_cell_renderer_pixbuf_new();
        column = gtk_tree_view_column_new_with_attributes("  ",
                renderer,
                "icon-name", BUDGIE_TRACK_LIST_DB_PLAYING,
                NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

        /* Title */
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Title",
                renderer,
                "text", BUDGIE_TRACK_LIST_DB_TITLE,
                NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

        /* Artist */
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Artist",
                renderer,
                "text", BUDGIE_TRACK_LIST_DB_ARTIST,
                NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

        gtk_widget_set_halign(list, GTK_ALIGN_FILL);
        self->list = list;

        /* Scroller for the listbox */
        scroll = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_kinetic_scrolling(GTK_SCROLLED_WINDOW(scroll),
                TRUE);
        gtk_container_add(GTK_CONTAINER(scroll), list);
        gtk_box_pack_start(GTK_BOX(page), scroll, TRUE, TRUE, 0);
        g_object_set(scroll, "margin-top", 20, NULL);
        gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll),
                GTK_SHADOW_NONE);

        /* Add the page to this container. */
        gtk_container_add(GTK_CONTAINER(self), page);
}

static void budgie_track_list_dispose(GObject *object)
{
        /* Destruct */
        G_OBJECT_CLASS (budgie_track_list_parent_class)->dispose (object);
}

/* Utility; return a new BudgieTrackList */
GtkWidget* budgie_track_list_new(void)
{
        BudgieTrackList *self;

        self = g_object_new(BUDGIE_TRACK_LIST_TYPE, NULL);
        return GTK_WIDGET(self);
}

void budgie_track_list_update_playing(BudgieTrackList *self, MediaInfo *now_playing)
{
        GtkTreeModel *model;
        GtkTreeIter iter;
        MediaInfo *info;

        model = gtk_tree_view_get_model(GTK_TREE_VIEW(self->list)); /* should be == self->store */

        /* Only have the playing track play. */
        gtk_tree_model_get_iter_first(model, &iter);
        do {
                gtk_tree_model_get(model,
                        &iter,
                        BUDGIE_TRACK_LIST_DB_INFO, &info,
                        -1);
                if (info == now_playing) {
                        gtk_list_store_set(GTK_LIST_STORE(model),
                                &iter,
                                BUDGIE_TRACK_LIST_DB_PLAYING, "media-playback-start",
                                -1);
                        GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(self->store), &iter);
                        gtk_tree_view_set_cursor(GTK_TREE_VIEW(self->list), path, NULL, FALSE);

                }
                else {
                        gtk_list_store_set(GTK_LIST_STORE(model),
                                &iter,
                                BUDGIE_TRACK_LIST_DB_PLAYING, NULL,
                                -1);
                }
        } while (gtk_tree_model_iter_next(model, &iter));
}
