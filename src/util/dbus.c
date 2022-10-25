#include "dbus.h"
#include <unistd.h>
#include <util/base.h>
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
		free(err_msg);
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

	gchar *source = NULL;
	g_autoptr(GVariant) dict = NULL;
	g_autoptr(GVariantIter) arr = NULL;

	g_variant_get(params, "(s@a{sv}as)", &source, &dict, &arr);

	GVariant *metadict = g_variant_lookup_value(dict, "Metadata",
						    G_VARIANT_TYPE_VARDICT);
	if (metadict) {
		g_variant_lookup(metadict, "xesam:title", "s", &meta->title);
		blog(LOG_INFO, "%s", meta->title);
	}
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
