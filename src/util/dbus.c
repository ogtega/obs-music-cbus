#include "dbus.h"
#include <unistd.h>
#include <util/base.h>
#include <util/bmem.h>
#include <util/c99defs.h>

GDBusConnection *bus_get()
{
	GDBusConnection *conn = NULL;
	GError *err = NULL;
	char *err_msg = NULL;

	conn = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &err);

	if (err) {
		err_msg = g_dbus_error_encode_gerror(err);
		blog(LOG_ERROR, "Connection Error (%s)", err_msg);
		g_error_free(err);
		g_free(err_msg);
	}

	if (NULL == conn) {
		exit(1);
	}

	return conn;
}

void signal_cb(GDBusConnection *conn, const gchar *sender_name,
	       const gchar *object_path, const gchar *interface_name,
	       const gchar *signal_name, GVariant *params, gpointer data)
{
	UNUSED_PARAMETER(conn);
	UNUSED_PARAMETER(sender_name);
	UNUSED_PARAMETER(object_path);
	UNUSED_PARAMETER(interface_name);
	UNUSED_PARAMETER(signal_name);

	struct metadata *meta = data;
	g_autoptr(GVariant) dict = NULL;

	g_variant_get(params, "(s@a{sv}as)", NULL, &dict, NULL);

	GVariant *metadict = g_variant_lookup_value(dict, "Metadata",
						    G_VARIANT_TYPE_VARDICT);

	if (metadict && !pthread_mutex_lock(&meta->lock)) {
		char *buf = NULL;
		gchar *artist = NULL;
		g_autoptr(GVariantIter) artists;

		if (meta->artist) {
			bfree(meta->artist);
			meta->artist = NULL;
		}

		if (meta->title) {
			bfree(meta->title);
			meta->title = NULL;
		}

		if (meta->album) {
			bfree(meta->album);
			meta->album = NULL;
		}

		g_variant_lookup(metadict, "xesam:title", "s", &meta->title);
		g_variant_lookup(metadict, "xesam:album", "s", &meta->album);
		g_variant_lookup(metadict, "xesam:artist", "as", &artists);

		while (g_variant_iter_loop(artists, "s", &artist)) {
			if (meta->artist) {
				buf = bzalloc(sizeof(char) *
					      (strlen(meta->artist) +
					       strlen(artist) + 2));
			} else {
				buf = bzalloc(sizeof(char) *
					      (strlen(artist) + 1));
			}

			if (meta->artist) {
				strcat(buf, meta->artist);
				bfree(meta->artist);
				strcat(buf, ", ");
			}

			strcat(buf, artist);

			meta->artist = buf;
		}

		g_free(artist);
		g_variant_iter_free(artists);

		if (meta->str)
			bfree(meta->str);

		meta->str = bzalloc(sizeof(char) * (strlen(meta->artist) +
						    strlen(meta->title) + 3));

		sprintf(meta->str, "%s - %s", meta->artist, meta->title);

		blog(LOG_INFO, "%s - %s", meta->artist, meta->title);
		pthread_mutex_unlock(&meta->lock);
	}

	g_variant_unref(params);
}

guint bus_subscribe(GDBusConnection *conn, struct metadata *data)
{
	return g_dbus_connection_signal_subscribe(
		conn, NULL, "org.freedesktop.DBus.Properties",
		"PropertiesChanged", "/org/mpris/MediaPlayer2", NULL,
		G_DBUS_SIGNAL_FLAGS_NONE, signal_cb, data, NULL);
}

void bus_unsubscribe(GDBusConnection *conn, guint id)
{
	g_dbus_connection_signal_unsubscribe(conn, id);
}
