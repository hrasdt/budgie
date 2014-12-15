/*
 * budgie-track-list.h
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
#ifndef budgie_track_list_h
#define budgie_track_list_h

#include <glib-object.h>
#include <gtk/gtk.h>

#include "db/budgie-db.h"

typedef struct _BudgieTrackList BudgieTrackList;
typedef struct _BudgieTrackListClass   BudgieTrackListClass;

#define BUDGIE_TRACK_LIST_TYPE (budgie_track_list_get_type())
#define BUDGIE_TRACK_LIST(obj)                   (G_TYPE_CHECK_INSTANCE_CAST ((obj), BUDGIE_TRACK_LIST_TYPE, BudgieTrackList))
#define IS_BUDGIE_TRACK_LIST(obj)                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUDGIE_TRACK_LIST_TYPE))
#define BUDGIE_TRACK_LIST_CLASS(klass)           (G_TYPE_CHECK_CLASS_CAST ((klass), BUDGIE_TRACK_LIST_TYPE, BudgieTrackListClass))
#define IS_BUDGIE_TRACK_LIST_CLASS(klass)        (G_TYPE_CHECK_CLASS_TYPE ((klass), BUDGIE_TRACK_LIST_TYPE))
#define BUDGIE_TRACK_LIST_GET_CLASS(obj)         (G_TYPE_INSTANCE_GET_CLASS ((obj), BUDGIE_TRACK_LIST_TYPE, BudgieTrackListClass))


/* BudgieTrackList object */
struct _BudgieTrackList {
        GtkBin parent;

        GtkWidget *image;
        GtkWidget *current_label;
        GtkWidget *count_label;
        GtkWidget *list;

        /* We'll copy stuff from the BudgieDB into this. Ultimately, BudgieDB should probably subclass GtkTreeModel */
        GtkListStore *store;
};

/* We want to be able to update the currently-playing track. */
enum {
        BUDGIE_NOT_PLAYING=0,
        BUDGIE_PLAYING=1,
        BUDGIE_PAUSED,
};

enum {
        BUDGIE_TRACK_LIST_DB_TITLE = 0,
        BUDGIE_TRACK_LIST_DB_TRACK,
        BUDGIE_TRACK_LIST_DB_ARTIST,
        BUDGIE_TRACK_LIST_DB_ALBUM,
        BUDGIE_TRACK_LIST_DB_BAND,
        BUDGIE_TRACK_LIST_DB_GENRE,
        BUDGIE_TRACK_LIST_DB_PATH,
        BUDGIE_TRACK_LIST_DB_MIME,
        BUDGIE_TRACK_LIST_DB_INFO,
        BUDGIE_TRACK_LIST_DB_PLAYING,

        BUDGIE_TRACK_LIST_DB_NUM_FIELDS
};

/* BudgieTrackList class definition */
struct _BudgieTrackListClass {
        GtkBinClass parent_class;
};

GType budgie_track_list_get_type(void);

/* BudgieControlBar methods */
GtkWidget* budgie_track_list_new(void);

void budgie_track_list_update_playing(BudgieTrackList *list, MediaInfo *now_playing);

#endif /* budgie_track_list_h */
