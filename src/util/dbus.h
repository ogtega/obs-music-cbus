#include <obs-module.h>
#include <dbus/dbus.h>
#include <glib.h>
#include <gio/gio.h>

struct metadata {
	char *artist;
	char *title;
	char *album;
	char *artUrl;
	char *str;
	pthread_mutex_t lock;
};

extern GDBusConnection *bus_get();
extern guint bus_subscribe(GDBusConnection *conn, struct metadata *data);
extern void bus_unsubscribe(GDBusConnection *conn, guint id);
