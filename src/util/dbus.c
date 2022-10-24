#include "dbus.h"
#include <unistd.h>

DBusConnection *bus_get()
{
	DBusConnection *conn = NULL;
	DBusError err;
	dbus_error_init(&err);

	conn = dbus_bus_get(DBUS_BUS_SESSION, &err);

	if (dbus_error_is_set(&err)) {
		blog(LOG_ERROR, "Connection Error (%s)", err.message);
		dbus_error_free(&err);
	}

	if (NULL == conn) {
		exit(1);
	}

	return conn;
}

struct metadata *bus_read_msg(DBusConnection *conn)
{
	char *data = NULL;
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
			if (!dbus_message_iter_init(msg, &args))
				blog(LOG_WARNING, "Message has no arguments!");
			else if (DBUS_TYPE_STRING !=
				 dbus_message_iter_get_arg_type(&args))
				blog(LOG_WARNING, "Argument is not string!");
			else {
				dbus_message_iter_get_basic(&args, &data);
				blog(LOG_INFO, "Got Signal with value %s",
				     data);
			}
		}

		dbus_message_unref(msg);
	}

	return NULL;
}

void bus_add_match(DBusConnection *conn)
{
	DBusError err;
	dbus_error_init(&err);

	dbus_bus_add_match(
		conn,
		"type='signal', path='/org/mpris/MediaPlayer2', member='PropertiesChanged', interface='org.freedesktop.DBus.Properties'",
		&err);
	dbus_connection_flush(conn);

	if (dbus_error_is_set(&err)) {
		blog(LOG_ERROR, "Match Error (%s)", err.message);
		dbus_error_free(&err);
	}
}
