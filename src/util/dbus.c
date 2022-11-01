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
		gchar **artists = NULL;

		g_variant_lookup(metadict, "xesam:title", "s", &meta->title);
		g_variant_lookup(metadict, "xesam:album", "s", &meta->album);
		g_variant_lookup(metadict, "xesam:artist", "^a&s", &artists);

		if (artists)
			meta->by = g_strjoinv(", ", artists);

		bfree(meta->str);

		long len = 0;

		if (!artists || !meta->title) {
			len = (strlen(meta->by) + strlen(meta->title) + 2);

			meta->str = bzalloc(sizeof(char) * len);
			g_snprintf(meta->str, len, "%s%s ", meta->by,
				   meta->title);
		} else {
			len = (strlen(meta->by) + strlen(meta->title) + 5);

			meta->str = bzalloc(sizeof(char) * len);

			if (len - 5 > 0)
				g_snprintf(meta->str, len, "%s - %s ", meta->by,
					   meta->title);
		}

		blog(LOG_INFO, "%s - %s", meta->by, meta->title);
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
	GError *err = NULL;
	char *err_msg = NULL;

	g_dbus_connection_signal_unsubscribe(conn, id);
	g_dbus_connection_close_sync(conn, NULL, &err);

	if (err) {
		err_msg = g_dbus_error_encode_gerror(err);
		blog(LOG_ERROR, "Connection Error (%s)", err_msg);
		g_error_free(err);
		g_free(err_msg);
	}
}
