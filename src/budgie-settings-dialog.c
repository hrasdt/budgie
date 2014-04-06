/*
 * budgie-settings-dialog.c
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
#include "budgie-settings-dialog.h"
#include "util.h"
#include "common.h"

/* Boilerplate GObject code */
static void budgie_settings_dialog_class_init(BudgieSettingsDialogClass *klass);
static void budgie_settings_dialog_init(BudgieSettingsDialog *self);
static void budgie_settings_dialog_dispose(GObject *object);

static void paths_cursor_cb(GtkWidget *widget, gpointer userdata);
static void paths_add_cb(GtkWidget *widget, gpointer userdata);
static void paths_remove_cb(GtkWidget *widget, gpointer userdata);
static void budgie_settings_refresh(BudgieSettingsDialog*self);

struct _BudgieSettingsDialogPriv {
        GSettings *settings;

        GtkWidget *toolbutton_add;
        GtkWidget *toolbutton_remove;
        GtkWidget *treeview_dirs;
        GtkWidget *dark_switch;
};

G_DEFINE_TYPE_WITH_CODE(BudgieSettingsDialog, budgie_settings_dialog, GTK_TYPE_DIALOG, G_ADD_PRIVATE(BudgieSettingsDialog))

enum SettingsColumns {
        SETTINGS_COLUMN_PATH,
        SETTINGS_N_COLUMNS
};

/* Initialisation */
static void budgie_settings_dialog_class_init(BudgieSettingsDialogClass *klass)
{
        GObjectClass *g_object_class;

        g_object_class = G_OBJECT_CLASS(klass);
        g_object_class->dispose = &budgie_settings_dialog_dispose;

        /* Set our template */
        gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(klass),
                "/com/evolve-os/budgie/media-player/preferences.ui");

        /* Map widgets */
        gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(klass), BudgieSettingsDialog, dark_switch);
        gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(klass), BudgieSettingsDialog, toolbutton_add);
        gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(klass), BudgieSettingsDialog, toolbutton_remove);
        gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(klass), BudgieSettingsDialog, treeview_dirs);

}

static void budgie_settings_dialog_init(BudgieSettingsDialog *self)
{
        GtkTreeViewColumn *column;
        GtkCellRenderer *cell;

        gtk_widget_init_template(GTK_WIDGET(self));
        self->priv = budgie_settings_dialog_get_instance_private(self);

        self->priv->settings = g_settings_new(BUDGIE_SCHEMA);

        /* Basically, Glade won't let us do this yet */
        g_object_set(self, "use-header-bar", TRUE, NULL);

        /* Bind widgets with GSettings */
        g_settings_bind(self->priv->settings, BUDGIE_DARK,
                self->priv->dark_switch, "active", G_SETTINGS_BIND_DEFAULT);

        /* Set up tree - these are ugly - swap out for a GtkListBox */
        g_signal_connect(self->priv->treeview_dirs, "cursor-changed",
                G_CALLBACK(paths_cursor_cb), self);
        cell = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Path",
                cell, "text", SETTINGS_COLUMN_PATH, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(self->priv->treeview_dirs), column);

        /* Manip buttons */
        g_signal_connect(self->priv->toolbutton_add, "clicked",
                G_CALLBACK(paths_add_cb), self);
        g_signal_connect(self->priv->toolbutton_remove, "clicked",
                G_CALLBACK(paths_remove_cb), self);

        budgie_settings_refresh(self);
}

static void budgie_settings_dialog_dispose(GObject *object)
{
        BudgieSettingsDialog *self;

        self = BUDGIE_SETTINGS_DIALOG(object);

        if (self->priv->settings) {
                g_object_unref(self->priv->settings);
                self->priv->settings = NULL;
        }

        /* Destruct */
        G_OBJECT_CLASS (budgie_settings_dialog_parent_class)->dispose (object);
}

/* Utility; return a new BudgieSettingsDialog */
GtkWidget* budgie_settings_dialog_new(void)
{
        BudgieSettingsDialog *self;

        self = g_object_new(BUDGIE_SETTINGS_DIALOG_TYPE, NULL);
        return GTK_WIDGET(self);
}

static void budgie_settings_refresh(BudgieSettingsDialog *self)
{
        gchar **media_dirs = NULL;
        gchar *path = NULL;
        gint length = 0, i;
        /* paths view */
        GtkListStore *store;
        GtkTreeIter iter;

        media_dirs = g_settings_get_strv(self->priv->settings, BUDGIE_MEDIA_DIRS);
        length = g_strv_length(media_dirs);

        store = gtk_list_store_new(SETTINGS_N_COLUMNS, G_TYPE_STRING);
        /* Add all media directories to paths view */
        for (i=0; i < length; i++) {
                path = media_dirs[i];
                gtk_list_store_append(store, &iter);
                gtk_list_store_set(store, &iter, SETTINGS_COLUMN_PATH,
                        path, -1);
        }
        gtk_tree_view_set_model(GTK_TREE_VIEW(self->priv->treeview_dirs),
                GTK_TREE_MODEL(store));

        if (media_dirs)
                g_strfreev(media_dirs);
}

static void paths_cursor_cb(GtkWidget *widget, gpointer userdata)
{
        BudgieSettingsDialog *self;
        GtkTreeSelection *selection = NULL;
        GtkTreeModel *model = NULL;
        GtkTreeIter iter;

        self = BUDGIE_SETTINGS_DIALOG(userdata);

        /* If nothing is selected, disable the remove button */
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self->priv->treeview_dirs));
        if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
                gtk_widget_set_sensitive(self->priv->toolbutton_remove, FALSE);
                return;
        }
        gtk_widget_set_sensitive(self->priv->toolbutton_remove, TRUE);
}

static void paths_add_cb(GtkWidget *widget, gpointer userdata)
{
        BudgieSettingsDialog *self;
        GtkWidget *dialog;
        GtkWidget *top;
        gchar *filename;
        /* settings */
        gchar **paths = NULL;
        GPtrArray *new_paths = NULL;
        guint length, i;

        self = BUDGIE_SETTINGS_DIALOG(userdata);
        top = gtk_widget_get_toplevel(GTK_WIDGET(self));
        dialog = gtk_file_chooser_dialog_new("Choose a directory",
                GTK_WINDOW(top), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                "Cancel", GTK_RESPONSE_REJECT,
                "Select", GTK_RESPONSE_ACCEPT, NULL);

        /* Present the dialog */
        if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_ACCEPT)
                goto end;

        /* Get paths */
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        paths = g_settings_get_strv(self->priv->settings, BUDGIE_MEDIA_DIRS);
        length = g_strv_length(paths);

        /* Make a new array to copy the original */
        new_paths = g_ptr_array_new();
        for (i=0; i < length; i++) {
                if (!g_str_equal(filename, paths[i]))
                        g_ptr_array_add(new_paths, paths[i]);
        }
        /* Copy path in */
        g_ptr_array_add(new_paths, filename);
        g_ptr_array_add(new_paths, NULL);

        /* Update GSettings with new directories */
        g_settings_set_strv(self->priv->settings, BUDGIE_MEDIA_DIRS,
                (const gchar* const*)new_paths->pdata);
        budgie_settings_refresh(self);
        g_ptr_array_free(new_paths, TRUE);

        g_free(filename);
end:
        gtk_widget_destroy(dialog);
}

static void paths_remove_cb(GtkWidget *widget, gpointer userdata)
{
        BudgieSettingsDialog *self;
        GtkTreeSelection *selection = NULL;
        GtkTreeModel *model = NULL;
        GtkTreeIter iter;
        GValue value = G_VALUE_INIT;
        const gchar *path;
        /* settings */
        gchar **paths = NULL;
        GPtrArray *new_paths = NULL;
        guint length, i;

        self = BUDGIE_SETTINGS_DIALOG(userdata);

        /* Shouldn't be a null selection. */
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self->priv->treeview_dirs));
        if (!gtk_tree_selection_get_selected(selection, &model, &iter))
                return;

        /* Retrieve old values */
        paths = g_settings_get_strv(self->priv->settings, BUDGIE_MEDIA_DIRS);
        gtk_tree_model_get_value(model, &iter, SETTINGS_COLUMN_PATH, &value);
        path = g_value_get_string(&value);
        length = g_strv_length(paths);

        /* Make a new array, with one less element */
        new_paths = g_ptr_array_new();
        /* Copy all elements except the one we're removing */
        for (i=0; i < length; i++) {
                if (!g_str_equal(path, paths[i]))
                        g_ptr_array_add(new_paths, paths[i]);
        }
        g_ptr_array_add(new_paths, NULL);

        /* Update GSettings with new directories */
        g_settings_set_strv(self->priv->settings, BUDGIE_MEDIA_DIRS,
                (const gchar* const*)new_paths->pdata);
        budgie_settings_refresh(self);
        g_ptr_array_free(new_paths, TRUE);
        g_value_unset(&value);
}
