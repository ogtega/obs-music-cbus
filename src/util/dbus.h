#include <obs-module.h>
#include <dbus/dbus.h>

struct metadata {
	char *artist;
	char *title;
	char *album;
	char *artUrl;
};

extern DBusConnection *bus_get();
extern void bus_add_match(DBusConnection *conn);
extern struct metadata *bus_read_msg(DBusConnection *conn);
