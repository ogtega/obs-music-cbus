#include "dbus.h"
#include <unistd.h>

GDBusConnection *bus_get()
{
	GDBusConnection *conn = NULL;
	GDBusError err;
	char *err_msg = NULL;
	dbus_error_init(&err);

	conn = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &err);

	if (err) {
		err_msg = g_dbus_error_encode_gerror(err);
		blog(LOG_ERROR, "Connection Error (%s)", err_msg);
		g_error_free(&err);
		free(err_msg);
	}

	if (NULL == conn) {
		exit(1);
	}

	return conn;
}

struct metadata *bus_read_msg(GDBusConnection *conn)
{
	char *iface = NULL;
	DBusMessageIter args;

	while (1) {
		dbus_connection_read_write(conn, 0);
		DBusMessage *msg = dbus_connection_pop_message(conn);

		// loop again if we haven't read a message
		if (NULL == msg) {
			sleep(1);
			continue;
		}

		// check if the message is a signal from the correct interface and with the correct name
		if (dbus_message_is_signal(msg,
					   "org.freedesktop.DBus.Properties",
					   "PropertiesChanged")) {
			// read the parameters
			if (!dbus_message_iter_init(msg, &args)) {
				blog(LOG_WARNING, "Message has no arguments!");
				continue;
			}

			dbus_message_iter_get_basic(&args, &iface);
			blog(LOG_INFO, "%s", iface);

			dbus_message_iter_next(&args);
		}

		dbus_message_unref(msg);
	}

	return NULL;
}

void bus_add_match(GDBusConnection *conn)
{
	GDBusError err;
	dbus_error_init(&err);

	dbus_bus_add_match(
		conn,
		"type='signal', path='/org/mpris/MediaPlayer2', member='PropertiesChanged', interface='org.freedesktop.DBus.Properties'",
		&err);
	dbus_connection_flush(conn);

	if (dbus_error_is_set(&err)) {
		blog(LOG_ERROR, "Match Error (%s)", err);
		dbus_error_free(&err);
	}
}
