/*
 * budgie-settings-dialog.h
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
#ifndef budgie_settings_dialog_h
#define budgie_settings_dialog_h

#include <glib-object.h>
#include <gtk/gtk.h>

typedef struct _BudgieSettingsDialog BudgieSettingsDialog;
typedef struct _BudgieSettingsDialogClass   BudgieSettingsDialogClass;
typedef struct _BudgieSettingsDialogPriv BudgieSettingsDialogPrivate;

#define BUDGIE_SETTINGS_DIALOG_TYPE (budgie_settings_dialog_get_type())
#define BUDGIE_SETTINGS_DIALOG(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), BUDGIE_SETTINGS_DIALOG_TYPE, BudgieSettingsDialog))
#define IS_BUDGIE_SETTINGS_DIALOG(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUDGIE_SETTINGS_DIALOG_TYPE))
#define BUDGIE_SETTINGS_DIALOG_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), BUDGIE_SETTINGS_DIALOG_TYPE, BudgieSettingsDialogClass))
#define IS_BUDGIE_SETTINGS_DIALOG_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), BUDGIE_SETTINGS_DIALOG_TYPE))
#define BUDGIE_SETTINGS_DIALOG_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), BUDGIE_SETTINGS_DIALOG_TYPE, BudgieSettingsDialogClass))

/* BudgieSettingsDialog object */
struct _BudgieSettingsDialog {
        GtkDialog parent;
        BudgieSettingsDialogPrivate *priv;
};

/* BudgieSettingsDialog class definition */
struct _BudgieSettingsDialogClass {
        GtkDialogClass parent_class;
};

GType budgie_settings_dialog_get_type(void);

/* BudgieSettingsDialog methods */
GtkWidget* budgie_settings_dialog_new(void);

#endif /* budgie_settings_dialog_h */
