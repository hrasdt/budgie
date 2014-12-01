/*
 * budgie-media-view.h
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
#ifndef budgie_media_view_h
#define budgie_media_view_h

#include <glib-object.h>
#include <gtk/gtk.h>

#include "db/budgie-db.h"
#include "budgie-track-list.h"

typedef struct _BudgieMediaView BudgieMediaView;
typedef struct _BudgieMediaViewClass   BudgieMediaViewClass;

#define BUDGIE_MEDIA_VIEW_TYPE (budgie_media_view_get_type())
#define BUDGIE_MEDIA_VIEW(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), BUDGIE_MEDIA_VIEW_TYPE, BudgieMediaView))
#define IS_BUDGIE_MEDIA_VIEW(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BUDGIE_MEDIA_VIEW_TYPE))
#define BUDGIE_MEDIA_VIEW_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), BUDGIE_MEDIA_VIEW_TYPE, BudgieMediaViewClass))
#define IS_BUDGIE_MEDIA_VIEW_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), BUDGIE_MEDIA_VIEW_TYPE))
#define BUDGIE_MEDIA_VIEW_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), BUDGIE_MEDIA_VIEW_TYPE, BudgieMediaViewClass))

typedef enum {
        MEDIA_MODE_ALBUMS = 0,
        MEDIA_MODE_SONGS,
        MEDIA_MODE_VIDEOS,
} BudgieMediaMode;

typedef enum {
        MEDIA_SELECTION_PREVIOUS = 0,
        MEDIA_SELECTION_CURRENT,
        MEDIA_SELECTION_NEXT,
        MEDIA_SELECTION_RANDOM,
} BudgieMediaSelection;

/* BudgieMediaView object */
struct _BudgieMediaView {
        GtkBin parent;
        BudgieDB *db;

        GtkWidget *stack;

        GtkWidget *icon_view;

        /* Current results */
        GPtrArray *results;

        /* Selection mode */
        BudgieMediaMode mode;
        GtkWidget *albums;
        GtkWidget *songs;
        GtkWidget *videos;

        /* Track pages */
        GtkWidget *album_tracks;
        GtkWidget *song_tracks;
        GtkWidget *video_tracks;

        gchar *current_path;
        gint index;
};

/* BudgieMediaView class definition */
struct _BudgieMediaViewClass {
        GtkBinClass parent_class;
};

GType budgie_media_view_get_type(void);

/* BudgieMediaView methods */

/**
 * Construct a new BudgieMediaView
 * @param database Database to use
 * @return A new BudgieMediaView
 */
GtkWidget* budgie_media_view_new(BudgieDB *database);

/**
 * Get a media info from the current view
 * @param select Selection mode
 * @return The specific media info, or NULL
 */
MediaInfo* budgie_media_view_get_info(BudgieMediaView *self,
                                      BudgieMediaSelection select);

/**
 * Set the currently active media
 * @param info Currently active media
 */
void budgie_media_view_set_active(BudgieMediaView *self,
                                  MediaInfo *active);

#endif /* budgie_media_view_h */
