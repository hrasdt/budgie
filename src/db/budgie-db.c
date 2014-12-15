/*
 * budgie-db.c
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
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include "budgie-db.h"

/* Private storage */
struct _BudgieDBPrivate {
        gchar *storage_path;
        sqlite3 *db;
        char *zErrMesg;
};

G_DEFINE_TYPE_WITH_PRIVATE(BudgieDB, budgie_db, G_TYPE_OBJECT)

/* Boilerplate GObject code */
static void budgie_db_class_init(BudgieDBClass *klass);
static void budgie_db_init(BudgieDB *self);
static void budgie_db_dispose(GObject *object);

/* Used to keep the database thread safe */
static GMutex _lock;

/* Utility functions */
static gboolean _db_create(BudgieDB *self);
static gchar* _sanitize_value(gchar *val);

/* MediaInfo API */
MediaInfo* new_media_info(sqlite3_stmt *stmt)
{
        MediaInfo *ret;

        ret = malloc(sizeof(MediaInfo));
        if (!ret){
                g_error("Couldn't allocate memory for a new record!");
                return NULL;
        }

        ret->id = sqlite3_column_int(stmt, BUDGIE_DB_COLUMN_ID);

        ret->title = g_strdup((gchar*)
                              sqlite3_column_text(stmt,
                                                  BUDGIE_DB_COLUMN_TITLE));

        ret->track_no = sqlite3_column_int(stmt, BUDGIE_DB_COLUMN_TRACK);

        ret->artist = g_strdup((gchar *)
                               sqlite3_column_text(stmt,
                                                   BUDGIE_DB_COLUMN_ARTIST));

        ret->album = g_strdup((gchar *)
                              sqlite3_column_text(stmt,
                                                  BUDGIE_DB_COLUMN_ALBUM));

        ret->band = g_strdup((gchar *)
                             sqlite3_column_text(stmt,
                                                 BUDGIE_DB_COLUMN_BAND));

        ret->genre = g_strdup((gchar *)
                              sqlite3_column_text(stmt,
                                                  BUDGIE_DB_COLUMN_GENRE));

        ret->path = g_strdup((gchar *)
                             sqlite3_column_text(stmt,
                                                 BUDGIE_DB_COLUMN_PATH));

        ret->mime = g_strdup((gchar *)
                             sqlite3_column_text(stmt,
                                                 BUDGIE_DB_COLUMN_MIME));

        return ret;
}

void free_media_info(gpointer p_info)
{
        MediaInfo *info;

        info = (MediaInfo*)p_info;
        if (info->title) {
                g_free(info->title);
        }
        if (info->artist) {
                g_free(info->artist);
        }
        if (info->album) {
                g_free(info->album);
        }
        if (info->band) {
                g_free(info->band);
        }
        if (info->genre) {
                g_free(info->genre);
        }
        if (info->path) {
                g_free(info->path);
        }
        if (info->mime) {
                g_free(info->mime);
        }
}

/* Initialisation */
static void budgie_db_class_init(BudgieDBClass *klass)
{
        GObjectClass *g_object_class;

        g_object_class = G_OBJECT_CLASS(klass);
        g_object_class->dispose = &budgie_db_dispose;
}

static void budgie_db_init(BudgieDB *self)
{
        const gchar *config;
        gint stat;
        self->priv = budgie_db_get_instance_private(self);

        /* Our storage location */
        config = g_get_user_config_dir();
        self->priv->storage_path = g_strdup_printf("%s/%s", config,
                CONFIG_NAME);

        /* Open the database */
        stat = sqlite3_open(self->priv->storage_path, &self->priv->db);

        if (stat != SQLITE_OK) {
                g_error("Failed to open the database! %s", sqlite3_errmsg(self->priv->db));
                sqlite3_close(self->priv->db);
        }

        /* Create a database if necessary */
        if (!_db_create(self)){
          g_error("Failed to initialise the database!");
          sqlite3_close(self->priv->db);
        }
}

static gchar* _sanitize_value(gchar *val){
        gint i;
        gchar *res, *c;

        if (val == NULL){
                res = g_strdup("");
                return res;
        }

        /* Lazy implementation. */
        res = g_strnfill(strlen(val) * 2, '\0');
        i = 0;
        for (c = val; *c != '\0'; c++){
                if (*c == '\''){
                        res[i++] = '\'';
                }
                res[i++] = *c;
        }

        return res;
}

static gboolean _db_create(BudgieDB *self){
        int stat;
        gchar *sql = g_strdup(""
                "CREATE TABLE IF NOT EXISTS items("
                "ID INTEGER PRIMARY KEY NOT NULL,"
                "title TEXT NOT NULL,"
                "track INTEGER NOT NULL,"
                "artist TEXT,"
                "album TEXT,"
                "band TEXT,"
                "genre TEXT,"
                "path TEXT NOT NULL UNIQUE,"
                "mimetype TEXT NOT NULL"
                ");");

        stat = sqlite3_exec(self->priv->db, sql,
                            NULL, NULL, &self->priv->zErrMesg);
        g_free(sql);
        if (stat != SQLITE_OK && stat != SQLITE_DONE){
                g_error("An SQL error occured while creating the table: %s",
                        self->priv->zErrMesg);
                return FALSE;
        }

        return TRUE;
}

static void budgie_db_dispose(GObject *object)
{
        BudgieDB *self;

        self = BUDGIE_DB(object);
        if (self->priv->storage_path) {
                g_free(self->priv->storage_path);
                self->priv->storage_path = NULL;
        }

        sqlite3_close(self->priv->db);

        /* Destruct */
        G_OBJECT_CLASS(budgie_db_parent_class)->dispose(object);
}

/* Utility; return a new BudgieDB */
BudgieDB* budgie_db_new(void)
{
        BudgieDB *self;

        self = g_object_new(BUDGIE_DB_TYPE, NULL);
        return BUDGIE_DB(self);
}

gboolean budgie_db_update(BudgieDB *self, GSList *tracks)
{
        GSList *ref;
        MediaInfo *info;
        sqlite3_stmt *stmt;
        gint stat;
        gint c;

        g_return_val_if_fail(self != NULL, FALSE);

        gchar *sql = g_strdup(""
                "insert or replace "
                "into items(ID, title, track, artist, album, "
                "           band, genre, path, mimetype) "
                "values ( (select id from items where path == ?), "
                              "         ?, ?, ?, ?, ?, ?, ?, ?);");

        g_mutex_lock(&_lock);

        /* BEGIN */
        stat = sqlite3_exec(self->priv->db, "BEGIN",
                NULL, NULL, &self->priv->zErrMesg);
        if (stat != SQLITE_OK) {
                g_error("SQL error: %d", stat);
                g_error("Further info: %s", self->priv->zErrMesg);

                g_free(sql);
                g_mutex_unlock(&_lock);
                return FALSE;
        }

        stat = sqlite3_prepare_v2(self->priv->db, (const char *) sql, -1,
                &stmt, NULL);
        if (stat != SQLITE_OK){
                g_error("SQL error: %d", stat);
                g_error("Failed to update the database!");

                sqlite3_finalize(stmt);
                g_free(sql);
                g_mutex_unlock(&_lock);
                return FALSE;
        }

        /* Now, go through the list and add all these new things. */
        c = 0;
        for (ref = tracks; ref != NULL; ref = g_slist_next(ref)) {
                c++;
                info = (MediaInfo*) ref->data;
                sqlite3_reset(stmt);

                /* Bind the values. */
                stat = sqlite3_bind_text(stmt, 1, info->path, -1, NULL);
                stat = sqlite3_bind_text(stmt, 2, info->title, -1, NULL);
                stat = sqlite3_bind_int(stmt, 3, info->track_no);
                stat = sqlite3_bind_text(stmt, 4, info->artist, -1, NULL);
                stat = sqlite3_bind_text(stmt, 5, info->album, -1, NULL);
                stat = sqlite3_bind_text(stmt, 6, info->band, -1, NULL);
                stat = sqlite3_bind_text(stmt, 7, info->genre, -1, NULL);
                stat = sqlite3_bind_text(stmt, 8, info->path, -1, NULL);
                stat = sqlite3_bind_text(stmt, 9, info->mime, -1, NULL);

                do {
                        stat = sqlite3_step(stmt);
                } while (stat == SQLITE_ROW);

                if (stat != SQLITE_DONE){
                        g_warning("SQL failed to add an item: %d", stat);
                }
        }

        /* END */
        stat = sqlite3_exec(self->priv->db, "COMMIT",
                NULL, NULL, &self->priv->zErrMesg);

        g_message("Added %d tracks\n", c);

        /* Wrap up */
        g_free(sql);
        sqlite3_finalize(stmt);
        g_mutex_unlock(&_lock);

        return TRUE;
}

MediaInfo* budgie_db_get_media(BudgieDB *self, gchar *path)
{
        MediaInfo *ret;
        gchar *s_path;
        gchar *sql;
        sqlite3_stmt *stmt = NULL;
        int stat;

        s_path = _sanitize_value(path);
        ret = malloc(sizeof(MediaInfo));
        if (!ret) {
                goto end;
        }

        sql = g_strdup_printf("SELECT * FROM items WHERE path == '%s'",
                       s_path);

        g_mutex_lock(&_lock);

        stat = sqlite3_prepare_v2(self->priv->db, (const char *)sql,
                -1, &stmt, NULL);
        stat = sqlite3_step(stmt);
        if (stat == SQLITE_DONE){
                /* Ran out of things :( */
                goto end;
        }
        else if (stat == SQLITE_ERROR){
                g_error("SQL error: %s", sqlite3_errmsg(self->priv->db));
                goto end;
        }
        else if (stat == SQLITE_ROW){
                /* We have data to process */
                ret = new_media_info(stmt);
        }

end:
        if (stmt){
                sqlite3_finalize(stmt);
        }

        g_mutex_unlock(&_lock);
        g_free(s_path);

        return ret;
}

GSList* budgie_db_get_all_media(BudgieDB* self)
{
        GSList *ret = NULL;
        MediaInfo *info;
        gchar *sql;
        sqlite3_stmt *stmt = NULL;
        int stat;

        g_mutex_lock(&_lock);

        sql = g_strdup("SELECT * FROM items ORDER BY track ASC, id ASC");

        /* Iterate until we run out of things */
        stat = sqlite3_prepare_v2(self->priv->db, (const char *)sql,
                -1, &stmt, NULL);
        stat = sqlite3_step(stmt);
        while (stat == SQLITE_ROW){
                /* We have data to process */
                info = new_media_info(stmt);

                ret = g_slist_append(ret, info);

                /* Continue. */
                stat = sqlite3_step(stmt);
        }

        if (stat == SQLITE_ERROR){
                g_error("SQL error: %s", sqlite3_errmsg(self->priv->db));
        }

        /* Wrap up */
        if (stmt){
                sqlite3_finalize(stmt);
        }

        g_mutex_unlock(&_lock);
        g_free(sql);

        return ret;
}

gboolean budgie_db_get_all_by_field(BudgieDB *self,
                                    MediaQuery query,
                                    GPtrArray **results)
{
        g_assert(query >= 0 && query < MEDIA_QUERY_MAX);

        GPtrArray *_results = NULL;

        sqlite3_stmt *stmt;
        gchar *sql;
        gint stat;

        gchar *append;

        g_mutex_lock(&_lock);
        _results = g_ptr_array_new();

        switch (query) {
                case MEDIA_QUERY_TITLE:
                        sql = g_strdup("SELECT DISTINCT title FROM items ORDER BY track ASC, id ASC;");
                        break;
                case MEDIA_QUERY_ALBUM:
                        sql = g_strdup("SELECT DISTINCT album FROM items ORDER BY track ASC, id ASC;");
                        break;
                case MEDIA_QUERY_ARTIST:
                        sql = g_strdup("SELECT DISTINCT artist FROM items ORDER BY track ASC, id ASC;");
                        break;
                case MEDIA_QUERY_GENRE:
                        sql = g_strdup("SELECT DISTINCT genre FROM items ORDER BY track ASC, id ASC;");
                        break;
                case MEDIA_QUERY_MIME:
                        sql = g_strdup("SELECT DISTINCT mime FROM items ORDER BY track ASC, id ASC;");
                        break;
                default:
                        sql = g_strdup("SELECT DISTINCT path FROM items ORDER BY track ASC, id ASC;");
        }

        stat = sqlite3_prepare_v2(self->priv->db, (const char *)sql,
                -1, &stmt, NULL);

        if (stat != SQLITE_OK) {
                g_warning("SQL error: %d", stat);
        }
        stat = sqlite3_step(stmt);
        while (stat == SQLITE_ROW) {
                /* We have data to process */
                append = g_strdup( (const gchar*) sqlite3_column_text(stmt, 0));
                if (append == NULL || append[0] == '\0'){
                        /* Don't append it */
                        g_free(append);
                }
                else {
                        g_ptr_array_add(_results, append);
                }

                /* Continue. */
                stat = sqlite3_step(stmt);
        }
        if (stat == SQLITE_ERROR) {
                g_error("SQL error: %s", sqlite3_errmsg(self->priv->db));
        }

        /* Wrap up */
        if (stmt){
                sqlite3_finalize(stmt);
        }
        g_mutex_unlock(&_lock);
        g_free(sql);

        /* No results */
        if (_results->len < 1) {
                g_ptr_array_free(_results, TRUE);
                return FALSE;
        }

        *results = _results;
        return TRUE;
}

gboolean budgie_db_search_field(BudgieDB *self,
                                MediaQuery query,
                                MatchQuery match,
                                gchar *term,
                                guint max,
                                GPtrArray **results)
{
        g_assert(query >= 0 && query < MEDIA_QUERY_MAX);
        g_assert(match >= 0 && match < MATCH_QUERY_MAX);
        g_assert(term != NULL);

        GPtrArray *_results = NULL;
        MediaInfo *info, *cmp;

        sqlite3_stmt *stmt;
        gchar *sql, *like_match, *s_term, *test, *limit;
        gint stat;

        gint i;
        gboolean should_append;

        /* Ensure we're not null */
        g_return_val_if_fail(self != NULL, FALSE);

        _results = g_ptr_array_new();

        s_term = _sanitize_value(term);

        if (max == -1){
                limit = g_strdup("");
        }
        else {
                limit = g_strdup_printf(" LIMIT %d", max);
        }

        switch (match) {
                case MATCH_QUERY_START:
                        like_match = g_strdup_printf("LIKE '%s%%'", s_term);
                        break;
                case MATCH_QUERY_END:
                        like_match = g_strdup_printf("LIKE '%%%s'", s_term);
                        break;
                case MATCH_QUERY_EXACT:
                        like_match = g_strdup_printf("== '%s'", s_term);
                        break;
                case MATCH_QUERY_ANYWHERE:
                default:
                        like_match = g_strdup_printf("LIKE '%%%s%%'", s_term);
        }

        switch (query) {
                case MEDIA_QUERY_TITLE:
                        test = g_strdup("title");
                        break;
                case MEDIA_QUERY_ALBUM:
                        test = g_strdup("album");
                        break;
                case MEDIA_QUERY_ARTIST:
                        test = g_strdup("artist");
                        break;
                case MEDIA_QUERY_GENRE:
                        test = g_strdup("genre");
                        break;
                case MEDIA_QUERY_MIME:
                        test = g_strdup("mimetype");
                        break;
                default:
                        g_warning("Invalid query '%d'", query);
                        g_free(like_match);
                        g_free(s_term);
                        g_free(_results);
                        return FALSE;
        }

        sql = g_strdup_printf("SELECT * FROM items WHERE %s %s ORDER BY track ASC, id ASC %s;",
                test, like_match, limit);

        g_free(test);
        g_free(like_match);
        g_free(limit);
        g_free(s_term);

        g_mutex_lock(&_lock);
        stat = sqlite3_prepare_v2(self->priv->db, (const char *)sql,
                -1, &stmt, NULL);
        if (stat != SQLITE_OK) {
                g_error("Failed to prepare SQL statement");
                sqlite3_finalize(stmt);
                g_mutex_unlock(&_lock);
                g_free(sql);
                g_ptr_array_free(_results, TRUE);
                return FALSE;
        }

        stat = sqlite3_step(stmt);
        while (stat == SQLITE_ROW) {
                should_append = TRUE;

                /* We have data to process */
                info = new_media_info(stmt);

                if (info == NULL){
                        should_append = FALSE;
                }
                else {
                        for (i = 0; i < _results->len; i++){
                                cmp = (MediaInfo*)_results->pdata[i];
                                if (g_str_equal(cmp->path, info->path)){
                                        should_append = FALSE;
                                }
                        }
                }

                if (should_append){
                        g_ptr_array_add(_results, info);
                }

                /* Continue. */
                stat = sqlite3_step(stmt);
        }
        if (stat == SQLITE_ERROR) {
                g_error("SQL error: %s", sqlite3_errmsg(self->priv->db));
        }

        /* Wrap up */
        sqlite3_finalize(stmt);
        g_mutex_unlock(&_lock);
        g_free(sql);

        /* No results */
        if (_results->len < 1) {
                g_ptr_array_free(_results, TRUE);
                return FALSE;
        }

        *results = _results;
        return TRUE;
}

/** PRIVATE **/
gint budgie_db_sort(gconstpointer a, gconstpointer b)
{
        MediaInfo* m1 = NULL;
        MediaInfo* m2 = NULL;

        /* Alphabetically compare */
        m1 = *(MediaInfo**)a;
        m2 = *(MediaInfo**)b;

        if (!m1 || !m2) {
                return 0;
        }

        return g_ascii_strcasecmp(m1->title, m2->title);
}
