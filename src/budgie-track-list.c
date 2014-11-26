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

        /* Append tracks to a pretty listbox */
        list = gtk_list_box_new();
        //        g_signal_connect(list, "row-activated",
        //                G_CALLBACK(list_selection_cb), self);
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

