#include <obs-module.h>
#include <dbus/dbus.h>
#include <glib.h>
#include <gio/gio.h>

struct metadata {
	char *artist;
	char *title;
	char *album;
	char *artUrl;
};

extern GDBusConnection *bus_get();
extern void bus_add_match(GDBusConnection *conn);
extern struct metadata *bus_read_msg(GDBusConnection *conn);
