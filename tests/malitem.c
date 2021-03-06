/*
 *  This file is part of mal-gtk.
 *
 *  mal-gtk is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  mal-gtk is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with mal-gtk.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <libxml/encoding.h>
#include "malgtk_malitem.h"
#include "malgtk_date.h"

typedef struct {
    MalgtkMalitem *item;
    MalgtkDate begin;
    MalgtkDate end;
} MalitemFixture;

static void
malitem_fixture_set_up (MalitemFixture *fixture,
                        gconstpointer user_data)
{
    fixture->item = malgtk_malitem_new ();
    malgtk_date_clear(&fixture->begin);
    malgtk_date_clear(&fixture->end);
}


static void
malitem_fixture_tear_down (MalitemFixture *fixture,
                           gconstpointer user_data)
{
    g_clear_object (&fixture->item);
}

static void
test_malitem_test1 (MalitemFixture *fixture,
                    gconstpointer user_data)
{
    g_autofree gchar *begin = NULL;
    g_autofree gchar *end   = NULL;

    malgtk_date_set_dmy(&fixture->begin, 1, 1, 2012);
    malgtk_date_set_dmy(&fixture->end, 1, 6, 2012);
    g_object_set(fixture->item, "series-date-begin", &fixture->begin, "series-date-end", &fixture->end, NULL);
    g_object_get(fixture->item, "season-begin", &begin, "season-end", &end, NULL);
    g_assert_cmpstr (begin, ==, "Winter 2012");
    g_assert_cmpstr (end, ==, "Summer 2012");

    malgtk_date_set_dmy(&fixture->begin, 0, 1, 2012);
    malgtk_date_set_dmy(&fixture->end, 0, 0, 2012);
    g_object_set(fixture->item, "series-date-begin", &fixture->begin, "series-date-end", &fixture->end, NULL);
    g_object_get(fixture->item, "season-begin", &begin, "season-end", &end, NULL);
    g_assert_cmpstr (begin, ==, "Winter 2012");
    g_assert_cmpstr (end, ==, "2012");

    malgtk_date_set_dmy(&fixture->begin, 0, 0, 0);
    malgtk_date_clear(&fixture->end);
    g_object_set(fixture->item, "series-date-begin", &fixture->begin, "series-date-end", &fixture->end, NULL);
    g_object_get(fixture->item, "season-begin", &begin, "season-end", &end, NULL);
    g_assert_cmpstr (begin, ==, "Unknown");
    g_assert_cmpstr (end, ==, "Unknown");
}

static void
test_malitem_test2 (MalitemFixture *fixture,
                    gconstpointer user_data)
{
    MalgtkMalitem *item = fixture->item;
    malgtk_malitem_add_synonym(item, "First");
    malgtk_malitem_add_synonym(item, "Second");
    malgtk_malitem_add_synonym(item, "Second");
    malgtk_malitem_add_synonym(item, "Another");

    g_auto(GStrv) strv = NULL;
    g_object_get(item, "series-synonyms", &strv, NULL);
    g_assert_cmpstr (strv[0], ==, "Another");
    g_assert_cmpstr (strv[1], ==, "First");
    g_assert_cmpstr (strv[2], ==, "Second");
    g_assert_null (strv[3]);
}

static void
test_malitem_test3 (MalitemFixture *fixture,
                    gconstpointer user_data)
{
    MalgtkMalitem *item = fixture->item;
    g_auto(GStrv) strv = g_malloc(sizeof(gchar*)*4);
    strv[0] = g_strdup("Bee");
    strv[1] = g_strdup("Apple");
    strv[2] = g_strdup("Apple");
    strv[3] = NULL;

    g_object_set(item, "series-synonyms", strv, NULL);

    g_strfreev(strv);
    strv = NULL;

    g_object_get(item, "series-synonyms", &strv, NULL);
    g_assert_cmpstr (strv[0], ==, "Apple");
    g_assert_cmpstr (strv[1], ==, "Bee");
    g_assert_null   (strv[2]);
}

static void
test_malitem_test4 (MalitemFixture *fixture,
                    gconstpointer user_data)
{
    MalgtkMalitem              *item               = fixture->item;
    gint64                      mal_db_id          = g_test_rand_int();
    gchar                      *series_title       = "Test Series";
    gchar                      *preferred_title    = "My Preferred Test Series Title";
    MalgtkDate                  series_begin;
    MalgtkDate                  series_end;
    gchar                      *image_url          = "http://i.myanimelist.net/g/23522342.jpg";
    gchar                      *synonyms[]         = { "SynOne", "SynTwo", NULL };
    gchar                      *synopsis           = "A simple synopsis of the Test Series";
    gchar                      *tags[]             = { "TagOne", "TagTwo", "TagThree", "TagFour", NULL };
    GDate                       date_start;
    GDate                       date_finish;
    gint64                      id                 = g_test_rand_int();
    g_autoptr(GDateTime)        last_updated       = g_date_time_new_from_unix_utc(g_test_rand_int_range(G_MININT, G_MAXINT));
    double                      score              = g_test_rand_double_range(0.0, 10.0);
    gboolean                    enable_reconsuming = g_test_rand_bit();
    gchar                      *fansub_group       = "Nutbladder";
    gchar                      *comments           = "My favorite ennui series";
    gint                        downloaded_items   = g_test_rand_int_range(0, G_MAXINT);
    gint                        times_consumed     = g_test_rand_int_range(0, G_MAXINT);
    MalgtkMalitemReconsumeValue reconsume_value    = MALGTK_MALITEM_RECONSUME_VALUE_MEDIUM;
    MalgtkMalitemPriority       priority           = MALGTK_MALITEM_PRIORITY_HIGH;
    gboolean                    enable_discussion  = g_test_rand_bit();
    gboolean                    has_details        = g_test_rand_bit();
    malgtk_date_set_from_string (&series_begin, "2014-02-01");
    malgtk_date_set_from_string (&series_end, "2015");
    g_date_set_dmy (&date_start, 3, 1, 2014);
    g_date_set_dmy (&date_finish, 6, 1, 2015);

    g_object_set (G_OBJECT(item),
                  "mal-db-id",           mal_db_id,
                  "series-title",        series_title,
                  "preferred-title",     preferred_title,
                  "series-date-begin",  &series_begin,
                  "series-date-end",    &series_end,
                  "image-url",           image_url,
                  "series-synonyms",     synonyms,
                  "series-synopsis",     synopsis,
                  "tags",                tags,
                  "date-start",         &date_start,
                  "date-finish",        &date_finish,
                  "id",                  id,
                  "last-updated",        last_updated,
                  "score",               score,
                  "enable-reconsuming",  enable_reconsuming,
                  "fansub-group",        fansub_group,
                  "comments",            comments,
                  "downloaded-items",    downloaded_items,
                  "times-consumed",      times_consumed,
                  "reconsume-value",     reconsume_value,
                  "priority",            priority,
                  "enable-discussion",   enable_discussion,
                  "has-details",         has_details,
                  NULL);

    gint64                       _mal_db_id;
    g_autofree gchar            *_series_title    = NULL;
    g_autofree gchar            *_preferred_title = NULL;
    g_autoptr(MalgtkDate)        _series_begin    = NULL;
    g_autoptr(MalgtkDate)        _series_end      = NULL;
    g_autofree gchar            *_season_begin    = NULL;
    g_autofree gchar            *_season_end      = NULL;
    g_autofree gchar            *_image_url       = NULL;
    g_auto (GStrv)               _synonyms        = NULL;
    g_autofree gchar            *_synopsis        = NULL;
    g_auto (GStrv)               _tags            = NULL;
    GDate                       *_date_start      = NULL;
    GDate                       *_date_finish     = NULL;
    gint64                       _id;
    g_autoptr(GDateTime)         _last_updated;
    double                       _score;
    gboolean                     _enable_reconsuming;
    g_autofree gchar            *_fansub_group    = NULL;
    g_autofree gchar            *_comments        = NULL;
    gint                         _downloaded_items;
    gint                         _times_consumed;
    MalgtkMalitemReconsumeValue  _reconsume_value;
    MalgtkMalitemPriority        _priority;
    gboolean                     _enable_discussion;
    gboolean                     _has_details;

    g_object_get (G_OBJECT(item),
                  "mal-db-id",          &_mal_db_id,
                  "series-title",       &_series_title,
                  "preferred-title",    &_preferred_title,
                  "series-date-begin",  &_series_begin,
                  "series-date-end",    &_series_end,
                  "season-begin",       &_season_begin,
                  "season-end",         &_season_end,
                  "image-url",          &_image_url,
                  "series-synonyms",    &_synonyms,
                  "series-synopsis",    &_synopsis,
                  "tags",               &_tags,
                  "date-start",         &_date_start,
                  "date-finish",        &_date_finish,
                  "id",                 &_id,
                  "last-updated",       &_last_updated,
                  "score",              &_score,
                  "enable-reconsuming", &_enable_reconsuming,
                  "fansub-group",       &_fansub_group,
                  "comments",           &_comments,
                  "downloaded-items",   &_downloaded_items,
                  "times-consumed",     &_times_consumed,
                  "reconsume-value",    &_reconsume_value,
                  "priority",           &_priority,
                  "enable-discussion",  &_enable_discussion,
                  "has-details",        &_has_details,
                  NULL);

    g_assert_cmpint   (mal_db_id,          ==, _mal_db_id);
    g_assert_cmpstr   (series_title,       ==, _series_title);
    g_assert_cmpstr   (preferred_title,    ==, _preferred_title);
    g_assert_true     (malgtk_date_is_equal (&series_begin, _series_begin));
    g_assert_true     (malgtk_date_is_equal (&series_end, _series_end));
    g_assert_cmpstr   (_season_begin,      ==, "Winter 2014");
    g_assert_cmpstr   (_season_end,        ==, "2015");
    g_assert_cmpstr   (image_url,          ==, _image_url);
    g_assert_cmpstr   (synonyms[0],        ==, _synonyms[0]);
    g_assert_cmpstr   (synonyms[1],        ==, _synonyms[1]);
    g_assert_null     (_synonyms[2]);
    g_assert_cmpstr   (synopsis,           ==, _synopsis);
    g_assert_cmpstr   (tags[3],            ==, _tags[0]);
    g_assert_cmpstr   (tags[0],            ==, _tags[1]);
    g_assert_cmpstr   (tags[2],            ==, _tags[2]);
    g_assert_cmpstr   (tags[1],            ==, _tags[3]);
    g_assert_null     (_tags[4]);
    g_assert_true     (g_date_compare(&date_start, _date_start) == 0);
    g_assert_true     (g_date_compare(&date_finish, _date_finish) == 0);
    g_assert_cmpint   (id,                 ==, _id);
    g_assert_true     (g_date_time_compare(last_updated, _last_updated) == 0);
    g_assert_cmpfloat (score,              ==, _score);
    g_assert_cmpint   (enable_reconsuming, ==, _enable_reconsuming);
    g_assert_cmpstr   (fansub_group,       ==, _fansub_group);
    g_assert_cmpstr   (comments,           ==, _comments);
    g_assert_cmpint   (downloaded_items,   ==, _downloaded_items);
    g_assert_cmpint   (times_consumed,     ==, _times_consumed);
    g_assert_cmpint   (reconsume_value,    ==, _reconsume_value);
    g_assert_cmpint   (priority,           ==, _priority);
    g_assert_cmpint   (enable_discussion,  ==, _enable_discussion);
    g_assert_cmpint   (has_details,        ==, _has_details);


    g_date_free (_date_start);
    g_date_free (_date_finish);
}

static void
test_malitem_xmlset (MalitemFixture *fixture,
                     gconstpointer user_data)
{
    static const char xml[] = "<MALitem version=\"1\"><series_itemdb_id>1</series_itemdb_id><series_title>Cowboy Bebop</series_title><series_preferred_title></series_preferred_title><series_date_begin>1998-04-03</series_date_begin><series_date_end>1999-04-24</series_date_end><image_url>http://cdn.myanimelist.net/images/anime/4/19644.jpg</image_url><series_synonyms><series_synonym>Cowboy Bebop</series_synonym></series_synonyms><series_synopsis></series_synopsis><tags/><date_start>0000-00-00</date_start><date_finish>0000-00-00</date_finish><id>9755128</id><last_updated>1238650198</last_updated><score>0.000000</score><enable_reconsuming>0</enable_reconsuming><fansub_group></fansub_group><comments></comments><downloaded_items>0</downloaded_items><times_consumed>0</times_consumed><reconsume_value>Invalid Reconsume Value</reconsume_value><priority>Invalid Priority</priority><enable_discussion>0</enable_discussion><has_details>0</has_details></MALitem>";
    MalgtkMalitem   *item   = fixture->item;
    xmlTextReaderPtr reader = xmlReaderForMemory(xml, G_N_ELEMENTS(xml) - 1, NULL, NULL, 0);
    xmlTextReaderRead(reader);
    malgtk_malitem_set_from_xml(item, reader);

    gint64                       mal_db_id          = 1;
    gchar                       *series_title       = "Cowboy Bebop";
    MalgtkDate                   series_begin;
    MalgtkDate                   series_end;
    gchar                       *image_url          = "http://cdn.myanimelist.net/images/anime/4/19644.jpg";
    gchar                       *synonyms[]         = { "Cowboy Bebop", NULL };
    gchar                       *tags[]             = { NULL };
    GDate                        date_start;
    GDate                        date_finish;
    gint64                       id                 = 9755128;
    g_autoptr(GDateTime)         last_updated       = g_date_time_new_from_unix_utc(1238650198);
    double                       score              = 0.0;
    gboolean                     enable_reconsuming = FALSE;
    gint                         downloaded_items   = 0;
    gint                         times_consumed     = 0;
    MalgtkMalitemReconsumeValue  reconsume_value    = MALGTK_MALITEM_RECONSUME_VALUE_INVALID;
    MalgtkMalitemPriority        priority           = MALGTK_MALITEM_PRIORITY_INVALID;
    gboolean                     enable_discussion  = FALSE;
    gboolean                     has_details        = FALSE;
    malgtk_date_set_from_string (&series_begin, "1998-04-03");
    malgtk_date_set_from_string (&series_end, "1999-04-24");
    g_date_clear (&date_start, 1);
    g_date_clear (&date_finish, 1);

    gint64                       _mal_db_id;
    g_autofree gchar             *_series_title = NULL;
    g_autoptr(MalgtkDate)        _series_begin  = NULL;
    g_autoptr(MalgtkDate)        _series_end    = NULL;
    g_autofree gchar             *_season_begin = NULL;
    g_autofree gchar             *_season_end   = NULL;
    g_autofree gchar             *_image_url    = NULL;
    g_auto (GStrv)               _synonyms      = NULL;
    g_auto (GStrv)               _tags          = NULL;
    GDate                        *_date_start   = NULL;
    GDate                        *_date_finish  = NULL;
    gint64                       _id;
    g_autoptr(GDateTime)         _last_updated;
    double                       _score;
    gboolean                     _enable_reconsuming;
    gint                         _downloaded_items;
    gint                         _times_consumed;
    MalgtkMalitemReconsumeValue  _reconsume_value;
    MalgtkMalitemPriority        _priority;
    gboolean                     _enable_discussion;
    gboolean                     _has_details;

    g_object_get (G_OBJECT(item),
                  "mal-db-id",          &_mal_db_id,
                  "series-title",       &_series_title,
                  "series-date-begin",  &_series_begin,
                  "series-date-end",    &_series_end,
                  "season-begin",       &_season_begin,
                  "season-end",         &_season_end,
                  "image-url",          &_image_url,
                  "series-synonyms",    &_synonyms,
                  "tags",               &_tags,
                  "date-start",         &_date_start,
                  "date-finish",        &_date_finish,
                  "id",                 &_id,
                  "last-updated",       &_last_updated,
                  "score",              &_score,
                  "enable-reconsuming", &_enable_reconsuming,
                  "downloaded-items",   &_downloaded_items,
                  "times-consumed",     &_times_consumed,
                  "reconsume-value",    &_reconsume_value,
                  "priority",           &_priority,
                  "enable-discussion",  &_enable_discussion,
                  "has-details",        &_has_details,
                  NULL);

    g_assert_cmpint   (mal_db_id,          ==, _mal_db_id);
    g_assert_cmpstr   (series_title,       ==, _series_title);
    g_assert_true     (malgtk_date_is_equal (&series_begin, _series_begin));
    g_assert_true     (malgtk_date_is_equal (&series_end, _series_end));
    g_assert_cmpstr   (_season_begin,      ==, "Spring 1998");
    g_assert_cmpstr   (_season_end,        ==, "Spring 1999");
    g_assert_cmpstr   (image_url,          ==, _image_url);
    g_assert_cmpstr   (synonyms[0],        ==, _synonyms[0]);
    g_assert_cmpstr   (synonyms[1],        ==, _synonyms[1]);
    g_assert_null     (_synonyms[2]);
    g_assert_true     (_tags[0] == tags[0]);
    g_assert_null     (_date_start);
    g_assert_null     (_date_finish);
    g_assert_cmpint   (id,                 ==, _id);
    g_assert_true     (g_date_time_compare(last_updated, _last_updated) == 0);
    g_assert_cmpfloat (score,              ==, _score);
    g_assert_cmpint   (enable_reconsuming, ==, _enable_reconsuming);
    g_assert_cmpint   (downloaded_items,   ==, _downloaded_items);
    g_assert_cmpint   (times_consumed,     ==, _times_consumed);
    g_assert_cmpint   (reconsume_value,    ==, _reconsume_value);
    g_assert_cmpint   (priority,           ==, _priority);
    g_assert_cmpint   (enable_discussion,  ==, _enable_discussion);
    g_assert_cmpint   (has_details,        ==, _has_details);

    xmlFreeTextReader(reader);
}

static void
test_malitem_xmlget (MalitemFixture *fixture,
                     gconstpointer user_data)
{
    static const char xml[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<MALitem version=\"1\"><series_itemdb_id>1</series_itemdb_id><series_title>Cowboy Bebop</series_title><series_preferred_title></series_preferred_title><series_date_begin>1998-04-03</series_date_begin><series_date_end>1999-04-24</series_date_end><image_url>http://cdn.myanimelist.net/images/anime/4/19644.jpg</image_url><series_synonyms><series_synonym>Cowboy Bebop</series_synonym></series_synonyms><series_synopsis></series_synopsis><tags/><date_start>0000-00-00</date_start><date_finish>2000-01-25</date_finish><id>9755128</id><last_updated>1238650198</last_updated><score>9.000000</score><enable_reconsuming>0</enable_reconsuming><fansub_group></fansub_group><comments></comments><downloaded_items>0</downloaded_items><times_consumed>0</times_consumed><reconsume_value>Invalid Reconsume Value</reconsume_value><priority>Invalid Priority</priority><enable_discussion>1</enable_discussion><has_details>0</has_details></MALitem>\n";

    MalgtkMalitem   *item   = fixture->item;
    xmlBufferPtr     buffer = xmlBufferCreate();;
    xmlTextWriterPtr writer = xmlNewTextWriterMemory(buffer, 0);
    xmlTextReaderPtr reader = xmlReaderForMemory(xml, G_N_ELEMENTS(xml) - 1, NULL, NULL, 0);

    /* Set the state */
    xmlTextReaderRead(reader);
    malgtk_malitem_set_from_xml(item, reader);

    /* Now serialize the state */
    xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
    malgtk_malitem_get_xml(item, writer);
    xmlTextWriterEndDocument(writer);

    /* Check idempotent */
    const char *content = (char*)xmlBufferContent(buffer);
    g_assert_cmpstr(xml, ==, content);

    /* Free */
    xmlFreeTextReader(reader);
    xmlFreeTextWriter(writer);
    xmlBufferFree(buffer);
}

struct notify_ctx
{
    gchar *name;
    gint64 cnt;
};

static void
notify_counter (GObject    *gobject,
                GParamSpec *pspec,
                gpointer    user_data)
{
    struct notify_ctx *ctx = user_data;
    if (strcmp(pspec->name, ctx->name) == 0) {
        ++ctx->cnt;
    }
}

static
void season_counter(GObject    *gobject,
                    GParamSpec *pspec,
                    gpointer    user_data)
{
    gint64 *cnt = user_data;
    ++*cnt;
}

static void
test_malitem_notify (MalitemFixture *fixture,
                     gconstpointer user_data)
{
    struct notify_ctx ctx;
    gint64 season_cnt = 0;
    g_signal_connect(G_OBJECT(fixture->item), "notify",
                     G_CALLBACK (notify_counter),
                     &ctx);
    g_signal_connect(G_OBJECT(fixture->item), "notify::season-begin",
                     G_CALLBACK (season_counter),
                     &season_cnt);
    g_signal_connect(G_OBJECT(fixture->item), "notify::season-end",
                     G_CALLBACK (season_counter),
                     &season_cnt);

#define  TEST_NOTIFY(prop, val, val2)  do {                         \
        ctx.name = prop;                                            \
        ctx.cnt = 0;                                                \
        g_object_set(G_OBJECT(fixture->item), prop, val, NULL);     \
        g_assert_cmpint(ctx.cnt, ==, 1);                            \
        g_object_set(G_OBJECT(fixture->item), prop, val, NULL);     \
        g_assert_cmpint(ctx.cnt, ==, 1);                            \
        g_object_set(G_OBJECT(fixture->item), prop, val2, NULL);    \
        g_assert_cmpint(ctx.cnt, ==, 2); } while (0);

    MalgtkDate md1, md2;
    GDate *d1, *d2;
    malgtk_date_clear(&md1);
    malgtk_date_clear(&md2);
    malgtk_date_set_from_string(&md1, "2010-01-01");
    malgtk_date_set_from_string(&md2, "2010-01-02");
    d1 = malgtk_date_get_g_date(&md1);
    d2 = malgtk_date_get_g_date(&md2);
    GDateTime *dt1 = g_date_time_new_now_utc();
    GDateTime *dt2 = g_date_time_add_months(dt1, 1);

    gchar *strv1[] = {"Banana", "Apple", "Apple", NULL};
    gchar *strv2[] = {"Banana", "Apple", "Cherry", NULL};

    TEST_NOTIFY("mal-db-id", 123, 122);
    TEST_NOTIFY("series-title", "abc", "abd");
    TEST_NOTIFY("preferred-title", "abc", "abd");
    TEST_NOTIFY("series-date-begin", &md1, &md2);
    g_assert_cmpint(season_cnt, ==, 2);
    TEST_NOTIFY("series-date-end", &md1, &md2);
    g_assert_cmpint(season_cnt, ==, 4);
    TEST_NOTIFY("image-url", "abc", "abd");
    TEST_NOTIFY("series-synonyms", strv1, strv2);
    TEST_NOTIFY("series-synopsis", "abc", "abd");
    TEST_NOTIFY("tags", strv1, strv2);
    TEST_NOTIFY("date-start", d1, d2);
    TEST_NOTIFY("date-finish", d1, d2);
    TEST_NOTIFY("id", 123, 122);
    TEST_NOTIFY("last-updated", dt1, dt2);
    TEST_NOTIFY("score", 1.1, 2.1);
    TEST_NOTIFY("enable-reconsuming", TRUE, FALSE);
    TEST_NOTIFY("fansub-group", "Vivid", "Triad");
    TEST_NOTIFY("comments", "abc", "");
    TEST_NOTIFY("downloaded-items", 1, 2);
    TEST_NOTIFY("times-consumed", 1, 2);
    TEST_NOTIFY("reconsume-value", MALGTK_MALITEM_RECONSUME_VALUE_MEDIUM, MALGTK_MALITEM_RECONSUME_VALUE_INVALID);
    TEST_NOTIFY("priority", MALGTK_MALITEM_PRIORITY_HIGH, MALGTK_MALITEM_PRIORITY_INVALID);
    TEST_NOTIFY("enable-discussion", TRUE, FALSE);
    TEST_NOTIFY("has-details", TRUE, FALSE);

#undef TEST_NOTIFY
}

int main(int argc, char *argv[])
{
    setlocale (LC_ALL, "");
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/malgtk/malitem/season", MalitemFixture, NULL,
                malitem_fixture_set_up, test_malitem_test1,
                malitem_fixture_tear_down);

    g_test_add ("/malgtk/malitem/synonyms1", MalitemFixture, NULL,
                malitem_fixture_set_up, test_malitem_test2,
                malitem_fixture_tear_down);

    g_test_add ("/malgtk/malitem/synonyms2", MalitemFixture, NULL,
                malitem_fixture_set_up, test_malitem_test3,
                malitem_fixture_tear_down);

    g_test_add ("/malgtk/malitem/getset", MalitemFixture, NULL,
                malitem_fixture_set_up, test_malitem_test4,
                malitem_fixture_tear_down);

    g_test_add ("/malgtk/malitem/xmlset", MalitemFixture, NULL,
                malitem_fixture_set_up, test_malitem_xmlset,
                malitem_fixture_tear_down);

    g_test_add ("/malgtk/malitem/notify", MalitemFixture, NULL,
                malitem_fixture_set_up, test_malitem_notify,
                malitem_fixture_tear_down);

    g_test_add ("/malgtk/malitem/xmlget", MalitemFixture, NULL,
                malitem_fixture_set_up, test_malitem_xmlget,
                malitem_fixture_tear_down);

    int res = g_test_run ();

    return res;
}
