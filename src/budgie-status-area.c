/*
 * budgie-status-area.c
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
#include <gst/gst.h>

#include "budgie-status-area.h"
#include "util.h"

/* Private storage */
struct _BudgieStatusAreaPrivate {
        gulong seek_id;
        GtkWidget *label;
        GtkWidget *time_label;
        GtkWidget *slider;

};

G_DEFINE_TYPE_WITH_CODE(BudgieStatusArea, budgie_status_area, GTK_TYPE_GRID, G_ADD_PRIVATE(BudgieStatusArea))

static void changed_cb(GtkWidget *widget, gdouble value, gpointer userdata);

/* Boilerplate GObject code */
static void budgie_status_area_class_init(BudgieStatusAreaClass *klass);
static void budgie_status_area_init(BudgieStatusArea *self);
static void budgie_status_area_dispose(GObject *object);

/* Initialisation */
static void budgie_status_area_class_init(BudgieStatusAreaClass *klass)
{
        GObjectClass *g_object_class;

        g_object_class = G_OBJECT_CLASS(klass);
        g_object_class->dispose = &budgie_status_area_dispose;

        g_signal_new("seek",
                G_TYPE_OBJECT, G_SIGNAL_RUN_FIRST,
                0, NULL, NULL, NULL, G_TYPE_NONE,
                1, G_TYPE_INT64);

        /* Set our template */
        gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(klass),
                "/com/evolve-os/budgie/media-player/status-area.ui");

        /* Map widgets */
        gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(klass), BudgieStatusArea, label);
        gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(klass), BudgieStatusArea, time_label);
        gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(klass), BudgieStatusArea, slider);
}

static void budgie_status_area_init(BudgieStatusArea *self)
{
        gtk_widget_init_template(GTK_WIDGET(self));

        self->priv = budgie_status_area_get_instance_private(self);

        self->priv->seek_id = g_signal_connect(self->priv->slider,
                "value-changed", G_CALLBACK(changed_cb), self);

        gtk_label_set_markup(GTK_LABEL(self->priv->label),
                "<b>Budgie Media Player</b>");
        gtk_label_set_text(GTK_LABEL(self->priv->time_label), "");

        gtk_widget_set_size_request(GTK_WIDGET(self), 400, -1);
}

static void budgie_status_area_dispose(GObject *object)
{
        /* Destruct */
        G_OBJECT_CLASS (budgie_status_area_parent_class)->dispose (object);
}

/* Utility; return a new BudgieStatusArea */
GtkWidget* budgie_status_area_new(void)
{
        BudgieStatusArea *self;

        self = g_object_new(BUDGIE_STATUS_AREA_TYPE, NULL);
        return GTK_WIDGET(self);
}

void budgie_status_area_set_media(BudgieStatusArea *self, MediaInfo *info)
{
        gchar *title_string = NULL;

        if (info->artist) {
                title_string = g_markup_printf_escaped("<b>%s</b> <i>by</i> <b>%s</b>",
                        info->title, info->artist);
        } else {
                title_string = g_markup_printf_escaped("<b>%s</b>", info->title);
        }

        gtk_label_set_markup(GTK_LABEL(self->priv->label), title_string);
        gtk_label_set_max_width_chars(GTK_LABEL(self->priv->label), 1);
        gtk_widget_queue_draw(GTK_WIDGET(self));

        g_free(title_string);
}

void budgie_status_area_set_media_time(BudgieStatusArea *self, gint64 max, gint64 current)
{
        gchar *time_string = NULL;
        gchar *total_string = NULL;
        gchar *lab_string = NULL;

        /* Clear info */
        if (max < 0) {
                gtk_widget_set_visible(self->priv->slider, FALSE);
                return;
        }
        gtk_widget_set_visible(self->priv->slider, TRUE);
        gint elapsed;

        elapsed = current/GST_SECOND;
        time_string = format_seconds(elapsed, FALSE);

        /* Update slider */
        current /= GST_SECOND;
        max /= GST_SECOND;
        total_string = format_seconds(max, FALSE);

        g_signal_handler_block(self->priv->slider, self->priv->seek_id);
        gtk_range_set_range(GTK_RANGE(self->priv->slider), 0, max);
        gtk_range_set_value(GTK_RANGE(self->priv->slider), current);
        g_signal_handler_unblock(self->priv->slider, self->priv->seek_id);

        /* Update labels */
        lab_string = g_strdup_printf("%s / %s", time_string, total_string);
        gtk_label_set_markup(GTK_LABEL(self->priv->time_label), lab_string);

        g_free(lab_string);
        g_free(time_string);
        g_free(total_string);
}

static void changed_cb(GtkWidget *widget, gdouble value, gpointer userdata)
{
        BudgieStatusArea *self;
        gint64 num;

        num = gtk_range_get_value(GTK_RANGE(widget)) * GST_SECOND;

        self = BUDGIE_STATUS_AREA(userdata);
        g_signal_emit_by_name(self, "seek", num);
}
